/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : player1.1.c
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
#include <libavutil/time.h>
#include <SDL2/SDL.h>

//
#include "wrap_base.h"
#include "packet_queue.h"
#include "player_v1.2.h"

#define NOSYNC_THRESHOLD 	10.0
#define SYNC_THRESHOLD 		0.01

int video_thread(void *arg);
int decode_and_show(PlayerState *ps);
void prepare_all(PlayerState *ps);
//int refresh_func(void *arg);
int audio_decode_frame(PlayerState *ps);
void audio_callback(void *userdata, uint8_t *stream, int len);
unsigned int refresh_func(unsigned int interval, void *opaque);
void schedule_refresh(PlayerState *ps, int delay);
void print_AVCodecContext(AVCodecContext *pCodecCtx, FILE *file_stream);
double synchronize(PlayerState *ps, AVFrame *srcFrame, double pts);

int main(int argc, char *argv[])
{
	SDL_Event       event;
    char            filename[1024] = FILE_NAME;
    PlayerState     *ps = NULL;
    AVPacket        *packet = NULL;

    ps = (PlayerState *)av_malloc(sizeof(PlayerState));
    memset(ps, 0, sizeof(PlayerState));
    
//    ps->filename = FILE_NAME;
    get_file_name(ps->filename, argc, argv);

    //prepare_all
    prepare_all(ps);

    av_dump_format(ps->pformat_ctx, 0, ps->filename, false);

    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    packet_queue_init(&(ps->audio_queue));
    packet_queue_init(&(ps->video_queue));

	printf("audio 时长：%d\n", ps->paudio_stream->duration * av_q2d(ps->paudio_stream->time_base));
	printf("video 时长：%d\n", ps->pvideo_stream->duration * av_q2d(ps->pvideo_stream->time_base));
	
    while(1)
    {
 //   	 printf("while(1)\n");
         if (ps->quit == 1)
         {
             break;
         }

         //如果队列数据过多，就等待以下
         if (ps->audio_queue.nb_packets > MAX_AUDIO_QUEUE_SIZE
		 	||ps->video_queue.nb_packets > MAX_VIDEO_QUEUE_SIZE)
         {
         	  printf("过多数据，延时\n");
              SDL_Delay(100);
              continue;
         }

         if (av_read_frame(ps->pformat_ctx, packet) < 0)
         {
             if ((ps->pformat_ctx->pb->error) == 0)
             {
             	  printf("读完延时\n");
                  SDL_Delay(100); //不是出错，可能是读完了
                  break;//continue;
             }
             else
             {
             	 printf("出错，延时\n");
                 continue; 		//出错了，继续读，这里
             }
         }

         //读取到数据了
         if (packet->stream_index == ps->video_stream_index)
         {	  
//         	  printf("video:packet_queue_put,packet size = %d\n", packet->size);
              packet_queue_put(&ps->video_queue, packet);
         }
         else if (packet->stream_index == ps->audio_stream_index)
         {
 //        	 printf("audio:packet_queue_put,packet size = %d\n", packet->size);
             packet_queue_put(&ps->audio_queue, packet);
         }
         else
         {
         	  printf("free packet!\n");
              av_free_packet(packet);
         }
    }
	getchar();
    return 0;
}



int video_thread(void *arg)
{
    PlayerState *ps = (PlayerState *)arg;

    SDL_Event   event;
    
    //用来刷新显示
//    SDL_CreateThread(refresh_func, "refresh_func", ps);

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
	
	schedule_refresh(ps, 40);
	
    while(1)
    {
    	SDL_WaitEvent(&event); //ps->event;
        if (ps->quit == 1)
        {
            break;
        }
        switch(event.type)
        {
            case REFRESH_EVENT:
                decode_and_show(ps);
                break;
            case SDL_WINDOWEVENT:
                SDL_GetWindowSize(ps->pwindow, &ps->window_w, &ps->window_h);
                break;
            case SDL_QUIT:
                ps->quit = 1;
                break;
            default:
                break;
        }

    }
}

void schedule_refresh(PlayerState *ps, int delay)
{
     SDL_AddTimer(delay, refresh_func, ps);
}

double get_audio_clock(PlayerState *ps)
{
	int hw_buf_size = ps->audio_buf_size - ps->audio_buf_index;
	int bytes_per_sec = ps->paudio_stream->codec->sample_rate 
						* ps->paudio_codec_ctx->channels * 2;
	double pts = ps->audio_clock - ((double)hw_buf_size/bytes_per_sec);
	
	return pts;			
	
}


