/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : player_v1.0.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/01
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

//ffmpeg
#define __STDC_CONSTANT_MACROS
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

//SDL
#include <SDL2/SDL.h>

#define FILENAME        "cuc_ieschool.flv"
#define ERR_STREAM      stderr
#define OUT_STREAM      stdout
#define WINDOW_W        640
#define WINDOW_H        320
#define MY_DEFINE_REFRESH_EVENT     (SDL_USEREVENT + 1)
#define MY_DEFINE_BREAK_EVENT       (SDL_USEREVENT + 2)

int find_video_stream_index(AVFormatContext *pFormatCtx);
SDL_Window *init_sdl_and_create_window(int window_w, int window_h);
int refresh_func(void *arg);
AVPixelFormat get_pixFormat(AVCodecContext *pCodecCtx);

void print_AVCodecContext(AVCodecContext *pCodecCtx, FILE *file_stream);

int thread_exit = 0;            //全局变量

int main(int argc, char *argv[])
{
     AVFormatContext    *pFormatCtx = NULL;
     AVCodecContext     *pCodecCtx = NULL;
     AVCodec            *pCodec = NULL;
     AVFrame            *pFrame = NULL;
     AVFrame            *pFrameYUV = NULL;
     AVPacket           *pPacket = NULL;
     struct SwsContext  *img_convert_ctx = NULL;
     int                i = 0;
     int                video_index = 0;
     char               filename[256] = FILENAME;
     FILE               *fp = NULL;
     int                ret = 0;
     uint8_t            *out_buffer = NULL;
     int                got_picture = 0;
     AVPixelFormat      pixFormat;

     //SDL
     SDL_Window         *pwindow = NULL;
     int                window_h = WINDOW_H;
     int                window_w = WINDOW_W;
     int                pixel_w = 0;
     int                pixel_h = 0;
     SDL_Renderer       *pSdlRenderer = NULL;
     Uint32             pixformat = 0;
     SDL_Texture        *pSdlTexture = NULL;
     SDL_Rect           sdl_rect;
     SDL_Event          event;



     if (argc == 2)
     {
          memcpy(filename, argv[1], strlen(argv[1])+1);
     }
     else if (argc > 2)
     {
         printf("Usage: ./*.out video_filename\n");
         return (-1);
     }

     //初始化ffmpeg, 并打开视频文件
     av_register_all();
     avformat_network_init();
     pFormatCtx = avformat_alloc_context();
     if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0)
     {
          fprintf(ERR_STREAM, "Countn't open input stream\n");
          return (-1);
     }

     //获取流信息
     if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
     {
          printf("Countn't find stream info\n");
          return (-1);
     }
     //获取视频流的index
     video_index = find_video_stream_index(pFormatCtx);

     pCodecCtx = pFormatCtx->streams[video_index]->codec;

     //找到一个解码器
     pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
     if (pCodec == NULL)
     {
          fprintf(ERR_STREAM, "Countn't found a decoder\n");
          exit(-1);
     }
     //打开解码器
     if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
     {
          fprintf(ERR_STREAM, "Countn't open decoder\n");
          exit(-1);
     }

     //输出以下视频文件的信息
     fprintf(ERR_STREAM, "--------------------------------file info-----------------------------\n");
     av_dump_format(pFormatCtx, 0, filename, 0);
     fprintf(ERR_STREAM, "----------------------------------------------------------------------\n");

     //获取像素, 下面SDL要用
     pixel_w = pCodecCtx->width;
     pixel_h = pCodecCtx->height;
     window_w = pixel_w;        //
     window_h = pixel_h;        //

     pFrame     = av_frame_alloc();
     pFrameYUV  = av_frame_alloc();

     //这里不懂

     out_buffer = (uint8_t *)av_malloc(avpicture_get_size(pixFormat, pixel_w, pixel_h));
     avpicture_fill((AVPicture *)pFrameYUV, out_buffer, pixFormat, pixel_w, pixel_h);

     img_convert_ctx = sws_getContext(pixel_w, pixel_h,
             pCodecCtx->pix_fmt, pixel_w, pixel_h,
             pixFormat, SWS_BICUBIC, NULL, NULL, NULL);


     //以下是SDL相关操作
     //初始化SDL并创建一个窗口
     pwindow = init_sdl_and_create_window(window_w, window_h);

     //新建一个渲染器
     pSdlRenderer = SDL_CreateRenderer(pwindow, -1, 0);
     // ???
