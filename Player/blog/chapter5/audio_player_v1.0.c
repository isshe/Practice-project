/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : audio_player.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/02
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __STDC_CONSTANT_MACROS      //ffmpeg要求

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>

#ifdef __cplusplus
}
#endif

#define MAX_AUDIO_FRAME_SIZE    192000  //1 second of 48khz 32bit audio

#define OUTPUT_PCM              1
#define USE_SDL                 1

#define FILE_NAME               "/home/isshe/Music/2.flac"
#define ERR_STREAM              stderr
#define OUT_SAMPLE_RATE         44100

static Uint8    *audio_chunk;
static Uint32   audio_len;
static Uint8    *audio_pos;

void get_file_name(char *filename, int argc, char *argv[]);
void open_and_find_inputfile_info(AVFormatContext **pformat_ctx, char *filename);
int get_audio_stream_index(AVFormatContext *pformat_ctx);
void fill_audio(void *udata, Uint8 *stream, int len);

int main(int argc, char *argv[])
{
    AVFormatContext     *pformat_ctx = NULL;
    int                 audio_stream = 0;
    AVCodecContext      *pcodec_ctx = NULL;
    AVCodec             *pcodec = NULL;
    AVPacket            *ppacket = NULL;        //!
    AVFrame             *pframe = NULL;
    uint8_t             *out_buffer = NULL;     //
    uint32_t            ret = 0;
    uint32_t            len = 0;
    int                 got_picture = 0;
    int                 index = 0;
    int64_t             in_channel_layout = 0;
    struct SwrContext   *au_convert_ctx = NULL;
    char                filename[256] = FILE_NAME;
    FILE                *output_fp = NULL;

    uint64_t            out_channel_layout = 0;
    int                 out_nb_samples = 0;
    AVSampleFormat      out_sample_fmt;
    int                 out_sample_rate = 0;
    int                 out_buffer_size = 0;
    int                 out_channels = 0;

    //SDL
    SDL_AudioSpec       wanted_spec;


    get_file_name(filename, argc, argv);
    fprintf(ERR_STREAM, "file name: %s\n", filename);

    //about ffmpeg
    //init
    av_register_all();
    avformat_network_init();

    //open input file
    pformat_ctx = avformat_alloc_context();
/*    if (avformat_open_input(&pformat_ctx, filename, NULL, NULL) != 0)
    {
        fprintf(ERR_STREAM, "Couldn't open input file\n");
        exit(-1);
    }

    //取得流信息
    if (avformat_find_stream_info(pformat_ctx, NULL) < 0)
*/
    open_and_find_inputfile_info(&pformat_ctx, filename);
    av_dump_format(pformat_ctx, 0, filename, false);        //输出文件信息，十分好用的一个函数
    audio_stream = get_audio_stream_index(pformat_ctx);

    pcodec_ctx = pformat_ctx->streams[audio_stream]->codec;

    //找到一个相应的解码器
    pcodec = avcodec_find_decoder(pcodec_ctx->codec_id);
    if (pcodec == NULL)
    {
         fprintf(ERR_STREAM, "Codec not found\n");
         exit(-1);
    }

    //打开解码器
    if (avcodec_open2(pcodec_ctx, pcodec, NULL) < 0)
    {
         fprintf(ERR_STREAM, "Couldn't open decoder\n");
         exit(-1);
    }

    output_fp = fopen("output.pcm", "w+");


    //out Audio param
    //nb_samples: AAC-1024 MP3-1152
    //1. AV_CH_LAYOUT_STEREO: 立体
    //2. 样本数, 当以音频流打开的时候这个不为0，当以视频打开是时候，为0，要注意。
    //雷神的代码中是使用了固定值1024（AAC音频为1024， MP3为1152）
    //3. 样本格式。在一个enum里
    //4. 样本率：44100， 每秒有44100个样本。
    //5. 通道数
    //6. 输出缓冲区大小
    out_channel_layout = AV_CH_LAYOUT_STEREO;
    out_nb_samples = 1024; //pcodec_ctx->frame_size;
    out_sample_fmt = AV_SAMPLE_FMT_S16;
    out_sample_rate = OUT_SAMPLE_RATE;         //44100
    out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
    out_buffer_size = av_samples_get_buffer_size(NULL,
            out_channels, out_nb_samples, out_sample_fmt, 1);

    out_buffer = (uint8_t *)av_malloc(out_buffer_size + 1); //MAX_AUDIO_FRAME_SIZE * 2);    //???out_buffer_size * 2);
    pframe = av_frame_alloc();

    //SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
         fprintf(ERR_STREAM, "Couldn't init SDL: %s\n", SDL_GetError());
         exit(-1);
    }

    //SDL_AudioSpec
    wanted_spec.freq        = out_sample_rate;  //样本率
    wanted_spec.format      = AUDIO_S16SYS;     //样本格式
    wanted_spec.channels    = out_channels;     //1-单声道，2-立体
    wanted_spec.silence     = 0;                //静音值？？？
    wanted_spec.samples     = out_nb_samples;   //输出样本数
    wanted_spec.callback    = fill_audio;       //回调函数
    wanted_spec.userdata    = pcodec_ctx;       //用户数据

    //打开音频设备， 成功返回0
    if (SDL_OpenAudio(&wanted_spec, NULL) < 0)
    {
         fprintf(ERR_STREAM, "Couldn't open Audio\n");
         exit(-1);
    }

    fprintf(ERR_STREAM, "Bit rate: %d\n", pformat_ctx->bit_rate);
    fprintf(ERR_STREAM, "Decoder Name = %s\n", pcodec_ctx->codec->long_name);
    fprintf(ERR_STREAM, "Channels: %d\n", pcodec_ctx->channels);
    fprintf(ERR_STREAM, "Sample per Second: %d\n", pcodec_ctx->sample_rate);

    in_channel_layout   = av_get_default_channel_layout(pcodec_ctx->channels);

    au_convert_ctx      = swr_alloc();
    //设置参数
    au_convert_ctx      = swr_alloc_set_opts(au_convert_ctx, out_channel_layout,
            out_sample_fmt, out_sample_rate, in_channel_layout,
            pcodec_ctx->sample_fmt, pcodec_ctx->sample_rate, 0, NULL);
    swr_init(au_convert_ctx);

    ppacket = (AVPacket *)av_malloc(sizeof(AVPacket));
    av_init_packet(ppacket);

    //播放