double synchronize(PlayerState *ps, AVFrame *srcFrame, double pts)
{
	double frame_delay;
	
	if (pts != 0)
	{
		ps->video_clock = pts;
	}
	else 
	{
		pts = ps->video_clock;
	}
	
	frame_delay = av_q2d(ps->pvideo_stream->codec->time_base);
	frame_delay += srcFrame->repeat_pict * (frame_delay * 0.5);
	
	ps->video_clock += frame_delay;
	
	return pts;
}

int decode_and_show(PlayerState *ps)
{
     AVPacket   *packet ;
     AVFrame    *pframe = NULL;//&ps->frame;
     AVFrame    *pframeYUV = &ps->frameYUV;
     int        got_frame = 0;
     int        decode_len = 0;

	 pframe = av_frame_alloc();

     packet = (AVPacket *)av_malloc(sizeof(AVPacket));
     if (packet_queue_get(&ps->video_queue, packet, 1) < 0)
     {
         fprintf(ERR_STREAM, "Get audio packet error\n");
         return -1;
     }
     
     printf("get video packet size = %d\n!!!!!", packet->size);
//     print_AVCodecContext(ps->pvideo_codec_ctx, stdout);
     decode_len = avcodec_decode_video2(ps->pvideo_codec_ctx,
             pframe, &got_frame, packet);
     if (decode_len < 0)
     {
         fprintf(ERR_STREAM, "Decode packet error\n");
         return (-1); //
     }

	 double pts = 0.0;
	 if ((pts = av_frame_get_best_effort_timestamp(pframe)) == AV_NOPTS_VALUE)
	 {
	 	  pts = 0;
	 }
	 
	 pts *= av_q2d(ps->pvideo_stream->time_base);
	 pts = synchronize(ps, pframe, pts);
	 pframe->opaque = &pts;

	 printf("video decode len = %d!!!!!!!\n");
     if (got_frame != 0)
     {
     	  double delay = ps->video_cur_pts - ps->frame_last_pts;
     	  if (delay <= 0 || delay >= 1.0)
     	  {
     	  	  delay = ps->frame_last_delay;
     	  }
     	  ps->frame_last_delay = delay;
     	  ps->frame_last_pts = ps->video_cur_pts;
     	  
     	  double ref_clock = get_audio_clock(ps);
     	  double diff = ps->video_cur_pts - ref_clock;
     	  double threshold = (delay > SYNC_THRESHOLD) ? delay : SYNC_THRESHOLD;
     	  if (fabs(diff) < NOSYNC_THRESHOLD) 	//不同步
     	  {
     	  		if (diff <= -threshold)
     	  		{
     	  			delay = 0;
     	  		}
     	  		else if (diff >= threshold)
     	  		{
     	  			delay *= 2;
     	  		}
     	  }
     	  
     	  ps->frame_timer += delay;
     	  double actual_delay = ps->frame_timer - (av_gettime() / 1000000.0);
     	  printf("actual_delay = %d\n", actual_delay);
     	  if (actual_delay <= 0.010)
     	  {
     	  		actual_delay = 0.010;
     	  }
     	  
     	  printf("!@!!delay = %lf@@@!!!\n",actual_delay * 1000 + 0.5);
//     	  SDL_AddTimer(actual_delay * 1000 + 0.5, refresh_func, ps);
		  schedule_refresh(ps, actual_delay * 1000 + 0.5);
     
          sws_scale(ps->sws_ctx, (const uint8_t * const *)pframe->data,
                  pframe->linesize, 0, ps->pixel_h,
                  pframeYUV->data, pframeYUV->linesize);

          ps->sdl_rect.w = ps->window_w;
          ps->sdl_rect.h = ps->window_h;

          SDL_UpdateTexture(ps->ptexture, NULL,
                  pframeYUV->data[0], pframeYUV->linesize[0]);
          SDL_RenderClear(ps->prenderer);
          SDL_RenderCopy(ps->prenderer, ps->ptexture, NULL, &ps->sdl_rect);
          SDL_RenderPresent(ps->prenderer);
     }
     else 
     {
     	schedule_refresh(ps, 40);
     }

     av_free_packet(packet);
}

//////////////////////////////////////////////////////////////////
unsigned int refresh_func(unsigned int interval, void *opaque)
{
	printf("refresh_func\n");
//	PlayerState *ps = (PlayerState *)arg;

    SDL_Event   event;
    
    event.type = REFRESH_EVENT;
//    event.user.data1 = opaque;
    SDL_PushEvent(&event);

    return 0;
}


