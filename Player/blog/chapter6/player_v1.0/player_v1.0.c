/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : player.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/05
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
#include <assert.h>
#include <unistd.h>

#define __STDC_CONSTANT_MACROS      //ffmpeg要求

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <SDL2/SDL.h>

#ifdef __cplusplus
}
#endif

//
#include "wrap_base.h"
#include "packet_queue.h"

#define AVCODE_MAX_AUDIO_FRAME_SIZE    192000  //1 second of 48khz 32bit audio
#define SDL_AUDIO_BUFFER_SIZE   1024    //

#define FILE_NAME               "/home/isshe/Music/WavinFlag.aac"
#define ERR_STREAM              stderr
#define OUT_SAMPLE_RATE         44100

#define OUT_STREAM              stdout
#define WINDOW_W                640
#define WINDOW_H                320
#define MY_DEFINE_REFRESH_EVENT (SDL_USEREVENT + 1)
#define MY_DEFINE_BREAK_EVENT   (SDL_USEREVENT + 2)


void audio_callback(void *userdata, Uint8 *stream, int len);
int audio_decode_frame(AVCodecContext *pcodec_ctx, uint8_t *audio_buf, int buf_size);
SDL_Window *init_sdl_and_create_window(int window_w, int window_h);
int refresh_func(void *arg);

int thread_exit = 0;
AVFrame         wanted_frame;
PacketQueue     audio_queue;
int      quit = 0;

