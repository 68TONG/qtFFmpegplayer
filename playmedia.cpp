#include "playmedia.h"

#include <iostream>
using namespace std;

PlayMedia::PlayMedia()
{

}

PlayMedia::~PlayMedia()
{
   releaseThis();
}

bool PlayMedia::setPlayMedia(const char *path)
{
    memset(media_url, 0, sizeof (media_url));
    strcpy(media_url, path);

    format_context = avformat_alloc_context();

    int sic = avformat_open_input(&format_context, path, NULL, NULL);
    if(sic < 0){
        av_strerror(sic, error_str, sizeof (error_str));
        cout << "avformat_open_input: " << error_str << endl;
        return false;
    }

    sic = avformat_find_stream_info(format_context, NULL);
    if(sic < 0){
        av_strerror(sic, error_str, sizeof (error_str));
        cout << "avformat_find_stream_info: " << error_str << endl;
        return false;
    }

    vector<pair<int, char *>> list;
    for(unsigned int i = 0;i < format_context->nb_streams;i++){
        int id = format_context->streams[i]->index;
        streamHash.insert(std::pair<int, AVStream *>(id, format_context->streams[i]));
        AVCodecParameters *codec_par = format_context->streams[i]->codecpar;
        list.push_back({id, (char *)avcodec_get_name(codec_par->codec_id)});

        if(defaultAudioId < 0 && codec_par->codec_type == AVMEDIA_TYPE_AUDIO)
            defaultAudioId = id;
        if(defaultVideoId < 0 && codec_par->codec_type == AVMEDIA_TYPE_VIDEO)
            defaultVideoId = id;
    }

    if(defaultAudioId >= 0){
        codec_ctxs[defaultAudioId] = NULL;
        if(openCodec(&codec_ctxs[defaultAudioId], streamHash[defaultAudioId]->codecpar) == false) return false;
//        cout << streamHash[defaultAudioId]->time_base.den << ' ' << streamHash[defaultAudioId]->time_base.num << endl;
    }
    if(defaultVideoId >= 0){
        codec_ctxs[defaultVideoId] = NULL;
        if(openCodec(&codec_ctxs[defaultVideoId], streamHash[defaultVideoId]->codecpar) == false) return false;
//        cout << streamHash[defaultVideoId]->time_base.den << ' ' << streamHash[defaultVideoId]->time_base.num << endl;
    }
    return true;
}

bool PlayMedia::openCodec(AVCodecContext **codec_ctx, AVCodecParameters *codec_par)
{
    const AVCodec *codec = avcodec_find_decoder(codec_par->codec_id);
    if(codec == NULL){
        cout << "avcodec_find_decoder: find codec falied" << endl;
        return false;
    }

    *codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(*codec_ctx, codec_par);
    if(*codec_ctx == NULL){
        cout << "avcodec_alloc_context3: alloc failed" << endl;
        return false;
    }

    int sic = avcodec_open2(*codec_ctx, codec, NULL);
    if(sic < 0){
        av_strerror(sic, error_str, sizeof (error_str));
        cout << "avcodec_open2: " << error_str << endl;
        return false;
    }
    return true;
}

bool PlayMedia::startDemuxtThread(std::vector<int> listId)
{
    if(format_context == NULL){
        cout << "startDemuxtThread: format_context == NULL" << endl;
        return false;
    }
    demuxt_thread = new thread(&PlayMedia::runDemuxt, this);
    abort = false;
    if(demuxt_thread == NULL){
        cout << "startDemuxtThread: failed" << endl;
        return false;
    }
    if(listId.empty()) streamId = {defaultAudioId, defaultVideoId};
    else streamId = listId;
    return true;
}

bool PlayMedia::startDeCodeThread()
{
    for(unsigned int i = 0;i < streamId.size();i++){
        unsigned int id = streamId[i];
        if(streamHash.count(id) == 0){
            cout << "startDecodeThread: find stream == NULL" << endl;
            continue;
        }
        codec_thread[id] = new thread(&PlayMedia::runDeCode, this, &packet_que[id], &frame_que[id], codec_ctxs[id]);
        if(codec_thread[id] == NULL){
            cout << "codec_thread create id: " << id << "failed" << endl;
            continue;
        }
    }
    return true;
}


bool PlayMedia::seekMedia(int offset)
{
    Changed_us = offset;
    is_seek = true;
    cout << offset << endl;
}

