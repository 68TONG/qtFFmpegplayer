#ifndef SDLAUDIO_H
#define SDLAUDIO_H

#include "queue.h"

extern "C"
{
#include "SDL.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include "libavformat/avformat.h"
}

void SDL_out_funcback(void *data, Uint8 *stream, int len);

typedef struct AudioParameters
{
    int frequency;
    int sampels;
    int channels;
    uint64_t channel_layout;
    AVSampleFormat fmt;

} AudioParameters;

class SDLAudio
{
public:
    SDLAudio();

    ~SDLAudio();

    bool InitAudio_SDL(AVCodecContext *codec_ctx);

    void Strat();

    void Stop();

    void setVolume(unsigned int value);

    void releaseThis();

    void initThis(AVStream *stream, uint64_t *us, Queue<AVFrame *> *que, bool is);

    SDL_AudioDeviceID device_id;

    SwrContext *swr_context = NULL;

    uint8_t *audio_buffer = NULL;
    uint8_t *audio_buffer_copy = NULL;
    unsigned int audio_buffer_set = 0;
    uint32_t audio_buffer_size = 0;
    uint32_t audio_buffer_index = 0;

    AudioParameters audiopar_src;
    AudioParameters audiopar_dst;

    Queue<AVFrame *> *que;

    uint64_t *main_us = NULL;
    uint64_t play_us = 0;
    double time_base = 0;
    double volume = 1.0;
    bool is_main_stream = false;
    unsigned int ffoset_byte = 0;
    unsigned int secodes_byte = 0;
};

#endif // SDLAUDIO_H