int main(int argc, char *argv[])
{
     AVFormatContext        *pformat_ctx = NULL;
     int                    audio_stream_index = -1;
     int                    video_stream_index = -1;
     AVCodecContext         *pvideo_codec_ctx = NULL;
     AVCodecContext         *paudio_codec_ctx = NULL;
     AVCodec                *pvideo_codec = NULL;
     AVCodec                *paudio_codec = NULL;
     AVPacket               packet ;
     AVPacket               *pPacket = NULL;
     AVFrame                *pframe = NULL;
     AVFrame                *pframeYUV = NULL;
     char                   filename[256] = FILE_NAME;
     struct SwsContext      *img_convert_ctx = NULL;
     uint8_t                *out_buffer = NULL;
     AVPixelFormat			pixelfmt;
 //    uint32_t               pixformat = 0;
     int                    got_frame = 0;



     // SDL
     SDL_Window             *pwindow = NULL;
     SDL_Renderer           *prenderer = NULL;
     SDL_Texture            *ptexture = NULL;
     SDL_Rect               sdl_rect ;
     SDL_Event              event;
     int                    window_h = WINDOW_H;
     int                    window_w = WINDOW_W;
     int                    pixel_w = 0;
     int                    pixel_h = 0;
     SDL_AudioSpec          wanted_spec;
     SDL_AudioSpec          spec;

     //获取文件
     get_file_name(filename, argc, argv);

     //初始化
     av_register_all();
     if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
     {
          fprintf(ERR_STREAM, "Couldn't init SDL:%s\n", SDL_GetError);
          exit(-1);
     }

     //这里pformat_ctx会有所指向
     if (avformat_open_input(&pformat_ctx, filename, NULL, NULL) != 0)
     {
         fprintf(ERR_STREAM, "Couldn't open input File\n");
         exit(-1);
     }

     //检测文件流信息
     if (avformat_find_stream_info(pformat_ctx, NULL) < 0)
     {
          fprintf(ERR_STREAM, "Couldn't find stream info\n");
          exit(-1);
     }

     //显示信息
     av_dump_format(pformat_ctx, 0, filename, false);

     //找视频流和音频流
     //当什么流都没有的时候返回-2，只有一个流返回-1
     if (find_stream_index(pformat_ctx, &video_stream_index, &audio_stream_index) == -2)
     {
          fprintf(ERR_STREAM, "Couldn't find stream index\n");
          exit(-1);
     }

     //获取解码器
     if (video_stream_index != -1)
     {
          pvideo_codec_ctx   = pformat_ctx->streams[video_stream_index]->codec;
     	  pvideo_codec       = avcodec_find_decoder(pvideo_codec_ctx->codec_id);
     //打开解码器
     	  if (avcodec_open2(pvideo_codec_ctx, pvideo_codec, NULL) < 0)
     	  {
          	  fprintf(ERR_STREAM, "Couldn't open video decoder\n");
          //exit(-1);
     	  }
     }
     
     if (audio_stream_index != -1)
     {
     	  paudio_codec_ctx   = pformat_ctx->streams[audio_stream_index]->codec;
     	  paudio_codec       = avcodec_find_decoder(paudio_codec_ctx->codec_id);  
     	  if (avcodec_open2(paudio_codec_ctx, paudio_codec, NULL) < 0)
     	  {
          	  fprintf(ERR_STREAM, "Couldn't open audio decoder\n");
          //exit(-1);
     	  }	
     }

     if (pvideo_codec_ctx == NULL && paudio_codec_ctx == NULL)
     {
          fprintf(ERR_STREAM, "Couldn't find decoder\n");
          exit(-1);
     }

     //视频相关,获取像素，多少像素开多大窗口
     pixel_w    = pvideo_codec_ctx->width;
     pixel_h    = pvideo_codec_ctx->height;
     window_w   = pixel_w;
     window_h   = pixel_h;

     pframe     = av_frame_alloc();
     pframeYUV  = av_frame_alloc();

     out_buffer = (uint8_t *)av_malloc(avpicture_get_size(pixelfmt, pixel_w, pixel_h));
     avpicture_fill((AVPicture *)pframeYUV, out_buffer, pixelfmt, pixel_w, pixel_h);
     img_convert_ctx = sws_getContext(pixel_w, pixel_h,
             pvideo_codec_ctx->pix_fmt, pixel_w, pixel_h,
             pixelfmt, SWS_BICUBIC, NULL, NULL, NULL);

     //开窗口
     pwindow = init_sdl_and_create_window(window_w, window_h);
     prenderer = SDL_CreateRenderer(pwindow, -1, 0);
     ptexture = SDL_CreateTexture(prenderer, SDL_PIXELFORMAT_IYUV,
             SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);

     //音频信息，用来打开音频设备
     wanted_spec.freq       = paudio_codec_ctx->sample_rate;
     wanted_spec.format     = AUDIO_S16SYS;
     wanted_spec.channels   = paudio_codec_ctx->channels;
     wanted_spec.silence    = 0;
     wanted_spec.samples    = 1024; //SDL_AUDIO_BUFFER_SIZE
     wanted_spec.callback   = audio_callback;
     wanted_spec.userdata   = paudio_codec_ctx;

     //打开音频设备
     if (SDL_OpenAudio(&wanted_spec, &spec) < 0)
     {
         fprintf(ERR_STREAM, "Couldn't open Audio:%s\n", SDL_GetError());
         //exit(-1);
     }

     wanted_frame.format            = AV_SAMPLE_FMT_S16;
     wanted_frame.sample_rate       = spec.freq;
     wanted_frame.channel_layout    = av_get_default_channel_layout(spec.channels);
     wanted_frame.channels          = spec.channels;

     packet_queue_init(&audio_queue);
     SDL_PauseAudio(0);

     sdl_rect.x = 0;
     sdl_rect.y = 0;
     pPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
     SDL_CreateThread(refresh_func, NULL, NULL);

     while(1)
     {
          SDL_WaitEvent(&event);
          switch(event.type)
          {
              case MY_DEFINE_REFRESH_EVENT:
                  if (av_read_frame(pformat_ctx, pPacket) >= 0)
                  {
                       if (pPacket->stream_index == video_stream_index)
                       {
                           if (avcodec_decode_video2(pvideo_codec_ctx,
                                       pframe, &got_frame, pPacket))
                           {
                               fprintf(ERR_STREAM, "Decode video error\n");
                               //exit(-1);//continue;
                           }
                           if (got_frame)
                           {
                                sws_scale(img_convert_ctx,
                                        (const uint8_t * const *)pframe->data,
                                        pframe->linesize, 0,
                                        pixel_h, pframeYUV->data,
                                        pframeYUV->linesize);
                                sdl_rect.w = window_w;
                                sdl_rect.h = window_h;

                                SDL_UpdateTexture(ptexture, NULL,
                                        pframeYUV->data[0], pframeYUV->linesize[0]);
                                SDL_RenderClear(prenderer);
                                SDL_RenderCopy(prenderer, ptexture,
                                        NULL, &sdl_rect);
                                SDL_RenderPresent(prenderer);
                           }
                       }
                       else if (pPacket->stream_index == audio_stream_index)
                       {
                           packet_queue_put(&audio_queue, pPacket);
                       }
                       av_free_packet(pPacket);
                  }
                  break;
              case SDL_WINDOWEVENT:
                  SDL_GetWindowSize(pwindow, &window_w, &window_h);
                  break;
              case SDL_QUIT:
                  thread_exit = 1;
                  break;
              case MY_DEFINE_BREAK_EVENT:
                  break;
          }

     }

     sws_freeContext(img_convert_ctx);
     av_frame_free(&pframe);
     av_frame_free(&pframeYUV);
     avcodec_close(pvideo_codec_ctx);
     avcodec_close(paudio_codec_ctx);
     avformat_close_input(&pformat_ctx);

     SDL_Quit();

     return 0;
}
////////////////////////////////////////////////////////
int refresh_func(void *arg)
{
    SDL_Event   event;

    thread_exit = 0;

    while (thread_exit == 0)
    {
        event.type = MY_DEFINE_REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(15);
    }

    thread_exit = 0;
//    event.type = MY_DEFINE_BREAK_EVENT;
//    SDL_PushEvent(&event);

    return 0;
}

