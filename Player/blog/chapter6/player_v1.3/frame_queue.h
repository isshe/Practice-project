/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : frame_queue.h
* Version      : v1.0.0
* Author       : i.sshe
* Github       : github.com/isshe
* Date         : 2016/10/06
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

#ifndef FRAME_QUEUE_H
#define FRAME_QUEUE_H

#ifdef __cplusplus
extern "C"{
#endif

/*=========================================================================*\
 * #include#                                                               *
\*=========================================================================*/
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
/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/

/*=========================================================================*\
 * #enum#                                                                  *
\*=========================================================================*/

/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/
typedef struct AVFrameList
{
    AVFrame             frame;
    struct AVFrameList  *next;
}AVFrameList;

typedef struct FrameQueue
{
     AVFrameList    *first_fm;
     AVFrameList    *last_fm;
     int            nb_frames;
     int 			queue_capacity; 		//要设置
//     int            size;
     SDL_mutex      *mutex;
     SDL_cond       *cond;

     struct FrameQueue *next;
}FrameQueue;

/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/


/*=====================================================================\
* Function   (名称): frame_queue_init()
* Description(功能): 初始化队列
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
void frame_queue_init(FrameQueue *queue);


/*=====================================================================\
* Function   (名称): frame_queue_put()
* Description(功能): 入队
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int frame_queue_put(FrameQueue *queue, AVFrame *frame);


/*=====================================================================\
* Function   (名称): frame_queue_get()
* Description(功能): 出队
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int frame_queue_get(FrameQueue *queue, AVFrame *frame, int block);

#ifdef __cplusplus
}
#endif

#endif

