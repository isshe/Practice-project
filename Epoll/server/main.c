/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : main.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/08/23
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include "comm_inc.h"
#include "main.h"
#include "wrap_sock.h"
#include "wrap_error.h"
#include "child.h"

int main(void)
{
    int     listenfd = 0;
    SA_IN   servaddr;
    int     port = 0;
    int     len = sizeof(servaddr);
    int     i = 0;
    pid_t   pid[MAXCHILD];
    pid_t   diepid;
    int     status = 0;
    child_arg_s   childarg;

    printf("输入大于1024的端口号，0为自动分配: ");
    scanf("%d", &port);

    //socket的包裹函数
    listenfd =Socket(AF_INET, SOCK_STREAM, 0);

    //
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    //绑定
    Bind(listenfd, (SA *)&servaddr, len);

    //获取自动分配的端口
    if (port == 0)
    {
         Getsockname(listenfd, (SA *)&servaddr, &len);
         port = ntohs(servaddr.sin_port);
    }
    printf("端口分：%d\n", port);

    //监听
    Listen(listenfd, MAXLISTEN);

    //创建进程，用以accept连接
    childarg.listenfd = listenfd;
    for (i = 0; i < MAXCHILD; i++)
    {
        childarg.childnum = i;
         create_process(&pid[i], child_process, (void*)&childarg);
    }

    while(1)
    {
         diepid = waitpid(-1, &status, WNOHANG);

         printf("%d die!\n", diepid);
         if (0 == diepid)
         {
              sleep(1);
              continue;
         }
//         sleep(2);

         for (i = 0; i < MAXCHILD; i++)
         {
              if (diepid == pid[i])
              {
                  childarg.childnum = i;
                  create_process(&pid[i], child_process, (void*)&childarg);
              }
         }

    }

//    close(listenfd);
    exit (0);
}
