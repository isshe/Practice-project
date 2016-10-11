/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : video.h
* Version      : v1.0.0
* Author       : i.sshe
* Github       : github.com/isshe
* Date         : 2016/10/07
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

#ifndef VIDEO_H_
#define VIDEO_H_
#ifdef __cplusplus
extern "C"{
#endif

/*=========================================================================*\
 * #include#                                                               *
\*=========================================================================*/
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
#include <SDL2/SDL.h>

#include "player.h"
#include "packet_queue.h"
#include "audio.h"

/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/

/*=========================================================================*\
 * #enum#                                                                  *
\*=========================================================================*/

/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/

/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/
/* 目录
 * 1. int prepare_video(PlayerState *ps);
 * 2. int play_video(PlayerState *ps);
 * 3. int decode_video(void *arg);
 * 4. int refresh_fun(void *arg);
 * 5. double get_audio_clock(PlayerState *ps);
 * 6. double get_frame_pts(PlayerState *ps, AVFrame *pframe);
 * 7. double get_delay(PlayerState *ps);
 */

/*=====================================================================\
* Function   (名称): prepare_video
* Description(功能): 完成一部分视频准备工作
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  : 0
*         failure  : -1
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int prepare_video(PlayerState *ps);


/*=====================================================================\
* Function   (名称): play_video()
* Description(功能): 还算是准备工作
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
int play_video(PlayerState *ps);


/*=====================================================================\
* Function   (名称): decode_and_show()
* Description(功能): 解码函数并显示
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
int decode_and_show(void *arg);


/*=====================================================================\
* Function   (名称): refresh_fun()
* Description(功能): 刷新函数
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
int refresh_fun(void *arg);


/*=====================================================================\
* Function   (名称): get_frame_pts()
* Description(功能): 获取视频帧的显示时间戳pts
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
double get_frame_pts(PlayerState *ps, AVFrame *pframe);


/*=====================================================================\
* Function   (名称): get_delay()
* Description(功能): 获取延时长度
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
double get_delay(PlayerState *ps);

#ifdef __cplusplus
}
#endif
#endif

