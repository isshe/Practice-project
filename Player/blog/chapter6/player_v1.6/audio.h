/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : audio.h
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

#ifndef AUDIO_H_
#define AUDIO_H_

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

/*目录
 * 1. int prepare_audio(PlayerState *ps);
 * 2. int play_audio(PlayerState *ps);
 * 3. void audio_callback(void *userdata, uint8_t *stream, int len);
 * 4. int audio_decode_frame(PlayerState *ps);
 */

/*=====================================================================\
* Function   (名称): prepare_audio
* Description(功能): 完成音频准备工作
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
int prepare_audio(PlayerState *ps);



/*=====================================================================\
* Function   (名称): play_audio
* Description(功能): 播放
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
int play_audio(PlayerState *ps);



/*=====================================================================\
* Function   (名称): audio_callback()
* Description(功能): 回调函数，打开设备的时候运行
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
void audio_callback(void *userdata, uint8_t *stream, int len);


/*=====================================================================\
* Function   (名称): audio_decode_frame()
* Description(功能): 解码
* Called By  (被调): 1. audio_callback()
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
int audio_decode_frame(PlayerState *ps);

#ifdef __cplusplus
}
#endif
#endif

