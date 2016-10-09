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
#include "main.h"
#include "child.h"
#include "wrap_error.h"
#include "wrap_sock.h"

int create_process(pid_t *pid, int (*proc)(void *arg), void *arg)
{
    pid_t   forkpid;
    int     exit_code = 0;

    forkpid = fork();       //错误处理呢
    switch( forkpid )
    {
        case 0:
            exit_code = proc(arg);
            exit(exit_code);
        case -1:
            err_ret("fork error");
            return errno;
        default:
            *pid = forkpid;
    }

    return 0;
}

int child_process(void *arg)
{
     int    i = 0;
     child_arg_s  *p = (child_arg_s*)arg;
     int    listenfd = p->listenfd;
     int    childnum = p->childnum;
     int    connfd = 0;
     int    nfds = 0;               //准备好fd个数
     int    epollfd = 0;
     int    n = 0;
     SA_IN  clientaddr;
     int    addrlen = sizeof(SA_IN);
     struct epoll_event ev;
     struct epoll_event events[MAX_EVENTS];
     int    count = 0;
     int    eventfd = 0;

    printf("start child: %d\n", childnum);

     epollfd = epoll_create1(0);
     if (-1 == epollfd)
     {
         err_ret("epoll_create1 error");
         return RET_FAILURE;
     }

     ev.events = EPOLLIN;
     ev.data.fd = listenfd;
     if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
     {
          err_ret("epoll_ctl error");
          return RET_FAILURE;
     }

//     for (i = 0; i < MAX_ACCEPT; i++)
     while(1)// && count < MAX_OPEN_FILE)
     {
         nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
         if (nfds == -1)
         {
              err_ret("epoll_wait error");
              return RET_FAILURE;
         }

         for (n = 0; n < nfds; n++)
         {
             if (events[n].data.fd == listenfd)      //有连接
             {
                  connfd = Accept(listenfd, (SA *)&clientaddr, &addrlen);
                  count++;
                  if (count % 100 == 0 || nfds % 100 == 0)
                      printf("childnum:%d, count:%d, nfds:%d.\n", childnum, count, nfds);
//                  setnonblocking(connfd);       //
                  ev.events = EPOLLIN;// | EPOLLOUT; //EPOLLET
                  ev.data.fd = connfd;
                  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1)
                  {
                       err_ret("epoll_ctl error");
                       close(connfd);
                       //continue; //return;
                  }
             }
             else
             {
                  if (events[n].events & EPOLLIN)
                  {
                       do_use_fd(events[n].data.fd);
                  }
                  else
                  {
                      //error
                  }
             }
         }

     }


     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/09/07
* Others     (其他): do_use_fd():响应客户端的请求。
\*=======================================================================*/
void do_use_fd(int fd)
{
    char buf[64] = {0};

    while(Recv(fd, buf, 64, 0) > 0)
    {
         Send(fd, buf, strlen(buf), 0);
    }
}
