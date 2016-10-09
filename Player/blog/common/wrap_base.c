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
#define __STDC_CONSTANT_MACROS      //ffmpeg要求

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>

#include <assert.h>
#include "wrap_base.h"
#ifdef __cplusplus
}
#endif


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/03
* Others     (其他): 成功返回
\*=======================================================================*/
int find_stream_index(AVFormatContext *pformat_ctx, int *video_stream, int *audio_stream)
{
    assert(video_stream != NULL || audio_stream != NULL);
	
	if (video_stream == NULL && audio_stream == NULL)
	{
		return -2;
	}
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

     //注意以下两个判断有可能返回-1.
     if (video_stream == NULL)
     {
         *audio_stream = audio_index;
         return *audio_stream;
     }

     if (audio_stream == NULL)
     {
         *video_stream = video_index;
          return *video_stream;
     }

     *video_stream = video_index;
     *audio_stream = audio_index;

     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/03
* Others     (其他):获取文件名
\*=======================================================================*/
void get_file_name(char *filename, int argc, char *argv[])
{
    if (argc == 2)
    {
        memcpy(filename, argv[1], strlen(argv[1])+1);
    }
    else if (argc > 2)
    {
         fprintf(stderr, "Usage: ./*.out filename\n");
         exit(-1);
    }
}
