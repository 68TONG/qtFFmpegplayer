#include "sdlaudio.h"
#include <iostream>
#include <thread>
using namespace std;

void SDL_out_funcback(void *data, Uint8 *stream, int len)
{
    AVFrame *frame = NULL;
    SDLAudio *is = (SDLAudio *)data;
    while(len > 0){
        if(is->audio_buffer_index < is->audio_buffer_size){
            int out_len = is->audio_buffer_size - is->audio_buffer_index;
            out_len = out_len > len ? len : out_len;
            memcpy(stream, is->audio_buffer + is->audio_buffer_index, out_len);
            len -= out_len;
            is->audio_buffer_index -= out_len;
            stream += out_len;
            continue;
        }
        if(is->que->Pop(frame, 10)){
            is->play_us = (frame->pts / is->time_base) * 1000000;
            if(is->is_main_stream) *is->main_us = is->play_us;
            else {
                int64_t us = is->play_us - *is->main_us,
                        duration = frame->duration / is->time_base * 1000000;
                if(us < duration * -1){
                    av_frame_free(&frame);
                    return;
                } else if(us > duration){
                    this_thread::sleep_for(chrono::milliseconds(us/1000));
                }
            }

            is->audio_buffer_index = 0;
            is->audio_buffer_size = av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples, (enum AVSampleFormat)frame->format, 1);
            if((is->swr_context == NULL && frame->nb_samples != is->audiopar_dst.sampels) ||
                    (is->audiopar_dst.fmt != frame->format) ||
                    (is->audiopar_dst.channels != frame->channels) ||
                    (is->audiopar_dst.frequency != frame->sample_rate)){
                swr_free(&is->swr_context);
                is->swr_context = swr_alloc_set_opts(NULL, is->audiopar_src.channel_layout, is->audiopar_src.fmt, is->audiopar_src.frequency,
                                                     frame->channel_layout, (enum AVSampleFormat)frame->format, frame->sample_rate, 0, NULL);
                if(is->swr_context == NULL || swr_init(is->swr_context) < 0){
                    cout << "swr_init: failed" << endl;
                    swr_free(&is->swr_context);
                    is->swr_context = NULL;
                    break;
                }
                is->audiopar_dst.frequency = frame->sample_rate;
                is->audiopar_dst.fmt = (enum AVSampleFormat)frame->format;
                is->audiopar_dst.channels = frame->channels;
                is->audiopar_dst.channel_layout = frame->channel_layout;
            }
            if(is->swr_context){
                uint8_t **out = &is->audio_buffer_copy;
                int out_sample_count = frame->nb_samples * is->audiopar_src.frequency / frame->sample_rate + 256;
                int out_size = av_samples_get_buffer_size(NULL, is->audiopar_src.channels, out_sample_count, is->audiopar_src.fmt, 0);
                if(out_size < 0){
                    cout << "av_samples_get_buffer_size: get size < 0" << endl;
                    break;
                }
                av_fast_malloc(out, &is->audio_buffer_set, out_size);
                int samples = swr_convert(is->swr_context, out, out_sample_count, (const uint8_t **)frame->extended_data, frame->nb_samples);
                if(samples < 0){
                    cout << "swr_convert return sample < 0" << endl;
                    break;
                }
                is->audio_buffer = is->audio_buffer_copy;
                is->audio_buffer_size = samples * is->audiopar_src.channels * av_get_bytes_per_sample(is->audiopar_src.fmt);
            } else {
                memcpy(is->audio_buffer, frame->data[0], is->audio_buffer_size);
            }
            av_frame_free(&frame);
        } else {
            memset(is->audio_buffer, 0, 1024);
            is->audio_buffer_size = 1024;
        }
    }
}

SDLAudio::SDLAudio()
{

}

SDLAudio::~SDLAudio()
{
    releaseThis();
}

bool SDLAudio::InitAudio_SDL(AVCodecContext *codec_ctx)
{
    if(SDL_Init(SDL_INIT_AUDIO) != 0){
        cout << "SDL_Init(Audio): failed" << endl;
        return false;
    }
    SDL_AudioSpec src_spec, dst_spec;
    dst_spec.freq = codec_ctx->sample_rate;
    dst_spec.format = AUDIO_S16SYS;
    dst_spec.channels = codec_ctx->channels;
    dst_spec.samples = codec_ctx->frame_size;
    dst_spec.silence = 0;
    dst_spec.callback = SDL_out_funcback;
    dst_spec.userdata = this;

    if(!(device_id = SDL_OpenAudioDevice(NULL, 0, &dst_spec, &src_spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE))){
        cout << "SDL_OpenAudioDevice: device_id == 0" << endl;
        return false;
    };

    audiopar_src.frequency = audiopar_dst.frequency = src_spec.freq;
    audiopar_src.fmt = audiopar_dst.fmt =AV_SAMPLE_FMT_S16;
    audiopar_src.channels = audiopar_dst.channels = src_spec.channels;
    audiopar_src.sampels = audiopar_dst.sampels = src_spec.samples;
    audiopar_src.channel_layout = audiopar_dst.channel_layout = av_get_default_channel_layout(src_spec.channels);
    int size = av_samples_get_buffer_size(NULL, audiopar_src.channels, audiopar_src.sampels, audiopar_src.fmt, 1);
    av_fast_malloc(&audio_buffer_copy, &audio_buffer_set, size);
    audio_buffer = audio_buffer_copy;
    return true;
}

void SDLAudio::Strat()
{
    SDL_PauseAudioDevice(device_id, false);
}

void SDLAudio::Stop()
{
    SDL_PauseAudioDevice(device_id, true);
}

void SDLAudio::setVolume(unsigned int value)
{
    volume = value * (2.0 / 100);
}

void SDLAudio::releaseThis()
{
    SDL_CloseAudioDevice(device_id);
    SDL_Quit();

    if(swr_context){
        swr_free(&swr_context);
        swr_context = NULL;
    }

    if(is_main_stream){
        delete main_us;
        main_us = NULL;
    }
//    free(audio_buffer);
//    audio_buffer = audio_buffer_copy = NULL;
}

void SDLAudio::initThis(AVStream *stream, uint64_t *us, Queue<AVFrame *> *que, bool is)
{
    this->time_base = (double)stream->time_base.den / stream->time_base.num;

    this->main_us = us;
    this->que = que;
    this->is_main_stream = is;
    audiopar_dst = audiopar_src = AudioParameters();
    audio_buffer_size = audio_buffer_index = 0;
    play_us = 0;
    volume = 1.0;

    if(swr_context){
        swr_free(&swr_context);
        swr_context = NULL;
    }
    SDL_CloseAudioDevice(device_id);
}