void PlayMedia::runDemuxt()
{
    while(abort == false){
        int sic = -1;
        for(unsigned int i = 0;i < streamId.size();i++)
        {
            if(packet_que[streamId[i]].Size() > 100){
                this_thread::sleep_for(chrono::milliseconds(10));
                sic = 0;
                break;
            }
        }
        if(sic == 0) continue;
        if(is_seek){
            sic = avformat_seek_file(format_context, -1, INT64_MIN, Changed_us, INT64_MAX, AVSEEK_FLAG_BACKWARD);
            if(sic < 0){
                cout << "avformat_seek_file: failed" << endl;
                continue;
            }
            for(unsigned int i = 0;i < streamId.size();i++){
                AVPacket *packet = av_packet_alloc();
                int id = streamId[i];
                clearQueuePacket(&packet_que[id]);
                packet->data = (uint8_t *)FLUSH_DATA;
                packet_que[id].Push(packet);
            }
            is_seek = false;
            continue;
        }
        AVPacket *packet = av_packet_alloc();
        sic = av_read_frame(format_context, packet);
        if(sic < 0){
            av_strerror(sic, error_str, sizeof (error_str));
            cout << "av_read_frame:" << error_str << endl;
            for(unsigned int i = 0;i < streamId.size();i++){
                packet_que[i].Abort(true);
            }
            return ;
        }
        sic = true;
        for(unsigned int i = 0;i < streamId.size();i++){
            if(packet->stream_index == streamId[i]){
                packet_que[packet->stream_index].Push(packet);
                sic = false;
                break;
            }
        }
        if(sic)av_packet_free(&packet);
    }
}

void PlayMedia::runDeCode(Queue<AVPacket *> *que_packet, Queue<AVFrame *> *que_frame, AVCodecContext *codec_ctx)
{
    if(codec_ctx == NULL){
        cout << "runDeCode: codec_ctx == NULL" << endl;
        return;
    }
    AVPacket *packet = NULL;
    while(abort == false){
        if(que_frame->Size() > 10){
            this_thread::sleep_for(chrono::milliseconds(10));
            continue;
        }
        int sic = que_packet->Pop(packet, 10);
        if(sic == 0){
            if(que_packet->getAbort() && que_packet->Size() == 0){
                abort = true;
                que_frame->Abort(true);
                return;
            }
            continue;
        }

        if(strcmp((char *)packet->data, FLUSH_DATA) == 0){
            avcodec_flush_buffers(codec_ctx);
            clearQueueFrame(que_frame);
            av_packet_free(&packet);
            continue;
        }
        sic = avcodec_send_packet(codec_ctx, packet);
        av_packet_free(&packet);
        if(sic < 0){
            av_strerror(sic, error_str, sizeof (error_str));
            cout << "avcodec_send_packet: " << error_str << endl;
            break;
        }

        while(true){
            AVFrame *frame = av_frame_alloc();
            sic = avcodec_receive_frame(codec_ctx, frame);
            if(sic == 0){
                que_frame->Push(frame);
            } else if(sic == AVERROR(EAGAIN)){
                av_frame_free(&frame);
                break;
            } else {
                abort = true;
                av_strerror(sic, error_str, sizeof (error_str));
                cout << "avcodec_receive_frame: " << error_str << endl;
                av_frame_free(&frame);
                break;
            }
        }
    }
}

void PlayMedia::releaseThis()
{
    abort = true;

    defaultAudioId = defaultVideoId = -1;
    if(demuxt_thread){
        demuxt_thread->join();
        delete demuxt_thread;
        demuxt_thread = NULL;
    }
    if(format_context){
        avformat_close_input(&format_context);
        format_context = NULL;
    }
    for(unsigned int i = 0;i < streamId.size();i++){
        unsigned int id = streamId[i];
        if(codec_ctxs[id]) avcodec_close(codec_ctxs[id]);

        clearQueuePacket(&packet_que[id]);

        clearQueueFrame(&frame_que[id]);

        thread *thread = codec_thread[id];
        if(thread){
            thread->join();
            delete thread;
        }
    }
    codec_ctxs.clear();
    codec_thread.clear();
    streamHash.clear();
    streamId.clear();
    packet_que.clear();
    frame_que.clear();
}

void PlayMedia::clearQueuePacket(Queue<AVPacket *> *que)
{
    if(que == NULL)return;
    AVPacket *val = NULL;
    while(que->Pop(val, 10)){
        av_packet_free(&val);
        val = NULL;
    }
}

void PlayMedia::clearQueueFrame(Queue<AVFrame *> *que)
{
    if(que == NULL)return;
    AVFrame *val = NULL;
    while(que->Pop(val, 10)){
        av_frame_free(&val);
        val = NULL;
    }
}
