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
	SDL_CreateThread(refresh_fun, "refresh func", ps);
//	SDL_AddTimer(40, video_refresh_timer_cb, ps);
	//video_refresh_timer_cb(0, ps);
	return 0;
}



int refresh_fun(void *arg)
{
	SDL_Event event;
	PlayerState *ps = (PlayerState*) arg;
	
	while(ps->quit == 0)
	{	
		printf("audio_clock = %lf, refresh!!!!!delay = %d\n", ps->audio_clock, ps->delay);
		event.type = ISSHE_REFRESH_EVENT;
		SDL_PushEvent(&event);
/*
		if (ps->delay == 0)
		{
			SDL_Delay(40);
		}
		else 
		{
			printf("delay 不为00000000000000000000000000拉\n");
			SDL_Delay(ps->delay);
		}
*/
		SDL_Delay(ps->delay);
	}
	return 0;
}



unsigned int video_refresh_timer_cb(uint32_t interval, void *userdata)
{
	SDL_Event event;
	
	PlayerState *ps = (PlayerState*) userdata;
	
	event.type = ISSHE_REFRESH_EVENT;
	SDL_PushEvent(&event);
	return 40;
}

void schedule_refresh(PlayerState *ps, int delay)
{
    SDL_AddTimer(delay, video_refresh_timer_cb, ps);
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
//////////////////////////////////////////////////////////////		  
		  pts = get_frame_pts(ps, pframe);
		  pframe->opaque = &pts;
		  printf("video frame pts = %lf\n", pts);
//////////////////////////////////////////////////////////////		  
          while(ps->video_frame_queue.nb_frames >= ps->video_frame_queue.queue_capacity)
          {
 //         	   printf("delay 5\n");
               SDL_Delay(5);      
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
//int show_picture(void *arg)
int show_picture(PlayerState *ps)
{
//	 PlayerState *ps = (PlayerState *)arg;
	 printf("show picture\n");
	 printf("!!!!!!\n");
     int ret = 0;
     AVFrame pframe; //av_frame_alloc();不用这个是因为按内存泄漏
/*	 
	 while (ps->video_frame_queue.nb_frames == 0)
	 {
	 	 SDL_Delay(1);
	 }
*/	 
	 printf("nb_frames = %d\n", ps->video_frame_queue.nb_frames);
     ret = frame_queue_get(&ps->video_frame_queue, &pframe, 1);
     if (ret < 0)
     {
          fprintf(ERR_STREAM, "frame queue get error\n");
          return -1;
     }
//////////////////////////////////////////////////////////////////
	 ps->cur_frame_pts = *(double *)pframe.opaque;
	 ps->delay = get_delay(ps) * 1000 + 0.5;
	 printf("显示里面：ps->delay = %d\n", ps->delay);
//////////////////////////////////////////////////////////////////////
/*     sws_scale(ps->psws_ctx, (uint8_t const *const *)pframe.data,
             	pframe.linesize, 0, ps->pixel_h,
             	ps->out_frame.data, ps->out_frame.linesize); 
  */   
     ps->sdl_rect.w = ps->window_w;
     ps->sdl_rect.h = ps->window_h;
     SDL_UpdateTexture(ps->ptexture, NULL,
             ps->out_frame.data[0], ps->out_frame.linesize[0]);
     SDL_RenderClear(ps->prenderer);
     SDL_RenderCopy(ps->prenderer, ps->ptexture, NULL, &ps->sdl_rect);
     SDL_RenderPresent(ps->prenderer);

     return 0;
}


double get_delay(PlayerState *ps)
{
	double 		ret_delay = 0.0;
//	double 		cur_frame_pts = ps->cur_frame_pts; 	//在解码的时候存储
	double 		frame_delay = 0.0;
	double 		cur_audio_clock = 0.0;
	double 		compare = 0.0;
	double  	threshold = 0.0;
	
	//这里的delay是秒为单位， 化为毫秒：*1000
	frame_delay = ps->cur_frame_pts - ps->pre_frame_pts;
	if (frame_delay <= 0 || frame_delay >= 1.0)
	{
		frame_delay = ps->pre_cur_frame_delay;
	}
	ps->pre_cur_frame_delay = frame_delay;
	ps->pre_frame_pts = ps->cur_frame_pts;
	
	cur_audio_clock = get_audio_clock(ps);
	//compare < 0 说明慢了， > 0说明快了
	compare = ps->cur_frame_pts - cur_audio_clock;

	//设置一个阀值, 是一个正数
	threshold = frame_delay > SYNC_THRESHOLD ? frame_delay : SYNC_THRESHOLD;
	
	ret_delay = frame_delay;
	if (compare <= -threshold) 		//慢
	{
		ret_delay = frame_delay / 2;
	}
	else if (compare >= threshold) 	//快了，就在上一帧延时的基础上加长延时
	{
		ret_delay = frame_delay * 2; 		
	}
	
	printf("获取delay里：cur_frame_pts = %lf, cur_audio_clock = %lf\n"
			"ret_delay = %lf, frame_delay = %lf\n",
			ps->cur_frame_pts, cur_audio_clock, ret_delay, frame_delay);
	printf("获取delay里：frame_delay = %lf, ps->pre_frame_pts = %lf"
			"compare = %lf, threshold = %lf\n",
			frame_delay,ps->pre_frame_pts, compare, threshold);

/*	
	ps->frame_timer += ret_delay/1000; 	//考虑要不要,注意这里是秒单位
	int64_t cur_time = av_gettime()/1000000; 	//av_gettime()返回微秒
	double real_delay = ps->frame_timer - cur_time;
	 if (real_delay <= 0.010)
	 {
	 	  read_delay = 0.010;
	 }
	 ret_delay = actual_delay * 1000 + 0.5;
*/	
	return ret_delay;
}

double get_audio_clock(PlayerState *ps)
{
	double cur_buf_pos = ps->audio_buf_index;
	//每个样本占2bytes。16bit
	long long bytes_per_sec = ps->paudio_stream->codec->sample_rate 
	 			* ps->paudio_codec_ctx->channels * 2;
	 			
	double cur_audio_clock = ps->audio_clock + 
	 				cur_buf_pos / (double)bytes_per_sec;
	 
	printf("cur_buf_pos = %lf, bytes_per_sec = %lld, "
	 		"cur_audio_clock = %lf, audio_clock = %lf\n", 
	 		cur_buf_pos, bytes_per_sec, cur_audio_clock, ps->audio_clock);	
	
	return cur_audio_clock;
}

double get_frame_pts(PlayerState *ps, AVFrame *pframe)
{
	double pts = 0.0; 
	double frame_delay = 0.0;
	
	pts = av_frame_get_best_effort_timestamp(pframe);
	if (pts == AV_NOPTS_VALUE) 		//???
	{
		pts = 0;
	}
	
	pts *= av_q2d(ps->pvideo_stream->time_base);
	
	if (pts != 0)
	{
		ps->video_clock = pts; 		//video_clock貌似没有什么实际用处
	}
	else
	{
		pts = ps->video_clock;
	}
	
	//更新video_clock, 这里不理解
	//这里用的是AVCodecContext的time_base
	//extra_delay = repeat_pict / (2*fps)
	frame_delay = av_q2d(ps->pvideo_stream->codec->time_base);
	frame_delay += pframe->repeat_pict / (frame_delay * 2);
	ps->video_clock += frame_delay;
	
	return pts;
}

