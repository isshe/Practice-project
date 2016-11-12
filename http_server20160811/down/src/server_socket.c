/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : server_socket.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/08/11
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include "error_wrap.h"


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/11
* Others     (其他):
\*=======================================================================*/
int server_socket(u_short *port)
{
     int    sockfd = 0;
     struct sockaddr_in     serv;
     int    serv_len = 0;

     sockfd = socket(PF_INET, SOCK_STREAM, 0);
     if (sockfd == -1)
     {
         fatal_error("socket");
     }

     memset(&serv, 0, sizeof(serv));
     serv.sin_family = AF_INET;         //IPv4
     serv.sin_port = htons(*port);
     serv.sin_addr.s_addr = htonl(INADDR_ANY);

     if (bind(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0)
     {
          fatal_error("bind");
     }

     //如果端口为0，则自动分配端口
     if (*port == 0)
     {
         serv_len = sizeof(serv);
         if (getsockname(sockfd, (struct sockaddr *)&serv, &serv_len) == -1)
         {
              fatal_error("getsockname");
         }
         *port = ntohs(serv.sin_port);      //
     }

     if (listen(sockfd, 5) < 0)
     {
          fatal_error("listen");
     }

     return sockfd;
}
