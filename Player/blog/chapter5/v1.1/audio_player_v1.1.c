/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : audio_player_v1.1.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/03
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
#include <SDL2/SDL.h>

#ifdef __cplusplus
}
#endif

//
#include "wrap_base.h"
#include "packet_queue.h"

#define AVCODE_MAX_AUDIO_FRAME_SIZE    192000  //1 second of 48khz 32bit audio
#define SDL_AUDIO_BUFFER_SIZE   1024    //

#define FILE_NAME               "/home/isshe/Music/1_qianbenying.flac"
#define ERR_STREAM              stderr
#define OUT_SAMPLE_RATE         44100

/*
static Uint8    *audio_chunk;
static Uint32   audio_len;
static Uint8    *audio_pos;
*/
AVFrame         wanted_frame;
PacketQueue     audio_queue;
int      quit = 0;

void audio_callback(void *userdata, Uint8 *stream, int len);
int audio_decode_frame(AVCodecContext *pcodec_ctx, uint8_t *audio_buf, int buf_size);
/*
void packet_queue_init(PacketQueue *queue);
int packet_queue_put(PacketQueue *queue, AVPacket *packet);
int packet_queue_get(PacketQueue *queue, AVPacket *packet, int block);
*/
int main(int argc, char *argv[])
{
    AVFormatContext     *pformat_ctx = NULL;
    int                 audio_stream = -1;
    AVCodecContext      *pcodec_ctx = NULL;
    AVCodecContext      *pcodec_ctx_cp = NULL;
    AVCodec             *pcodec = NULL;
    AVPacket            packet ;        //!
    AVFrame             *pframe = NULL;
    char                filename[256] = FILE_NAME;

    //SDL
    SDL_AudioSpec       wanted_spec;
    SDL_AudioSpec       spec ;

    //ffmpeg 初始化
    av_register_all();

    //SDL初始化
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        fprintf(ERR_STREAM, "Couldn't init SDL:%s\n", SDL_GetError());
        exit(-1);
    }

    get_file_name(filename, argc, argv);

    //打开文件
    if (avformat_open_input(&pformat_ctx, filename, NULL, NULL) != 0)
    {
         fprintf(ERR_STREAM, "Couldn't open input file\n");
         exit(-1);
    }

    //检测文件流信息
    //旧版的是av_find_stream_info()
    if (avformat_find_stream_info(pformat_ctx, NULL) < 0)
    {
         fprintf(ERR_STREAM, "Not Found Stream Info\n");
         exit(-1);
    }

    //显示文件信息，十分好用的一个函数
    av_dump_format(pformat_ctx, 0, filename, false);


    //找视音频流
    if (find_stream_index(pformat_ctx, NULL, &audio_stream) == -1)
    {
        fprintf(ERR_STREAM, "Couldn't find stream index\n");
        exit(-1);
    }
    printf("audio_stream = %d\n", audio_stream);

    //找到对应的解码器
    pcodec_ctx = pformat_ctx->streams[audio_stream]->codec;
    pcodec = avcodec_find_decoder(pcodec_ctx->codec_id);
    if (!pcodec)
    {
         fprintf(ERR_STREAM, "Couldn't find decoder\n");
         exit(-1);
    }

