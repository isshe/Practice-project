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

#define FILE_NAME               "/home/isshe/Music/WavinFlag.aac"
#define ERR_STREAM              stderr
#define OUT_SAMPLE_RATE         44100

static uint8_t      *audio_buf;
static int          audio_len;
static long long    audio_buf_index;

void get_file_name(char *filename, int argc, char *argv[]);
void open_and_find_inputfile_info(AVFormatContext **pformat_ctx, char *filename);
int get_audio_stream_index(AVFormatContext *pformat_ctx);
void fill_audio(void *udata, Uint8 *stream, int len);

int main(int argc, char *argv[])
{
    AVFrame             out_frame;
    AVFormatContext     *pformat_ctx = NULL;
    int                 audio_stream = 0;
    AVCodecContext      *pcodec_ctx = NULL;
    AVCodec             *pcodec = NULL;
    AVPacket            *ppacket = NULL;        //!
    AVFrame             *pframe = NULL;
    uint8_t             *out_buffer = NULL;     //
    int                 decode_len = 0;
    uint32_t            len = 0;
    int                 got_picture = 0;
    int                 index = 0;
    int64_t             in_channel_layout = 0;
    struct SwrContext   *swr_ctx = NULL;
    char                filename[256] = FILE_NAME;
    FILE                *output_fp = NULL;
    int                 convert_len = 0;
    int                 data_size = 0;
    //SDL
    SDL_AudioSpec       wanted_spec;


    get_file_name(filename, argc, argv);
    fprintf(ERR_STREAM, "file name: %s\n", filename);

    //about ffmpeg
    //init
    av_register_all();
    //SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
         fprintf(ERR_STREAM, "Couldn't init SDL: %s\n", SDL_GetError());
         exit(-1);
    }

    //open input file
//    pformat_ctx = avformat_alloc_context();
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


    out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);    
    audio_buf = out_buffer;
    
    pframe = av_frame_alloc();

    //SDL_AudioSpec
    //nb_samples: AAC-1024 MP3-1152
    //1. AV_CH_LAYOUT_STEREO: 立体
    //2. 样本数, 当以音频流打开的时候这个不为0，当以视频打开是时候，为0，要注意。
    //雷神的代码中是使用了固定值1024（AAC音频为1024， MP3为1152）
    //3. 样本格式。在一个enum里
    //4. 采样率：44100， 每秒有44100个样本。
    //5. 通道数
    //6. 输出缓冲区大小
    wanted_spec.freq        = pcodec_ctx->sample_rate;  //样本率44100
    wanted_spec.format      = AUDIO_S16SYS;     //样本格式
    wanted_spec.channels    = pcodec_ctx->channels;     //1-单声道，2-立体
    wanted_spec.silence     = 0;                //静音值？？？
    wanted_spec.samples     = 1024;             //输出样本数
    wanted_spec.callback    = fill_audio;       //回调函数
    wanted_spec.userdata    = pcodec_ctx;       //用户数据

    //打开音频设备， 成功返回0
    if (SDL_OpenAudio(&wanted_spec, NULL) < 0)
    {
         fprintf(ERR_STREAM, "Couldn't open Audio\n");
         exit(-1);
    }

    out_frame.format         = AV_SAMPLE_FMT_S16;
    out_frame.sample_rate    = wanted_spec.freq;
    out_frame.channels       = wanted_spec.channels;
    out_frame.channel_layout = av_get_default_channel_layout(wanted_spec.channels);

    //打开解码器
    if (avcodec_open2(pcodec_ctx, pcodec, NULL) < 0)
    {
         fprintf(ERR_STREAM, "Couldn't open decoder\n");
         exit(-1);
    }

    fprintf(ERR_STREAM, "Bit rate: %d\n", pformat_ctx->bit_rate);
    fprintf(ERR_STREAM, "Decoder Name = %s\n", pcodec_ctx->codec->long_name);
    fprintf(ERR_STREAM, "Channels: %d\n", pcodec_ctx->channels);
    fprintf(ERR_STREAM, "Sample per Second: %d\n", pcodec_ctx->sample_rate);

    ppacket = (AVPacket *)av_malloc(sizeof(AVPacket));
    av_init_packet(ppacket);
    //播放
    SDL_PauseAudio(0);      //???
    while( av_read_frame(pformat_ctx, ppacket) >= 0 )
    {
        printf("pakcet size = %d\n", ppacket->size);
        if (ppacket->stream_index == audio_stream)
        {
            decode_len = avcodec_decode_audio4(pcodec_ctx, pframe, &got_picture, ppacket);
            if (decode_len < 0)
            {
                fprintf(ERR_STREAM, "Couldn't decode audio frame\n");
                continue;       //
            }

            if (got_picture)
            {
                   if (swr_ctx != NULL)
                   {
                        swr_free(&swr_ctx);
                        swr_ctx = NULL;
                   }

                   swr_ctx = swr_alloc_set_opts(NULL, out_frame.channel_layout,
                          (AVSampleFormat)out_frame.format,out_frame.sample_rate,
                          pframe->channel_layout,(AVSampleFormat)pframe->format,
                          pframe->sample_rate, 0, NULL);
                          
                  //初始化
                  if (swr_ctx == NULL || swr_init(swr_ctx) < 0)
                  {
                       fprintf(ERR_STREAM, "swr_init error\n");
                       break;
                  }
                    
                  convert_len = swr_convert(swr_ctx, &audio_buf,
                                    MAX_AUDIO_FRAME_SIZE,
                                    (const uint8_t **)pframe->data, 
                                    pframe->nb_samples);
            }
            

                            
         printf("decode len = %d, convert_len = %d\n", decode_len, convert_len);
            //回到缓冲区头，继续播放数据
            audio_buf_index = 0;
            audio_buf   =  out_buffer;
            audio_len   =  out_frame.channels * convert_len * av_get_bytes_per_sample((AVSampleFormat)out_frame.format);
            
            
            while(audio_len > 0)
            {
                 SDL_Delay(1);  //停1微秒
            }
            
        }
        av_init_packet(ppacket);
//        av_free_packet(ppacket);
    }

    swr_free(&swr_ctx);
    SDL_CloseAudio();
    SDL_Quit();

    fclose(output_fp);

    av_free(out_buffer);
    avcodec_close(pcodec_ctx);
    avformat_close_input(&pformat_ctx);

    return 0;
}

void fill_audio(void *udata, Uint8 *stream, int len)
{
    printf("回调函数中：len = %d, audio_len = %d\n", len, audio_len);
    SDL_memset(stream, 0, len);
    
    if (audio_len == 0)
    {
         return ;
    }
    len = len > audio_len ? audio_len : len;

    SDL_MixAudio(stream, (uint8_t*)audio_buf + audio_buf_index, len, SDL_MIX_MAXVOLUME);
    audio_buf_index += len;
    audio_len       -= len;
    stream          += len;
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


