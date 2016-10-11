/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : seek_about.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/10
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include "seek_about.h"



void do_seek(PlayerState *ps, double increase)
{
	double 	pos = 0.0;

	pos = get_audio_clock(ps); 	//这里以什么为基准同步，就用哪个clock。
	pos += increase;

	if (ps->seek_req == 0)
	{
		ps->seek_req = 1;
		ps->seek_pos = (int64_t)(pos * AV_TIME_BASE); 		//seek_pos用double会不会更好点？
		//AVSEEK_FLAG_BACKWARD，ffmpeg定义为1
		ps->seek_flags = increase > 0 ? 0 : AVSEEK_FLAG_BACKWARD;


	}
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/10
* Others     (其他): 查找相应位置
\*=======================================================================*/
void seeking(PlayerState *ps)
{
	int 	stream_index = -1;
	int64_t seek_target = ps->seek_pos;

	if (ps->video_stream_index >= 0)
	{
		stream_index = ps->video_stream_index;
	}
	else if (ps->audio_stream_index >= 0)
	{
		stream_index = ps->audio_stream_index;
	}

	if (stream_index >= 0)
	{
		//AV_TIME_BASE_Q是AV_TIME_BASE的倒数，用AVRational结构存储
		seek_target = av_rescale_q(seek_target, AV_TIME_BASE_Q,
						ps->pformat_ctx->streams[stream_index]->time_base);
	}

	if (av_seek_frame(ps->pformat_ctx, stream_index,
			 seek_target, ps->seek_flags) < 0)
	{
		fprintf(ERR_STREAM, "error while seeking\n");
	}
	else
	{
		if (ps->video_stream_index >= 0)
		{
			packet_queue_flush(&ps->video_packet_queue);
			//
		}

		if (ps->audio_stream_index >= 0)
		{
			packet_queue_flush(&ps->audio_packet_queue);
		}
	}

	ps->seek_req = 0;
}
