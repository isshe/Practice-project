/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : video.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/07
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include "video.h"

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/07
* Others     (其他):
\*=======================================================================*/
int prepare_video(PlayerState *ps)
{
     ps->pvideo_stream = ps->pformat_ctx->streams[ps->video_stream_index];
     ps->pvideo_codec_ctx = ps->pvideo_stream->codec;

     ps->pvideo_codec = avcodec_find_decoder(ps->pvideo_codec_ctx->codec_id);
    if (ps->pvideo_codec == NULL)
    {
        fprintf(ERR_STREAM, "Couldn't find video decoder\n");
        return (-1);
    }

    //打开解码器
    if (avcodec_open2(ps->pvideo_codec_ctx, ps->pvideo_codec, NULL) < 0)
    {
         fprintf(ERR_STREAM, "Couldn't open video decoder\n");
         return -1;
    }

    return 0;
}




/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/07
* Others     (其他):
\*=======================================================================*/
int play_video(PlayerState *ps)
{
     ps->pixel_w    = ps->pvideo_codec_ctx->width;
     ps->pixel_h    = ps->pvideo_codec_ctx->height;
     ps->window_w   = ps->pixel_w;
     ps->window_h   = ps->pixel_h;

     ps->out_frame          = av_frame_alloc();
     ps->pixfmt  = AV_PIX_FMT_YUV420P;
     ps->out_frame->width   = ps->window_w;
     ps->out_frame->height  = ps->window_h;

     //
     ps->video_buf = (uint8_t *)av_malloc(
              avpicture_get_size(ps->pixfmt,
                  ps->out_frame->width, ps->out_frame->height)
             );

     avpicture_fill((AVPicture *)&ps->out_frame, ps->video_buf,
             (AVPixelFormat)ps->out_frame->format,
             ps->out_frame->width, ps->out_frame->height);

     //sws_scale 会用到
     ps->sws_ctx = sws_getContext(ps->pvideo_codec_ctx->width,
             ps->pvideo_codec_ctx->height, ps->pvideo_codec_ctx->pix_fmt,
             ps->out_frame->width, ps->out_frame->height,
             (AVPixelFormat)ps->out_frame->format,
             SWS_BILINEAR, NULL, NULL, NULL);

     ps->sdl_rect.x = 0;
     ps->sdl_rect.y = 0;

    //创建窗口
    //SDL_WINDOW_RESIZABLE: 使窗口可以拉伸
    ps->pwindow = SDL_CreateWindow("Isshe Video Player!",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            ps->window_w, ps->window_h,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (ps->pwindow == NULL)
    {
         fprintf(ERR_STREAM, "Couldn't Create Window\n");
         exit(-1);        //
    }

    //新建一个渲染器
    ps->prenderer = SDL_CreateRenderer(ps->pwindow, -1, 0);

    ps->ptexture  = SDL_CreateTexture(ps->prenderer,
            SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
            ps->pixel_w, ps->pixel_h);

	//新建线程解码
    ps->video_frame_queue.queue_capacity = 30;
	SDL_CreateThread(decode_video, "decode_video", ps);

    SDL_AddTimer(40, show_picture, ps);
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/07
* Others     (其他):
\*=======================================================================*/
int decode_video(void *arg)
{
     PlayerState *ps = (PlayerState *)arg;
     AVPacket packet ;
     AVFrame *frame = av_frame_alloc();
     double pts = 0.0;
     int ret = 0;

     while(1)
     {
         // ...
          if (ps->quit == 1)
              break;

          ret = packet_queue_get(&ps->video_packet_queue, &packet, 1);
          if (ret < 0)
          {
              fprintf(ERR_STREAM, "Get video packet error\n");
              continue;     //
          }

          ret = avcodec_send_packet(ps->pvideo_codec_ctx, &packet);
          if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
          {
              fprintf(ERR_STREAM, "send video packet error\n");
              continue;    //
          }

          ret = avcodec_receive_frame(ps->pvideo_codec_ctx, frame);
          if (ret < 0 && ret != AVERROR_EOF)
          {
               fprintf(ERR_STREAM, "receive video frame error\n");
               continue;
          }

          //同步相关

          if (ps->video_frame_queue.nb_frames >= ps->video_frame_queue.queue_capacity)
          {
               SDL_Delay(100);      //
          }

          frame_queue_put(&ps->video_frame_queue, frame);
     }

     av_frame_free(&frame);

     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/07
* Others     (其他): 取出frame来显示
\*=======================================================================*/

uint32_t show_picture(void *userdata)
{
     PlayerState *ps = (PlayerState *)userdata;
     int ret = 0;
     AVFrame *pframe = av_frame_alloc();

     ret = frame_queue_get(&ps->video_frame_queue, 1);
     if (ret < 0)
     {
          fprintf(ERR_STREAM, "frame queue get error\n");
          return -1;
     }

     sws_scale(ps->sws_ctx, (uint8_t const *const *)pframe->data,
             pframe->linesize, 0, ps->pvideo_codec_ctx->height,
             ps->out_frame->data, ps->out_frame->linesize);

     //显示
     ps->sdl_rect.w = ps->window_w;
     ps->sdl_rect.h = ps->window_h;
     SDL_UpdateTexture(ps->ptexture, NULL,
             ps->out_frame->data[0], ps->out_frame->linesize[0]);
     SDL_RenderClear(ps->prenderer);
     SDL_RenderCopy(ps->prenderer, ps->ptexture, NULL, &ps->sdl_rect);
     SDL_RenderPresent(ps->prederer);

     //frame 还没释放。
//     sws_freeContext(ps->sws_ctx);

     return 1;
}
