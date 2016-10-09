/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : player1.1.h
* Version      : v1.0.0
* Author       : i.sshe
* Github       : github.com/isshe
* Date         : 2016/10/05
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

#ifndef PLAYERV1_1_H
#define PLAYERV1_1_H

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
/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/
#define MAX_AUDIO_FRAME_SIZE    192000  //1 second of 48khz 32bit audio
#define SDL_AUDIO_BUFFER_SIZE   1024    //

#define FILE_NAME               "/home/isshe/Music/WavinFlag.aac"
#define ERR_STREAM              stderr
#define OUT_SAMPLE_RATE         44100
#define OUT_STREAM              stdout
#define WINDOW_W                640
#define WINDOW_H                320
#define REFRESH_EVENT 			(SDL_USEREVENT + 1)
#define BREAK_EVENT   			(SDL_USEREVENT + 2)
#define MAX_AUDIO_QUEUE_SIZE 	64
#define MAX_VIDEO_QUEUE_SIZE 	25
/*=========================================================================*\
 * #enum#                                                                  *
\*=========================================================================*/

/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/
typedef struct PlayerState
{
    //公共
    AVFormatContext     *pformat_ctx;
    char                filename[1024];
    int                 quit;

    //音频
    int                 audio_stream_index;
    AVStream            *paudio_stream;
    AVCodecContext      *paudio_codec_ctx;
    AVCodec 			*paudio_codec;
    PacketQueue         audio_queue;
    uint8_t             audio_buf[(MAX_AUDIO_FRAME_SIZE * 3)/2];
    unsigned int        audio_buf_size;
    unsigned int        audio_buf_index;
    double 				audio_clock;

    //视频
    int                 video_stream_index;
    AVStream            *pvideo_stream;
    AVCodecContext      *pvideo_codec_ctx;
    AVCodec 			*pvideo_codec;
    PacketQueue         video_queue;
    uint8_t             *video_buf;
    unsigned int        video_buf_size;
    unsigned int        video_buf_index;
    int                 pixel_w ;
    int                 pixel_h ;
    int                 window_w;
    int                 window_h;
    struct SwsContext   *sws_ctx;
    SDL_Window          *pwindow;
    SDL_Renderer        *prenderer;
    SDL_Texture         *ptexture;
    SDL_Rect            sdl_rect;
    AVFrame             frame;         //
    AVFrame             frameYUV;      //
    AVPixelFormat       pixfmt;
    
    double 				video_clock;
    double 				video_cur_pts;
    double 				cur_frame_pts;
    double 				frame_last_pts;
    double 				frame_last_delay;
    double 				frame_timer;

    SDL_Thread 			*video_tid;

    AVFrame             wanted_frame;
    AVPacket            packet;
    uint8_t             *pkt_data;
    int                 pkt_size;

}PlayerState;
/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/

#ifdef __cplusplus
}
#endif

#endif

