/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : frame_queue.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/06
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
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
#include <SDL2/SDL.h>
}
#include "frame_queue.h"


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/03
* Others     (其他): 初始化
\*=======================================================================*/
void frame_queue_init(FrameQueue *queue)
{
//     memset(queue, 0, sizeof(FrameQueue));
     queue->first_fm    	= NULL;
     queue->last_fm     	= NULL;
     queue->nb_frames 		= 0;
     queue->mutex           = SDL_CreateMutex();
     queue->cond            = SDL_CreateCond();
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/03
* Others     (其他): 入队
\*=======================================================================*/
int frame_queue_put(FrameQueue *queue, AVFrame *frame)
{
     AVFrameList   *fm_list;

     fm_list = (AVFrameList *)av_malloc(sizeof(AVFrameList));
     if (fm_list == NULL)
     {
          return -1;
     }
 /* 
     fm_list->frame = (AVFrame *)av_mallocz(sizeof(AVFrame));
     memcpy(fm_list->frame, frame, sizeof(AVFrame));
*/
	 fm_list->frame = *frame;
     fm_list->next  = NULL;

     //上锁
     SDL_LockMutex(queue->mutex);

     if (queue->last_fm == NULL)    //空队
     {
          queue->first_fm = fm_list;
     }
     else
     {
         queue->last_fm->next = fm_list;
     }

     queue->last_fm = fm_list;  //这里queue->last_fm = queue->last_fm->next 的意思，但是，处理了更多的情况。
     queue->nb_frames++;
//     queue->size += frame->size;
     SDL_CondSignal(queue->cond);      //???

     SDL_UnlockMutex(queue->mutex);

     return 0;
}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/03
* Others     (其他): 出队
\*=======================================================================*/

int frame_queue_get(FrameQueue *queue, AVFrame *frame, int block)
{
     AVFrameList   *fm_list = NULL;
     int            ret = 0;

     SDL_LockMutex(queue->mutex);
	 
     while(1)
     {
          fm_list = queue->first_fm;
          if (fm_list != NULL)         //队不空，还有数据
          {
              queue->first_fm = queue->first_fm->next;    //fm_list->next
              if (queue->first_fm == NULL)
              {
                   queue->last_fm = NULL;
              }
              
              queue->nb_frames--;

              *frame = fm_list->frame;          // 复制给packet。

              av_free(fm_list);
              ret = 1;
              break;
          }
          else if (block == 0)
          {
               ret = 0;
               break;
          }
          else
          {
              SDL_CondWait(queue->cond, queue->mutex);
          }
     }

     SDL_UnlockMutex(queue->mutex);

     return ret;
}