///////////////////////////////////////////////////////////
//把这个放到一个函数不知道会不会有错？
//pwindow指向的内存会不会释放之类的？
SDL_Window *init_sdl_and_create_window(int window_w, int window_h)
{
    SDL_Window *pwindow = NULL;

     if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
     {
         fprintf(ERR_STREAM, "Countn't initialize SDL: %s\n", SDL_GetError());
         exit(-1);
     }

     pwindow = SDL_CreateWindow("Isshe Video Player SDL2",
             SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
             window_w, window_h,
             SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);     //这个选项是使窗口可以拉伸
     if (!pwindow)
     {
          fprintf(ERR_STREAM, "SDL: Countn't Create window:%s\n", SDL_GetError());
          exit(-1);
     }

     return pwindow;
}

//////////////////////////////////////////////////////
//注意userdata是前面的AVCodecContext.
//len的值常为2048,表示一次发送多少。
//audio_buf_size：一直为样本缓冲区的大小，wanted_spec.samples.（一般每次解码这么多，文件不同，这个值不同)
//audio_buf_index： 标记发送到哪里了。
//这个函数的工作模式是:
//1. 解码数据放到audio_buf, 大小放audio_buf_size。(audio_buf_size = audio_size;这句设置）
//2. 调用一次callback只能发送len个字节,而每次取回的解码数据可能比len大，一次发不完。
//3. 发不完的时候，会len == 0，不继续循环，退出函数，继续调用callback，进行下一次发送。
//4. 由于上次没发完，这次不取数据，发上次的剩余的，audio_buf_size标记发送到哪里了。
//5. 注意，callback每次一定要发且仅发len个数据，否则不会退出。
//如果没发够，缓冲区又没有了，就再取。发够了，就退出，留给下一个发，以此循环。
//三个变量设置为static就是为了保存上次数据，也可以用全局变量，但是感觉这样更好。
void audio_callback(void *userdata, Uint8 *stream, int len)
{
     AVCodecContext *pcodec_ctx     = (AVCodecContext *)userdata;
     int len1 = 0;
     int audio_size = 0;

     //注意是static
     //为什么要分那么大？
     static uint8_t         audio_buf[(AVCODE_MAX_AUDIO_FRAME_SIZE * 3) / 2];
     static unsigned int    audio_buf_size = 0;
     static unsigned int    audio_buf_index = 0;

     //初始化stream，每次都要。
     SDL_memset(stream, 0, len);

     while(len > 0)
     {
          if (audio_buf_index >= audio_buf_size)
          {
              //数据全部发送，再去获取
              //自定义的一个函数
              audio_size = audio_decode_frame(pcodec_ctx, audio_buf, sizeof(audio_buf));
              if (audio_size < 0)
              {
                  //错误则静音
                  audio_buf_size = 1024;
                  memset(audio_buf, 0, audio_buf_size);
              }
              else
              {
                  audio_buf_size = audio_size;
              }
              audio_buf_index = 0;      //回到缓冲区开头
          }

          len1 = audio_buf_size - audio_buf_index;
//          printf("len1 = %d\n", len1);
          if (len1 > len)       //len1常比len大，但是一个callback只能发len个
          {
               len1 = len;
          }
          
          //新程序用 SDL_MixAudioFormat()代替
          //混合音频， 第一个参数dst， 第二个是src，audio_buf_size每次都在变化
          SDL_MixAudio(stream, (uint8_t*)audio_buf + audio_buf_index, len, SDL_MIX_MAXVOLUME);
          //
          //memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1);
          len -= len1;
          stream += len1;
          audio_buf_index += len1;
     }
     
}

///////////////////////////////////////////////////////

