/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : packet_queue.h
* Version      : v1.0.0
* Author       : i.sshe
* Github       : github.com/isshe
* Date         : 2016/10/03
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

#ifndef PACKET_QUEUE_H_
#define PACKET_QUEUE_H_

#ifdef __cplusplus
extern "C"{
#endif

/*=========================================================================*\
 * #include#                                                               *
\*=========================================================================*/
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>
/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/

/*=========================================================================*\
 * #enum#                                                                  *
\*=========================================================================*/

/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/
typedef struct PacketQueue
{
    AVPacketList    *first_pkt;     //队头的一个packet, 注意类型不是AVPacket
    AVPacketList    *last_pkt;      //队尾packet
    int             nb_packets;     // paket个数
    int             size;           //
    SDL_mutex       *mutex;         //
    SDL_cond        *cond;          // 条件变量
}PacketQueue;

/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/


/*=====================================================================\
* Function   (名称): pakcet_queue_init()
* Description(功能): 初始化pakcet队列
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
void packet_queue_init(PacketQueue *queue);

/*=====================================================================\
* Function   (名称): packet_queue_put()
* Description(功能): 把packet入队。
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
int packet_queue_put(PacketQueue *queue, AVPacket *packet);


/*=====================================================================\
* Function   (名称): pakcet_queue_get()
* Description(功能): 从队列获取一个pakcet,
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  : 返回0
*         failure  : 返回-1
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
int packet_queue_get(PacketQueue *queue, AVPacket *pakcet, int block);


/*=====================================================================\
* Function   (名称): packet_queue_flush()
* Description(功能): 清空队列
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
void packet_queue_flush(PacketQueue *queue);


#ifdef __cplusplus
}
#endif

#endif

