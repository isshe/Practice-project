/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : net_api.h
* Version      : v1.0.0
* Author       : i.sshe
* Github       : github.com/isshe
* Date         : 2016/10/21
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

#ifndef _NET_API_H_
#define _NET_API_H_


#ifdef __cplusplus
extern "C"{
#endif

/*=========================================================================*\
 * #include#                                                               *
\*=========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>


/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/
#define RETURN_FAIL         (-1)
#define RETURN_OK           0

#define SELECT_READ         0x1
#define SELECT_WRITE        0x2
#define SELECT_EXCEPT       0x4

#define SELECT_READ_MASK    0x10000
#define SELECT_WRITE_MASK   0x20000
#define SELECT_EXCEPT_MASK  0x40000

#define DEFAULT_RECV_TIMEOUT    500
#define DEFAULT_SEND_TIMEOUT    1000

/*=========================================================================*\
 * #enum#                                                                  *
\*=========================================================================*/

/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/

/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/



/*=====================================================================\
* Function   (名称): set_sock_attr()
* Description(功能): 进行一些属性设置，例如发送接收超时，发送接收缓冲区大小
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
int set_sock_attr(int fd, int bReuseAddr, int nSndTimeOut,
        int nRcvTimeOut, int nSndBuf, int nRcvBuf);



/*=====================================================================\
* Function   (名称): tcp_block_connect()
* Description(功能): 建立连接
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
int tcp_block_connect(const char *localHost, const char *localServ,
        const char *dstHost, const char *dstServ);



/*=====================================================================\
* Function   (名称): hz_itoa()
* Description(功能): 数字转字符串
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
void hz_itoa(int src_n, char *dst_s);



/*=====================================================================\
* Function   (名称): tcp_listen()
* Description(功能): 创建一个监听套接字
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
int tcp_listen(const char *host, const char *serv, int *addrlen);



/*=====================================================================\
* Function   (名称): hz_select()
* Description(功能): select函数的相关操作
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
int hz_select(int *fd_array, int fd_num, int fd_type, int time_out);


/*=====================================================================\
* Function   (名称): tcp_block_accept()
* Description(功能): 接受客户端连接
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
int tcp_block_accept(int fd, struct sockaddr *sa, int *psa_len);


/*=====================================================================\
* Function   (名称): set_sock_noblock()
* Description(功能): 设置为非阻塞读写
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
int set_sock_noblock(int sockfd);



/*=====================================================================\
* Function   (名称): tcp_noblock_recv()
* Description(功能): 非阻塞接收
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
int tcp_noblock_recv(int sockfd, char *recv_buf, int buf_size, int recv_size, int time_out);


#ifdef __cplusplus
}
#endif

#endif

