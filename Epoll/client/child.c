/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : child.c
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
#include "child.h"
#include "wrap_error.h"
#include "wrap_sock.h"
#include "client.h"
#include "wrap_epoll.h"

int create_process(pid_t *pid, int (*proc)(void *arg), void *arg)
{
    pid_t   forkpid;
    int     exit_code = 0;

    forkpid = fork();       //错误处理呢
    switch( forkpid )
    {
        case 0:
            exit_code = proc(arg);      //如果子进程出错，可以在重启：加个while()
            exit(exit_code);
        case -1:
            err_ret("fork error");
            return errno;
        default:
            break;
//            *pid = forkpid;
    }

    return 0;
}

int child_process(void *arg)
{
    char    cliip[16] = {0};
    int     i = 0;
    int     servport = SERVER_PORT;
    int     cliport = 0;
    char    serv_ip[16] = SERVER_IP;
    int     sockfd = 0;
    int     all_sockfd[PROCTOCONN] = {0};        //把fd存起来，100w个fd的话需要4M左右内存
    int     count_sucfd = 0;
    struct  epoll_event ev;
    struct  epoll_event events[MAX_EVENTS];
    char    buf[64] = "say something";
    int     nfds = 0;
    int     j = 0;
    int     epollfd = 0;
    
//    int     fds[MAX_SOCKFD] = {0}; //开1024个一个连接，进程默认只能打开这么多
    child_arg_s *myarg = (child_arg_s*)arg;
    SA_IN   servaddr;
    SA_IN   cliaddr;
    int     sain_len = sizeof(SA_IN);


    //填充服务器的信息
    memset(&servaddr, 0, sizeof(SA_IN));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(servport);
    inet_pton(AF_INET, serv_ip, &servaddr.sin_addr);

//    Setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt,sizeof(opt));

    //填充客户端信息，主要是为了使用指定的IP和端口。
    memcpy(cliip, myarg->ip, strlen(myarg->ip));
    memset(&cliaddr, 0, sizeof(SA_IN));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(cliport);
    inet_pton(AF_INET, cliip, &cliaddr.sin_addr);

//    printf("childnum = %d, ip = %s\n", myarg->childnum, myarg->ip);
    count_sucfd = 0;
    if ((epollfd = Epoll_create(0)) == -1)
    {
        return RET_FAILURE;
    }
    for (i = 0; i < PROCTOCONN; i++)
    {
        sockfd = Socket(AF_INET, SOCK_STREAM, 0);
        bind(sockfd, (SA *)&cliaddr, sain_len);
        if (connect(sockfd, (SA *)&servaddr, sizeof(SA_IN)) == 0)       //success
        {
            //保存连接成功的fd
            all_sockfd[count_sucfd++] = sockfd;
 //           send(sockfd, buf, strlen(buf), 0);
            
        }
    }
 /*   
    for (i = 0; i < count_sucfd; i++)
    {
        //把fd加入到epoll的监听集合里面, 如果成功就发送
        ev.events = EPOLLIN;
        ev.data.fd = all_sockfd[i];
        if (Epoll_ctl(epollfd, EPOLL_CTL_ADD, all_sockfd[i], &ev) == 0)
        {
            //发送，
            if (send(all_sockfd[i], buf, strlen(buf), 0) < 0)
            {
                printf("%d send error\n", all_sockfd[i]);
            }
        }
      
       //等待0.1ms, 如果有准备好的fd， 则接收
        //可能会有些接收不到，当循环结束的时候。
        if ((nfds = Epoll_wait(epollfd, events, MAX_EVENTS, 100)) > 0)
        {
            for (j = 0; j < nfds; j++)
            {
                recv(events[j].data.fd, buf, 64, 0);           //接收了，但是不要打印。
            }
        }

    }
*/
 /*   
    //接收遗漏的， 10s
        while ((nfds = Epoll_wait(epollfd, events, MAX_EVENTS, 500000)) > 0)
        {
            for (j = 0; j < nfds; j++)
            {
                recv(events[j].data.fd, buf, 64, 0);           //接收了，但是不要打印。
            }
        }
*/
    printf("childnum = %d, ip = %s\n", myarg->childnum, myarg->ip);

    sleep(150);      //停下来一段时间。然后把fd给close掉。

    // close(fds[i]);

    return RET_SUCCESS;
}
