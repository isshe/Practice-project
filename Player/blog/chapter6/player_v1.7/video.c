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
* Others     (其他): 一些准备工作
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
* Others     (其他): 其实这里还算准备工作，开窗口什么的
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
     //现在因为只用了一次sws_getContext()所以，这个内存在main释放。
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

	//新建线程运行刷新函数
	SDL_CreateThread(refresh_fun, "refresh func", ps);

	return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/08
* Others     (其他): 刷新函数
\*=======================================================================*/
int refresh_fun(void *arg)
{
	SDL_Event event;
	PlayerState *ps = (PlayerState*) arg;

	while(ps->player_state != -1 && ps->video_quit != -1)
	{
		switch(ps->player_state)
		{
            case 0:     //播放
		        event.type = ISSHE_REFRESH_EVENT;
		        SDL_PushEvent(&event);
		        SDL_Delay(ps->delay);
                break;
            case 1:     //暂停
            	while(ps->player_state == 1)
            	{
            		SDL_Delay(40);
            	}
                break;
            default:
                break;
		}
    }
    
    printf("退出视频刷新函数!\n");
	return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/07
* Others     (其他):
\*=======================================================================*/
int decode_and_show(void *arg)
{
     PlayerState *ps = (PlayerState *)arg;
     AVPacket packet ;
     AVFrame *pframe = av_frame_alloc();
     AVFrame *tempframe = av_frame_alloc();
     double pts = 0.0;
     int ret = 0;

	  //这部分处理退出
      if (ps->video_packet_queue.nb_packets == 0)
      {
      	  if (ps->zero_packet_count >= 10)
      	  {
      	  	  ps->video_quit = -1;
      	  }
      	  ps->zero_packet_count++;
          return 0;
      }

	  ps->zero_packet_count = 0;

      //从packet队列取一个packet出来解码
      ret = packet_queue_get(&ps->video_packet_queue, &packet, 1);
      if (ret < 0)
      {
          fprintf(ERR_STREAM, "Get video packet error\n");
          return -1;     //
      }

      ret = avcodec_send_packet(ps->pvideo_codec_ctx, &packet);
      if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
      {
          fprintf(ERR_STREAM, "send video packet error\n");
          return -1;     //
      }

      ret = avcodec_receive_frame(ps->pvideo_codec_ctx, pframe);
      if (ret < 0 && ret != AVERROR_EOF)
      {
           fprintf(ERR_STREAM, "receive video frame error\n");
           return -1;
      }

     //下面三句实现音视频同步，还有一句在audio部分。
     //获取pts
     pts = get_frame_pts(ps, pframe);

     //ps中用cur_frame_pts是为了减少get_delay()的参数
     ps->cur_frame_pts = pts; //*(double *)pframe.opaque;
     ps->delay = get_delay(ps) * 1000 + 0.5;

     printf("video frame pts = %lf\n", pts);
     printf("显示里面：ps->delay = %d\n", ps->delay);

     //这个函数看了手册也不大理解
     sws_scale(ps->psws_ctx, (uint8_t const *const *)pframe->data,
            pframe->linesize, 0, ps->pixel_h,
            ps->out_frame.data, ps->out_frame.linesize);

     ps->sdl_rect.w = ps->window_w;
     ps->sdl_rect.h = ps->window_h;

     SDL_UpdateTexture(ps->ptexture, NULL,
             ps->out_frame.data[0], ps->out_frame.linesize[0]);
     SDL_RenderClear(ps->prenderer);
     SDL_RenderCopy(ps->prenderer, ps->ptexture, NULL, &ps->sdl_rect);
     SDL_RenderPresent(ps->prenderer);


     av_frame_free(&pframe);

     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/08
* Others     (其他): 获取延迟时间
\*=======================================================================*/
double get_delay(PlayerState *ps)
{
	double 		ret_delay = 0.0;
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
	//两帧之间的延时
	ps->pre_cur_frame_delay = frame_delay;
	ps->pre_frame_pts = ps->cur_frame_pts;

	cur_audio_clock = get_audio_clock(ps);

	//compare < 0 说明慢了， > 0说明快了
	compare = ps->cur_frame_pts - cur_audio_clock;

	//设置一个阀值, 是一个正数
	//这里设阀值为两帧之间的延迟，
	threshold = frame_delay;
	//SYNC_THRESHOLD ? frame_delay : SYNC_THRESHOLD;


	if (compare <= -threshold) 		//慢， 加快速度
	{
		ret_delay = frame_delay / 2;
	}
	else if (compare >= threshold) 	//快了，就在上一帧延时的基础上加长延时
	{
		ret_delay = frame_delay * 2;
	}
	else
	{
		//
		ret_delay = frame_delay;//frame_delay;
	}
/*
	printf("获取delay里：cur_frame_pts = %lf, cur_audio_clock = %lf\n"
			"ret_delay = %lf, frame_delay = %lf\n",
			ps->cur_frame_pts, cur_audio_clock, ret_delay, frame_delay);
	printf("获取delay里：frame_delay = %lf, ps->pre_frame_pts = %lf"
			"compare = %lf, threshold = %lf\n",
			frame_delay,ps->pre_frame_pts, compare, threshold);
*/

/*	//这部分可能用了更准确之类的，但是这次没用，也没测试过
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


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/08
* Others     (其他): 获取一个帧的pts
\*=======================================================================*/
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
	//extra_delay = repeat_pict / (2*fps), 这个公式是在ffmpeg官网手册看的
	frame_delay = av_q2d(ps->pvideo_stream->codec->time_base);
	frame_delay += pframe->repeat_pict / (frame_delay * 2);
	ps->video_clock += frame_delay;

	return pts;
}