//对于音频来说，一个packet里面，可能含有多帧(frame)数据。
int audio_decode_frame(AVCodecContext *pcodec_ctx,
        uint8_t *audio_buf, int buf_size)
{
     AVPacket   packet;
     AVFrame    *frame;
     int        got_frame;
     int        pkt_size = 0;
//     uint8_t    *pkt_data = NULL;
     int        decode_len;
     int        try_again = 0;
     long long  audio_buf_index = 0;
     long long  data_size = 0;
     SwrContext *swr_ctx = NULL;
     int        convert_len = 0;
     int        convert_all = 0;
     
     if (packet_queue_get(&audio_queue, &packet, 1) < 0)
     {
          fprintf(ERR_STREAM, "Get queue packet error\n");
          return -1;
     }

//     pkt_data = packet.data;
     pkt_size = packet.size;
//     fprintf(ERR_STREAM, "pkt_size = %d\n", pkt_size);

     frame = av_frame_alloc();
     while(pkt_size > 0)
     {
//          memset(frame, 0, sizeof(AVFrame));
          //pcodec_ctx:解码器信息
          //frame:输出，存数据到frame
          //got_frame:输出。0代表有frame取了，不意味发生了错误。
          //packet:输入，取数据解码。
          decode_len = avcodec_decode_audio4(pcodec_ctx,
                  frame, &got_frame, &packet);
          if (decode_len < 0) //解码出错
          {
               //重解, 这里如果一直<0呢？
               fprintf(ERR_STREAM, "Couldn't decode frame\n");
               if (try_again == 0)
               {
                    try_again++;
                    continue;
               }
               try_again = 0;
          }


          if (got_frame)
          {
          
 /*              //用定的音频参数获取样本缓冲区大小
               data_size = av_samples_get_buffer_size(NULL,
                       pcodec_ctx->channels, frame->nb_samples,
                       pcodec_ctx->sample_fmt, 1);

               assert(data_size <= buf_size);
//               memcpy(audio_buf + audio_buf_index, frame->data[0], data_size);
*/
              //chnanels: 通道数量, 仅用于音频
              //channel_layout: 通道布局。
              //多音频通道的流，一个通道布局可以具体描述其配置情况.通道布局这个概念不懂。
              //大概指的是单声道(mono)，立体声道（stereo), 四声道之类的吧？
              //详见源码及：https://xdsnet.gitbooks.io/other-doc-cn-ffmpeg/content/ffmpeg-doc-cn-07.html#%E9%80%9A%E9%81%93%E5%B8%83%E5%B1%80
              
              
              if (frame->channels > 0 && frame->channel_layout == 0)
              {
                   //获取默认布局，默认应该了stereo吧？
                   frame->channel_layout = av_get_default_channel_layout(frame->channels);
              }
              else if (frame->channels == 0 && frame->channel_layout > 0)
              {
                  frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);
              }

                
              if (swr_ctx != NULL)
              {
                   swr_free(&swr_ctx);
                   swr_ctx = NULL;
              }
              
              //设置common parameters
              //2,3,4是output参数，4,5,6是input参数。
              swr_ctx = swr_alloc_set_opts(NULL, wanted_frame.channel_layout,
                      (AVSampleFormat)wanted_frame.format,
                      wanted_frame.sample_rate, frame->channel_layout,
                      (AVSampleFormat)frame->format, frame->sample_rate, 0, NULL);
              //初始化
              if (swr_ctx == NULL || swr_init(swr_ctx) < 0)
              {
                   fprintf(ERR_STREAM, "swr_init error\n");
                   break;
              }
              //av_rescale_rnd(): 用指定的方式队64bit整数进行舍入(rnd:rounding),
              //使如a*b/c之类的操作不会溢出。
              //swr_get_delay(): 返回 第二个参数分之一（下面是：1/frame->sample_rate)
              //AVRouding是一个enum，1的意思是round away from zero.
    /*
              int dst_nb_samples = av_rescale_rnd(
                      swr_get_delay(swr_ctx, frame->sample_rate) + frame->nb_samples,
                      wanted_frame.sample_rate, wanted_frame.format,
                      AVRounding(1));
    */

              //转换音频。把frame中的音频转换后放到audio_buf中。
              //第2，3参数为output， 第4，5为input。
              //可以使用#define AVCODE_MAX_AUDIO_FRAME_SIZE 192000 
              //把dst_nb_samples替换掉, 最大的采样频率是192kHz.
              convert_len = swr_convert(swr_ctx, 
                                &audio_buf + audio_buf_index,
                                AVCODE_MAX_AUDIO_FRAME_SIZE,
                                (const uint8_t **)frame->data, 
                                frame->nb_samples);
                                
              printf("decode len = %d, convert_len = %d\n", decode_len, convert_len);
              //解码了多少，解码到了哪里
    //          pkt_data += decode_len;
              pkt_size -= decode_len;
              //转换后的有效数据存到了哪里，又audio_buf_index标记
              audio_buf_index += convert_len;//data_size;
              //返回所有转换后的有效数据的长度
              convert_all += convert_len;
         }
     }
     return wanted_frame.channels * convert_all * av_get_bytes_per_sample((AVSampleFormat)wanted_frame.format);
//     return audio_buf_index;
}