/*
    pcodec_ctx_cp = avcodec_alloc_context3(pcodec);
    if (avcodec_copy_context(pcodec_ctx_cp, pcodec_ctx) != 0)
    {
        fprintf(ERR_STREAM, "Couldn't copy codec context\n");
        exit(-1);
    }
 */   

    //设置音频信息, 用来打开音频设备。
    wanted_spec.freq        = pcodec_ctx->sample_rate;
    wanted_spec.format      = AUDIO_S16SYS;
    wanted_spec.channels    = pcodec_ctx->channels;        //通道数
    wanted_spec.silence     = 0;    //设置静音值
    wanted_spec.samples     = SDL_AUDIO_BUFFER_SIZE;        //读取第一帧后调整?
    wanted_spec.callback    = audio_callback;
    wanted_spec.userdata    = pcodec_ctx;

    //wanted_spec:想要打开的
    //spce: 实际打开的。
    //这里会开一个线程吗？调用callback。
    //SDL_OpenAudio->open_audio_device(开线程)->SDL_RunAudio->fill(这个实际就是callback函数）
    //可以用SDL_OpenAudioDevice()代替
    if (SDL_OpenAudio(&wanted_spec, &spec) < 0)         
    {
        fprintf(ERR_STREAM, "Couldn't open Audio:%s\n", SDL_GetError());
        exit(-1);
    }

    //设置参数，供解码时候用
    wanted_frame.format         = AV_SAMPLE_FMT_S16;
    wanted_frame.sample_rate    = spec.freq;
    wanted_frame.channel_layout = av_get_default_channel_layout(spec.channels);
    wanted_frame.channels       = spec.channels;

    //初始化AVCondecContext，以及进行一些处理工作。
    avcodec_open2(pcodec_ctx, pcodec, NULL);

    //初始化队列
    packet_queue_init(&audio_queue);
    
    //可以用SDL_PauseAudioDevice()代替,目前用的这个应该是旧的。
    //0是不暂停，非零是暂停
    SDL_PauseAudio(0);              //为什么要这个？

    //读一帧数据
    while(av_read_frame(pformat_ctx, &packet) >= 0)     //读一个packet，数据放在packet.data
    {
         if (packet.stream_index == audio_stream)
         {
             packet_queue_put(&audio_queue, &packet);
         }
         else
         {
             av_free_packet(&packet);
         }
    }
    getchar();      //...
    return 0;
}


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
     
     //初始化stream
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


//对于音频来说，一个packet里面，可能含有多帧(frame)数据。
//这个函数，每次调用最多只解码一帧。
int audio_decode_frame(AVCodecContext *pcodec_ctx, 
        uint8_t *audio_buf, int buf_size)
{
     static AVPacket    packet;
     static uint8_t     *audio_pkt_data = NULL;
     static int         audio_pkt_size = 0;
     static AVFrame     frame;

     int len = 0;
     int data_size = 0;
     int got_frame = 0;
     
     SwrContext     *swr_ctx = NULL;
     fprintf(ERR_STREAM, "audio_pkt_size = %d\n", audio_pkt_size);
     fprintf(ERR_STREAM, "buf_size = %d\n", buf_size);
     while(1)
     {
          while(audio_pkt_size > 0)
          {
              //从packet解码，存到frame中
              got_frame = 0;
              len = avcodec_decode_audio4(pcodec_ctx, &frame, &got_frame, &packet);
              fprintf(ERR_STREAM, "audio_pkt_size = %d!\n", audio_pkt_size);
              fprintf(ERR_STREAM, "len = %d!\n", len);
              if (len < 0)
              {
                   fprintf(ERR_STREAM, "Decode error\n");
                   audio_pkt_size = 0;
                   break;
              }
              audio_pkt_data += len;
              audio_pkt_size -= len;
              data_size = 0;
              if (got_frame)
              {
                   data_size = av_samples_get_buffer_size(NULL,
                           pcodec_ctx->channels, frame.nb_samples,
                           pcodec_ctx->sample_fmt, 1);
                   //
 //                  fprintf(ERR_STREAM, "data_size = %d\n", data_size);
                   assert(data_size <= buf_size);
                   memcpy(audio_buf, frame.data[0], data_size);
              }

              //???
			if (frame.channels > 0 && frame.channel_layout == 0)
			{
				frame.channel_layout = av_get_default_channel_layout(frame.channels);
			}
			else if (frame.channels == 0 && frame.channel_layout > 0)
			{
				frame.channels = av_get_channel_layout_nb_channels(frame.channel_layout);
			}
			if (swr_ctx)
			{
				swr_free(&swr_ctx);
				swr_ctx = NULL;
			}
			
			swr_ctx = swr_alloc_set_opts(NULL, wanted_frame.channel_layout, (AVSampleFormat)wanted_frame.format, wanted_frame.sample_rate,
				frame.channel_layout, (AVSampleFormat)frame.format, frame.sample_rate, 0, NULL);
			if (!swr_ctx || swr_init(swr_ctx) < 0)
			{
//				cout << "swr_init failed:" << endl;
				break;
			}
			int dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, frame.sample_rate) + frame.nb_samples,
				wanted_frame.sample_rate, wanted_frame.format,AVRounding(1));
			int len2 = swr_convert(swr_ctx, &audio_buf, dst_nb_samples,
				(const uint8_t**)frame.data, frame.nb_samples);
				
			return wanted_frame.channels * len2 * av_get_bytes_per_sample((AVSampleFormat)wanted_frame.format);
			
              if (data_size <= 0)
              {
                   //没有数据了，继续获取
                   continue;
              }

              //有数据了
              return data_size;
          }
          
          fprintf(ERR_STREAM, "audio_pkt_size = %d!!!!\n", audio_pkt_size);
