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
#include "frame_queue.h"
#include "packet_queue.h"
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
int prepare_video(PlayerState *ps);
int play_video(PlayerState *ps);
int decode_video(void *arg);
int show_picture(PlayerState *ps);
unsigned int video_refresh_timer_cb(uint32_t interval, void *userdata);
void schedule_refresh(PlayerState *ps, int delay);
int refresh_fun(void *arg);


#ifdef __cplusplus
}
#endif

#endif

