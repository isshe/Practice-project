/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : player_v1.3.h
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

#ifndef PLAYER_H_
#define PLAYER_H_
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
#include "packet_queue.h"
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
#define ISSHE_REFRESH_EVENT 	(SDL_USEREVENT + 1)
#define BREAK_EVENT   			(SDL_USEREVENT + 2)
#define VIDEO_QUIT_EVENT 		(SDL_USEREVENT + 3)
#define AUDIO_QUIT_EVENT		(SDL_USEREVENT + 4)  				
#define MAX_AUDIO_QUEUE_SIZE 	128
#define MAX_VIDEO_QUEUE_SIZE 	64
#define READ_FRAME_ERROR_TIMES 	10 		//连续没读到packet10次，认为没有了


/*=========================================================================*\
 * #enum#                                                                  *
\*=========================================================================*/

/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/

//这是一个统筹的结构
typedef struct PlayerState
{
     //公共
     AVFormatContext    *pformat_ctx;
     char               filename[1024];
     int                quit;
     int 				player_state;
/*
     SDL_Thread         *audio_decode_tid;
     SDL_Thread         *audio_tid;
     SDL_Thread         *video_decode_tid;
     SDL_Thread         *video_tid;
*/
     //音频
     int                audio_stream_index;
     AVStream           *paudio_stream;
     AVCodecContext     *paudio_codec_ctx;
     AVCodec            *paudio_codec;
     PacketQueue        audio_packet_queue;
     uint8_t            audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
     unsigned int       audio_buf_size;
     unsigned int       audio_buf_index;
     int 				audio_quit;

     //视频
     int                video_stream_index;
     AVStream           *pvideo_stream;
     AVCodecContext     *pvideo_codec_ctx;
     AVCodec            *pvideo_codec;
     PacketQueue        video_packet_queue;
     uint8_t            *video_buf;
     unsigned int       video_buf_size;
     unsigned int       video_buf_index;
     struct SwsContext  *psws_ctx;
     int 				video_quit; 	
     int 				zero_packet_count; //计算获取不到packet的次数，退出的依据

     int                pixel_w;
     int                pixel_h;
     int                window_w;
     int                window_h;

     SDL_Window         *pwindow;
     SDL_Renderer       *prenderer;
     SDL_Texture        *ptexture;
     SDL_Rect           sdl_rect;
     AVPixelFormat      pixfmt;
//     AVFrame            frame;
     AVFrame            out_frame;

     //同步相关
     double             audio_clock;
     double             video_clock;
     double 			pre_frame_pts; 			//前一帧显示时间
     double 			cur_frame_pts; 			//packet.pts
     double 			pre_cur_frame_delay; 	//当前帧和前一帧的延时，前面两个相减的结果
     uint32_t			delay;
     double             frame_timer;
}PlayerState;

/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/


/*=====================================================================\
* Function   (名称): player_state_init()
* Description(功能): 初始化
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
void player_state_init(PlayerState *ps);



/*=====================================================================\
* Function   (名称): decode_thread
* Description(功能): 解码
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
int decode_thread(void *arg);
/*=====================================================================\
* Function   (名称): prepare_common()
* Description(功能): 准备
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
int prepare_common(PlayerState *ps);

//int deal_with_event(PlayerState *ps, SDL_Event event);

#ifdef __cplusplus
}
#endif


#endif

