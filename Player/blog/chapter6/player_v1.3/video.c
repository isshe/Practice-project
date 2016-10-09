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
	 printf("play video!!!\n");
     ps->pixel_w    = ps->pvideo_codec_ctx->width;
     ps->pixel_h    = ps->pvideo_codec_ctx->height;
     ps->window_w   = ps->pixel_w;
     ps->window_h   = ps->pixel_h;

     ps->pixfmt  = AV_PIX_FMT_YUV420P;
     ps->out_frame.format  = AV_PIX_FMT_YUV420P;
     ps->out_frame.width   = ps->pixel_w;
     ps->out_frame.height  = ps->pixel_h;
     
     //
     ps->video_buf = (uint8_t *)av_malloc(
              avpicture_get_size(ps->pixfmt,
                  ps->out_frame.width, ps->out_frame.height)
             );
             
     //用av_image_fill_arrays代替。
     //根据所给参数和提供的数据设置data指针和linesizes。
     avpicture_fill((AVPicture *)&ps->out_frame, ps->video_buf,
             ps->pixfmt,
             ps->out_frame.width, ps->out_frame.height);

     //使用sws_scale之前要用这个函数进行相关转换操作。
     //分配和返回一个 SwsContext.
     //sws_freeContext(ps->psws_ctx); 需要用这个函数free内存。
     //现在因为只用了一次sws_getContext()所以，这个内存最后释放。
     //因为一直输出格式什么都一样，所以没有放在靠近sws_scale的地方。
     ps->psws_ctx = sws_getContext(ps->pixel_w,
             ps->pixel_h, ps->pvideo_codec_ctx->pix_fmt,
             ps->out_frame.width, ps->out_frame.height,
             ps->pixfmt,
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
    ps->video_frame_queue.queue_capacity = VIDEO_FRAME_QUEUE_CAPACITY;
	SDL_CreateThread(decode_video, "decode_video", ps);

//	schedule_refresh(ps, 40);
//	SDL_CreateThread(refresh_fun, "refresh func", ps);
	SDL_AddTimer(40, video_refresh_timer_cb, ps);
	//video_refresh_timer_cb(0, ps);
	
	return 0;
}



int refresh_fun(void *arg)
{
	SDL_Event event;

	PlayerState *ps = (PlayerState*) arg;
	while(ps->quit == 0)
	{
		printf("refresh!!!!!\n");
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(10);
	}
	

	return 0;
}

void schedule_refresh(PlayerState *ps, int delay)
{
    SDL_AddTimer(delay, video_refresh_timer_cb, ps);
}

unsigned int video_refresh_timer_cb(uint32_t interval, void *userdata)
{
	SDL_Event event;
	
	PlayerState *ps = (PlayerState*) userdata;
	
	event.type = REFRESH_EVENT;
	SDL_PushEvent(&event);
	return 40;
}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/07
* Others     (其他):
\*=======================================================================*/
int decode_video(void *arg)
{
	 printf("decode_video!!!\n");
     PlayerState *ps = (PlayerState *)arg;
     AVPacket packet ;
     AVFrame *pframe = av_frame_alloc();
     AVFrame *tempframe = av_frame_alloc();
     double pts = 0.0;
     int ret = 0;

     while(1)
     {
         // ...
          if (ps->quit == 1)
              break;

		  if (ps->video_packet_queue.nb_packets == 0)
		  {
		  	  continue;
		  }

		  //从packet队列取一个packet出来解码
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

          ret = avcodec_receive_frame(ps->pvideo_codec_ctx, pframe);
          if (ret < 0 && ret != AVERROR_EOF)
          {
               fprintf(ERR_STREAM, "receive video frame error\n");
               continue;
          }


          
          while(ps->video_frame_queue.nb_frames >= ps->video_frame_queue.queue_capacity)
          {
          	   printf("delay 1\n");
 //              SDL_Delay(1);      
          }
          
          //修理过后再放到队列里
          //如果先入队，再使用这个函数处理，显示会不对，不明白为什么
     	  sws_scale(ps->psws_ctx, (uint8_t const *const *)pframe->data,
             	pframe->linesize, 0, ps->pixel_h,
             	ps->out_frame.data, ps->out_frame.linesize);   
                    	       
         //解码完的frame，存到frame队列中
          frame_queue_put(&ps->video_frame_queue, pframe);  			//因为把ps->out_frame写在这里，找了一天错误。
     	  printf("put frame, nb_frames = %d video pacekt = %d\n", ps->video_frame_queue.nb_frames, ps->video_packet_queue.nb_packets);
     }

     av_frame_free(&pframe);

     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/07
* Others     (其他): 取出frame来显示
\*=======================================================================*/
extern FILE *fp;

int show_picture(PlayerState *ps)
{
	 printf("show picture\n");
     int ret = 0;
     AVFrame pframe; //av_frame_alloc();不用这个是因为按内存泄漏
	 
	 while (ps->video_frame_queue.nb_frames == 0)
	 {
	 	 SDL_Delay(1);
	 }
	 
     ret = frame_queue_get(&ps->video_frame_queue, &pframe, 1);

     if (ret < 0)
     {
          fprintf(ERR_STREAM, "frame queue get error\n");
          return -1;
     }
	
     ps->sdl_rect.w = ps->window_w;
     ps->sdl_rect.h = ps->window_h;
     SDL_UpdateTexture(ps->ptexture, NULL,
             ps->out_frame.data[0], ps->out_frame.linesize[0]);
     SDL_RenderClear(ps->prenderer);
     SDL_RenderCopy(ps->prenderer, ps->ptexture, NULL, &ps->sdl_rect);
     SDL_RenderPresent(ps->prenderer);
     
     //frame 还没释放。
//	 av_frame_free(pframe);
//     SDL_AddTimer(40, video_refresh_timer_cb, ps);
//     av_frame_unref(pframe);

     return 0;
}
