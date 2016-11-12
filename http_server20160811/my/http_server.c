/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : http_server.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/08/15
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include "common_inc.h"
#include "http_server.h"
#include "wrap_sock.h"
#include "wrap_error.h"
#include "deal_request.h"


int main(void)
{
    int     serverfd = 0;
    int     clientfd = 0;
    SA_IN   servaddr;
    SA_IN   cliaddr;
    int    servport = 0;
    int     len = sizeof(SA_IN);
    pthread_t   newthreadID ;

    printf("输入大于1024的端口号，输入0自动分配: ");             //
    scanf("%d", &servport);

    serverfd = Socket(AF_INET, SOCK_STREAM, 0);       //PF_INET

    memset(&servaddr, 0, sizeof(SA_IN));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(servport);

    Bind(serverfd, (SA *)&servaddr, sizeof(servaddr));

    //获取自动分配的端口
    if (servport == 0)
    {
         Getsockname(serverfd, (SA *)&servaddr, &len);
         servport = ntohs(servaddr.sin_port);
    }

    Listen(serverfd, 100);
    printf("http server running on port %d\n", servport);


    while(1)
    {
         int clientlen = sizeof(SA_IN);
         clientfd = Accept(serverfd, (SA*)&cliaddr, &clientlen);
         printf("clientfd111 = %d\n", clientfd);
         if (pthread_create(&newthreadID, NULL, accept_request, (void*)clientfd) != 0)
         {
              err_msg("pthread_create");
         }
    }

    close(serverfd);

    return 0;
}