//    SDL_PauseAudio(0);      //???
    while( av_read_frame(pformat_ctx, ppacket) >= 0 )
    {
        if (ppacket->stream_index == audio_stream)
        {
            ret = avcodec_decode_audio4(pcodec_ctx, pframe, &got_picture, ppacket);
            if (ret < 0)
            {
                fprintf(ERR_STREAM, "Couldn't decode audio frame\n");
                continue;       //
            }

            if (got_picture > 0)
            {
                 swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE,
                         (const uint8_t **)pframe->data, pframe->nb_samples);
                 fprintf(ERR_STREAM, "index:%5d\t pts:%lld\t packet size:%d, wanted_spec.sample: %d:%d\n",
                         index, ppacket->pts, ppacket->size, wanted_spec.samples, pframe->nb_samples);

                 if (wanted_spec.samples != pframe->nb_samples)
                 {
                     SDL_CloseAudio();
                     out_nb_samples = pframe->nb_samples;
                     out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, out_sample_fmt, 1);
                     wanted_spec.samples = out_nb_samples;
                     SDL_OpenAudio(&wanted_spec, NULL);
                 }

//                 fwrite(out_buffer, 1, out_buffer_size, output_fp);
//                 index++;
            }

//            while(audio_len > 0)
//            {
//                 SDL_Delay(1);  //停1ms
//            }

            //回到缓冲区头，继续播放数据
            audio_chunk = (Uint8 *)out_buffer;
            audio_len   = out_buffer_size;
            audio_pos   = audio_chunk;

            SDL_PauseAudio(0);
            while(audio_len > 0)
            {
                 SDL_Delay(1);  //停1微秒
            }
        }
        av_free_packet(ppacket);
    }

    swr_free(&au_convert_ctx);
    SDL_CloseAudio();
    SDL_Quit();

    fclose(output_fp);

    av_free(out_buffer);
    avcodec_close(pcodec_ctx);
    avformat_close_input(&pformat_ctx);

    return 0;
}

void get_file_name(char *filename, int argc, char *argv[])
{
     if (argc == 2)
     {
         memcpy(filename, argv[1], strlen(argv[1]) + 1);
     }
     else if (argc > 2)
     {
          fprintf(ERR_STREAM, "Usage: ./*.out audio_file.mp3\n");
          exit(-1);
     }
}

void open_and_find_inputfile_info(AVFormatContext **pformat_ctx, char *filename)
{
     if (avformat_open_input(pformat_ctx, filename, NULL, NULL) != 0)
     {
          fprintf(ERR_STREAM, "Couldn' open input file\n");
          exit(-1);
     }

     if (avformat_find_stream_info(*pformat_ctx, NULL) < 0)
     {
          fprintf(ERR_STREAM, "Couldn' find stream info\n");
          exit(-1);
     }
}

int get_audio_stream_index(AVFormatContext *pformat_ctx)
{
     int i = 0;
     int audio_stream = -1;

     for (i = 0; i < pformat_ctx->nb_streams; i++)
     {
          if (pformat_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
          {
              audio_stream = i;
              break;
          }
     }

     if (audio_stream == -1)
     {
          fprintf(ERR_STREAM, "Didn't find audio stream\n");
          exit(-1);
     }

     return audio_stream;
}

void fill_audio(void *udata, Uint8 *stream, int len)
{
    SDL_memset(stream, 0, len);
    if (audio_len == 0)
    {
         return ;
    }
    len = (len > audio_len ? audio_len : len);

    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}
