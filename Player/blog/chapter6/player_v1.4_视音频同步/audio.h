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

int prepare_audio(PlayerState *ps);

int play_audio(PlayerState *ps);

void audio_callback(void *userdata, uint8_t *stream, int len);

int audio_decode_frame(PlayerState *ps);

#ifdef __cplusplus
}
#endif

#endif