//////////////////////////////////////////////////////////////////
void prepare_all(PlayerState *ps)
{
     SDL_AudioSpec      wanted_spec;
     SDL_AudioSpec      spec;

     av_register_all();
     if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
     {
          fprintf(ERR_STREAM, "Couldn't init SDL:%s\n", SDL_GetError());
          exit(-1);
     }

     //这里的pformat_ctx会有所指向
     if (avformat_open_input(&ps->pformat_ctx, ps->filename, NULL, NULL) != 0)
     {
          fprintf(ERR_STREAM, "Couldn't open input file!\n");
          exit(-1);
     }

     if (avformat_find_stream_info(ps->pformat_ctx, NULL) < 0)
     {
         fprintf(ERR_STREAM, "Couldn't find stream info\n");
         exit(-1);
     }

     ps->video_stream_index = -1;
     ps->audio_stream_index = -1;
     if (find_stream_index(ps->pformat_ctx,
                 &ps->video_stream_index,
                 &ps->audio_stream_index) == -2)
     {
          fprintf(ERR_STREAM, "Couldn't find any stream index\n");
          exit(-1);
     }

     //有视频流
     //这里想要中途退出，不知道该如何做
     if (ps->video_stream_index != -1)
     {
          ps->pvideo_stream = ps->pformat_ctx->streams[ps->video_stream_index];
          ps->pvideo_codec_ctx = ps->pvideo_stream->codec;
          ps->pvideo_codec = avcodec_find_decoder(ps->pvideo_codec_ctx->codec_id);
          if (ps->pvideo_codec == NULL)
          {
              fprintf(ERR_STREAM, "Couldn't find video decoder\n");
              ps->video_stream_index = -1;
              //exit(-1);
          }

          //打开解码器，并初始化codec_ctx
          if (avcodec_open2(ps->pvideo_codec_ctx, ps->pvideo_codec, NULL) < 0)
          {
               fprintf(ERR_STREAM, "Couldn't open video decoder\n");
               ps->video_stream_index = -1;
               //exit(-1);
          }

          ps->pixel_w   = ps->pvideo_codec_ctx->width;
          ps->pixel_h   = ps->pvideo_codec_ctx->height;
          ps->window_w  = ps->pixel_w;
          ps->window_h  = ps->pixel_h;

          //这里要再了解
          ps->video_buf = (uint8_t *)av_malloc(
                  avpicture_get_size(ps->pixfmt,
                      ps->pixel_w, ps->pixel_h));
                      
		  printf("!!!!!!!!!avpicture_get_size = %d!!!!!pixfmt = %d\n", 
		  			avpicture_get_size(ps->pixfmt,
		  			ps->pixel_w, ps->pixel_h), ps->pixel_h);
		  			
          avpicture_fill((AVPicture *)&ps->frameYUV,
                  ps->video_buf, ps->pixfmt, //AV_PIX_FMT_YUV420P
                  ps->pixel_w, ps->pixel_h);
                  
          ps->sws_ctx = sws_getContext(ps->pixel_w, ps->pixel_h,
                  ps->pvideo_codec_ctx->pix_fmt, ps->pixel_w, ps->pixel_h,
                  ps->pixfmt,//AV_PIX_FMT_YUV420P
                  SWS_BICUBIC, NULL, NULL, NULL);
          printf("ps->sws_ctx = %p\n", ps->sws_ctx);

          ps->sdl_rect.x = 0;
          ps->sdl_rect.y = 0;

          //新建线程
    	  ps->video_tid = SDL_CreateThread(video_thread, "video_thread", ps);

     }

     //有音频流
     if (ps->audio_stream_index != -1)
     {
          ps->paudio_stream = ps->pformat_ctx->streams[ps->audio_stream_index];
          ps->paudio_codec_ctx = ps->paudio_stream->codec;
          ps->paudio_codec = avcodec_find_decoder(ps->paudio_codec_ctx->codec_id);
          if (ps->paudio_codec == NULL)
          {
              fprintf(ERR_STREAM, "Couldn't find audio decoder\n");
              exit(-1); //
          }

          wanted_spec.freq      = ps->paudio_codec_ctx->sample_rate;
          wanted_spec.format    = AUDIO_S16SYS;
          wanted_spec.channels  = ps->paudio_codec_ctx->channels;
          wanted_spec.silence   = 0;
          wanted_spec.samples   = 1024;     //
          wanted_spec.callback  = audio_callback;
          wanted_spec.userdata  = ps; // ps->paudio_codec_ctx;

          //打开音频设备
          if (SDL_OpenAudio(&wanted_spec, &spec) < 0)
          {
               fprintf(ERR_STREAM, "Couldn't open audio device\n");
               exit(-1);    //
          }

          //
          ps->wanted_frame.format           = AV_SAMPLE_FMT_S16;
          ps->wanted_frame.sample_rate      = spec.freq;
          ps->wanted_frame.channel_layout   = av_get_default_channel_layout(spec.channels);
          ps->wanted_frame.channels         = spec.channels;


		  //初始化AVCondecContext，以及进行一些处理工作。
		  avcodec_open2(ps->paudio_codec_ctx, ps->paudio_codec, NULL);
 //         packet_queue_init(&(ps->audio_queue));

          //
          SDL_PauseAudio(0);
     }

     //保存到ps结构中，要注意数据时候会在函数返回时被释放
}


