/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : wrap_base.c
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



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/03
* Others     (其他): 成功返回
\*=======================================================================*/
int find_stream_index(AVFormatContext *pformat_ctx, int *video_stream, int audio_stream)
{
     int i = 0;
     int audio_index = -1;
     int video_index = -1;

     for (i = 0; i < pformat_ctx->nb_streams; i++)
     {
          if (pformat_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
          {
              video_index = i;
          }
          if (pformat_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
          {
              audio_index = i;
          }
     }

     *video_stream = video_index;
     *audio_stream = audio_index;

     if (video_index == -1 && audio_index == -1)
     {
         return -3
     }
     else if (video_index == -1)
     {
          return -2;
     }
     else if (audio_index == -1)
     {
          return -1;
     }

     return 0;
}
