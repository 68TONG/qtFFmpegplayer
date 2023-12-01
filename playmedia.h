#ifndef PLAYMEDIA_H
#define PLAYMEDIA_H

#include <thread>
#include <map>
#include <vector>

#include "queue.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/time.h"
}

#define FLUSH_DATA "flush_data"

class PlayMedia
{
public:
    PlayMedia();

    ~PlayMedia();

    bool setPlayMedia(const char *path);

    bool startDemuxtThread(std::vector<int> listId);

    bool startDeCodeThread();

    bool seekMedia(int offset);

    bool openCodec(AVCodecContext **codec_ctx, AVCodecParameters *codec_par);

    void runDemuxt();

    void runDeCode(Queue<AVPacket *> *que_packet, Queue<AVFrame *> *que_frame, AVCodecContext *codec_ctx);

    void releaseThis();

    void clearQueuePacket(Queue<AVPacket *> *que);

    void clearQueueFrame(Queue<AVFrame *> *que);

    std::map<int, AVStream *> streamHash;
    int defaultVideoId = -1, defaultAudioId = -1;

    std::vector<int> streamId;

    AVFormatContext *format_context = NULL;

    std::map<int, Queue<AVPacket *>> packet_que;

    std::map<int, AVCodecContext *> codec_ctxs;

    std::map<int, Queue<AVFrame *>> frame_que;

    std::map<int, std::thread *> codec_thread;

    std::thread *demuxt_thread = NULL;

    bool abort = false;

    bool is_seek = false;

    int64_t Changed_us = 0;

    char error_str[256];

    char media_url[256];
};

#endif // PLAYMEDIA_H
