/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : wrap_epoll.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/09/07
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include <sys/epoll.h>

#include "wrap_epoll.h"
#include "wrap_error.h"



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/09/07
* Others     (其他): epoll_create()的包裹函数
\*=======================================================================*/
int Epoll_create(int flag)
{
     int epollfd = 0;

     //epoll_create() or epoll_create1() ！再学习的时候再改
     if ((epollfd = epoll_create1(flag)) == -1)
     {
          err_ret("epoll_create error");
     }
     return epollfd;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/09/07
* Others     (其他): Epoll_ctl()的包裹函数
\*=======================================================================*/
int Epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
    int ret = 0;

    if ((ret = epoll_ctl(epfd, op, fd, event)) == -1)
    {
         err_ret("epoll_ctl error");
    }

    return ret;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/09/07
* Others     (其他): epoll_wait()的包裹函数
\*=======================================================================*/
int Epoll_wait(int epfd, struct epoll_event *events,
               int maxevents, int timeout)
{
     int nfds = 0;

     if ((nfds = epoll_wait(epfd, events, maxevents, timeout)) == -1)
     {
          err_ret("epoll_wait error");
     }

     return nfds;
}