void audio_callback(void *userdata, uint8_t *stream, int len)
{
    PlayerState *ps = (PlayerState *)userdata;
    int 		send_data_size = 0;
    int 		audio_size = 0;
    
	if (ps->quit == 1)
	{
		exit(-1);
	}
    
    SDL_memset(stream, 0, len);


    while(len > 0)
    {
         if (ps->audio_buf_index >= ps->audio_buf_size)
         {
             //数据已经全部发送，再去取
             audio_size = audio_decode_frame(ps);

             if (audio_size < 0)
             {
                  //错误则静音一下
                  ps->audio_buf_size = 1024;
                  memset(ps->audio_buf, 0, ps->audio_buf_size); //
             }
             else
             {
                 ps->audio_buf_size = audio_size;       //解码这么多
             }
             //回到缓冲区头部
             ps->audio_buf_index = 0;
         }
         send_data_size = ps->audio_buf_size - ps->audio_buf_index;
         if (len < send_data_size)
         {
             send_data_size = len;
         }
         //
         SDL_MixAudio(stream, 
         			(uint8_t *)ps->audio_buf + ps->audio_buf_index,
                 	send_data_size, SDL_MIX_MAXVOLUME);

         len -= send_data_size;
         stream += send_data_size;
         ps->audio_buf_index += send_data_size;
    }
}

//对于音频来说，一个packet里面，可能含有多帧(frame)数据。
int audio_decode_frame(PlayerState *ps)
{
	 uint8_t *audio_buf = ps->audio_buf;
	 int 		buf_size = sizeof (ps->audio_buf);
	 AVCodecContext 	*pcodec_ctx = ps->paudio_codec_ctx;
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
     
     if (packet_queue_get(&ps->audio_queue, &packet, 1) < 0)
     {
          fprintf(ERR_STREAM, "Get queue packet error\n");
          return -1;
     }
	
	 if (packet.pts != AV_NOPTS_VALUE)
	 {
	 	ps->audio_clock = av_q2d(ps->paudio_stream->time_base) * packet.pts;
	 }
	 
     pkt_size = packet.size;

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


/*
		  decode_len = avcodec_send_packet(pcodec_ctx, &packet);
		  if (decode_len < 0 )
		  {
		  	   printf("send packet error\n");
		  }
		  decode_len = avcodec_receive_frame(pcodec_ctx, frame);
		  if (decode_len < 0 )
		  {
		  	   printf("receive frame error\n");
		  }
		  
*/
          if (got_frame)
          {
          
 /*              //用定的音频参数获取样本缓冲区大小
               data_size = av_samples_get_buffer_size(NULL,
                       pcodec_ctx->channels, frame->nb_samples,
                       pcodec_ctx->sample_fmt, 1);

               assert(data_size <= buf_size);
//               memcpy(audio_buf + audio_buf_index, frame->data[0], data_size);
*/			}
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
              swr_ctx = swr_alloc_set_opts(NULL, ps->wanted_frame.channel_layout,
                      (AVSampleFormat)ps->wanted_frame.format,
                      ps->wanted_frame.sample_rate, frame->channel_layout,
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
                                &audio_buf ,
                                MAX_AUDIO_FRAME_SIZE,
                                (const uint8_t **)frame->data, 
                                frame->nb_samples);
                                
 //             printf("decode len = %d, convert_len = %d\n", decode_len, convert_len);
              //解码了多少，解码到了哪里
    //          pkt_data += decode_len;
              pkt_size -= decode_len;
              //转换后的有效数据存到了哪里，又audio_buf_index标记
              audio_buf_index += convert_len;//data_size;
              //返回所有转换后的有效数据的长度
              convert_all += convert_len;
         
     }
     
     data_size = ps->wanted_frame.channels * convert_all 
     		* av_get_bytes_per_sample((AVSampleFormat)ps->wanted_frame.format);
     ps->audio_clock += ((double)data_size) 
     		/ (2 * ps->paudio_stream->codec->channels 
     			* ps->paudio_stream->codec->sample_rate); 
     return (data_size);
//     return audio_buf_index;
}

//////////////////////////////////////////////////////////////////
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

#ifdef __cplusplus
}
#endif