/*
          if (packet.data != NULL)
          {
               fprintf(ERR_STREAM, "free packet\n");
               av_free_packet(&packet);
          }
*/

          if (quit == 1)
          {
              return -1;
          }

          //从队列中获取一个packet
          if (packet_queue_get(&audio_queue, &packet, 1) < 0)
          {
               return -1;
          }

          audio_pkt_data = packet.data;
          audio_pkt_size = packet.size;
          fprintf(ERR_STREAM, "audio_pkt_size = %d!!!!！！！\n", audio_pkt_size);
     }//while(1)
}


/*
int audio_decode_frame(AVCodecContext* aCodecCtx, uint8_t* audio_buf, int buf_size)
{
	static AVPacket pkt;
	static uint8_t* audio_pkt_data = NULL;
	static int audio_pkt_size = 0;
	static AVFrame frame;

	int len1;
	int data_size = 0;

	SwrContext* swr_ctx = NULL;

	while (true)
	{
		while (audio_pkt_size > 0)
		{
			int got_frame = 0;
			len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt);
			if (len1 < 0) // 出错，跳过
			{
				audio_pkt_size = 0;
				break;
			}

			audio_pkt_data += len1;
			audio_pkt_size -= len1;
			data_size = 0;
			if (got_frame)
			{
				data_size = av_samples_get_buffer_size(NULL, aCodecCtx->channels, frame.nb_samples, aCodecCtx->sample_fmt, 1);
				printf("data_size = %d\n", data_size);
				assert(data_size <= buf_size);
				memcpy(audio_buf, frame.data[0], data_size);
			}

			if (frame.channels > 0 && frame.channel_layout == 0)
				frame.channel_layout = av_get_default_channel_layout(frame.channels);
			else if (frame.channels == 0 && frame.channel_layout > 0)
				frame.channels = av_get_channel_layout_nb_channels(frame.channel_layout);

			if (swr_ctx)
			{
				swr_free(&swr_ctx);
				swr_ctx = NULL;
			}

			swr_ctx = swr_alloc_set_opts(NULL, wanted_frame.channel_layout, (AVSampleFormat)wanted_frame.format, wanted_frame.sample_rate,
				frame.channel_layout, (AVSampleFormat)frame.format, frame.sample_rate, 0, NULL);

			if (!swr_ctx || swr_init(swr_ctx) < 0)
			{
//				cout << "swr_init failed:" << endl;
				break;
			}

			int dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, frame.sample_rate) + frame.nb_samples,
				wanted_frame.sample_rate, wanted_frame.format,AVRounding(1));
			int len2 = swr_convert(swr_ctx, &audio_buf, dst_nb_samples,
				(const uint8_t**)frame.data, frame.nb_samples);
			if (len2 < 0)
			{
//				cout << "swr_convert failed\n";
				break;
			}

			return wanted_frame.channels * len2 * av_get_bytes_per_sample((AVSampleFormat)wanted_frame.format);

			if (data_size <= 0)
				continue; // No data yet,get more frames

			return data_size; // we have data,return it and come back for more later
		}

		if (pkt.data)
			av_free_packet(&pkt);

		if (quit)
			return -1;

		if (packet_queue_get(&audio_queue, &pkt, true) < 0)
			return -1;

		audio_pkt_data = pkt.data;
		audio_pkt_size = pkt.size;
	}
}
*/