//     pixformat = SDL_PIXELFORMAT_IYUV;      //???

     //新建一个SDL_Texture
     pSdlTexture = SDL_CreateTexture(pSdlRenderer, SDL_PIXELFORMAT_IYUV,
             SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);


     //读文件，并显示
     sdl_rect.x = 0;
     sdl_rect.y = 0;
     pPacket    = (AVPacket *)av_malloc(sizeof(AVPacket));
     SDL_CreateThread(refresh_func, NULL, NULL);
     while(1)
     {
          //事件的处理尝试过放在靠近显示的地方， 但是当拉伸窗口的时候（也就是有其他事件的时候），
          //视频会处理得很快，跳过延时。
          //放里面会显示很稳定，质量更好，但是有以上问题，暂不知如何处理。
          SDL_WaitEvent(&event);
          if (event.type == MY_DEFINE_REFRESH_EVENT)
          {
              if (av_read_frame(pFormatCtx, pPacket) >= 0)
              {
                  if (pPacket->stream_index == video_index)
                  {
                  	   print_AVCodecContext(pCodecCtx, stdout);
                       ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, pPacket);
                       printf("decode len = %d\n", ret);
                       if (ret < 0)
                       {
                            fprintf(ERR_STREAM, "Decode error\n");
                            exit(-1); //or continue;
                       }

                       if (got_picture == 0)
                       {
                            printf("get picture error\n");
                       }
                       else
                       {
                            //调整解码出来的图像
                            sws_scale(img_convert_ctx,
                                    (const uint8_t * const *)pFrame->data,
                                    pFrame->linesize, 0, pixel_h,
                                    pFrameYUV->data, pFrameYUV->linesize);

                            //用于重新设置窗口大小
                           sdl_rect.w = window_w;
                           sdl_rect.h = window_h;

                           SDL_UpdateTexture(pSdlTexture, NULL,
                              pFrameYUV->data[0], pFrameYUV->linesize[0]);
                           SDL_RenderClear(pSdlRenderer);
                           SDL_RenderCopy(pSdlRenderer, pSdlTexture, NULL, &sdl_rect);
                           SDL_RenderPresent(pSdlRenderer);
                      }
                 }
                 av_free_packet(pPacket);           //注意！！！

              }//if (av_read_frame)
              else          //
              {
                    thread_exit = 1;
                    break;
              }

          }
          else if (event.type == SDL_WINDOWEVENT)
          {
                SDL_GetWindowSize(pwindow, &window_w, &window_h);
          }
          else if (event.type == SDL_QUIT)
          {
                thread_exit = 1;
                break;
          }
          else if (event.type == MY_DEFINE_BREAK_EVENT)
          {
                break;
          }

    }
     sws_freeContext(img_convert_ctx);
     av_frame_free(&pFrame);
     av_frame_free(&pFrameYUV);
     avcodec_close(pCodecCtx);
     avformat_close_input(&pFormatCtx);

     //SDL
     SDL_Quit();

     return 0;
}

int find_video_stream_index(AVFormatContext *pFormatCtx)
{
     int index = -1;
     int i = 0;

     for (i = 0; i < pFormatCtx->nb_streams; i++)
     {
          if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
          {
              index = i;
              break;
          }
     }

     if (index == -1)
     {
          fprintf(ERR_STREAM, "Countn't find a video stream\n");
          exit(-1);
     }

     return index;
}

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

AVPixelFormat get_pixFormat(AVCodecContext *pCodecCtx)
{
     AVPixelFormat  pixFormat;

     switch(pCodecCtx->pix_fmt)
     {
         case AV_PIX_FMT_YUVJ420P:
             pixFormat = AV_PIX_FMT_YUV420P;
             break;
         case AV_PIX_FMT_YUVJ422P:
             pixFormat = AV_PIX_FMT_YUV422P;
             break;
         case AV_PIX_FMT_YUVJ444P:
             pixFormat = AV_PIX_FMT_YUV444P;
             break;
         case AV_PIX_FMT_YUVJ440P:
             pixFormat = AV_PIX_FMT_YUV440P;
             break;
         default:
             pixFormat = pCodecCtx->pix_fmt;
             break;
     }

     return pixFormat;
}

void print_AVCodecContext(AVCodecContext *pCodecCtx, FILE *file_stream)
{
    fprintf(file_stream, "-----------------------------AVCodecContext-----------------------------\n");
    fprintf(file_stream,
            "log_level_offset = %d\n"
            "codec_id = %d\n"
            "codec_name = %s\n"
            "codec_type = %d\n"
            "bit_rate = %d\n"
            "bit_rate_tolerance = %d\n"
            "global_quality = %d\n"
            "compression_level = %d\n"
            "flags = %d\n"
            "flags = %d\n"
            "delay = %d\n"
            "width = %d\n"
            "height = %d\n"
            "coded_width = %d\n"
            "coded_height = %d\n"
            "frame_size = %d\n",
            pCodecCtx->log_level_offset, pCodecCtx->codec_id,
            pCodecCtx->codec_name,
            pCodecCtx->codec_type, pCodecCtx->bit_rate,
            pCodecCtx->bit_rate_tolerance, pCodecCtx->global_quality,
            pCodecCtx->compression_level, pCodecCtx->flags,
            pCodecCtx->flags2, pCodecCtx->delay,
            pCodecCtx->width, pCodecCtx->height,
            pCodecCtx->coded_width, pCodecCtx->coded_height,
            pCodecCtx->frame_size);
    fprintf(file_stream, "--------------------------------------------------------------------------\n");
}
