/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : net_api.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/21
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include "net_api.h"



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/21
* Others     (其他): 进行一些属性设置， 两个时间是毫秒
\*=======================================================================*/

int set_sock_attr(int fd, int bReuseAddr, int nSndTimeOut,
					int nRcvTimeOut, int nSndBuf, int nRcvBuf)
{
     int    err_ret = RETURN_OK;
     struct timeval     sndTo;
     struct timeval     rcvTo;

     if (fd <= 0)
     {
         return RETURN_FAIL;
     }

     //nSndTimeOut 单位是毫秒
     sndTo.tv_sec = nSndTimeOut / 1000;
     sndTo.tv_usec = (nSndTimeOut % 1000) * 1000;

     rcvTo.tv_sec = nRcvTimeOut / 1000;
     rcvTo.tv_usec = (nRcvTimeOut % 1000) * 1000;

     if (bReuseAddr != 0
             && setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&bReuseAddr, sizeof(int)) < 0)
     {
          err_ret = RETURN_FAIL;
     }

     //设置接收/发送超时
     if (nSndTimeOut != 0
             && setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (void *)&sndTo, sizeof(sndTo)) < 0)
     {
         err_ret = RETURN_FAIL;
     }

     if (nRcvTimeOut != 0
             && setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcvTo, sizeof(rcvTo)) < 0)
     {
          err_ret = RETURN_FAIL;
     }

     //设置接收/发送缓冲区
     if (nSndBuf != 0
             && setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&nSndBuf, sizeof(nSndBuf)) < 0)
     {
         err_ret = RETURN_FAIL;
     }

     if (nRcvBuf != 0
             && setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *)&nRcvBuf, sizeof(nRcvBuf)) < 0)
     {
          err_ret = RETURN_FAIL;
     }

     return err_ret;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/21
* Others     (其他): 建立连接
\*=======================================================================*/
int tcp_block_connect(const char *localHost, const char *localServ,
        const char *dstHost, const char *dstServ)
{
    int         sockfd = 0;
    int         n = 0;
    struct addrinfo     hints;
    struct addrinfo     *dstRes = NULL;
    struct addrinfo     *localRes = NULL;
    struct addrinfo     *resSave = NULL;

    if (dstHost == NULL || dstServ == NULL)
    {
         return RETURN_FAIL;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;            //自动指定适合的协议族
    hints.ai_socktype = SOCK_STREAM;

    //获取相关信息， 相关知识点在《网络编程第3版卷1》p246
    //dstHost: 主机名或地址串
    //dstServ: 服务名或端口号数串
    //hints： 暗示
    //dstRes：结果
    if ((n = getaddrinfo(dstHost, dstServ, &hints, &dstRes)) != 0)
    {
         return RETURN_FAIL;
    }
    resSave = dstRes;       //

    //来到这里，说明dstRes != NULL
    do
    {
         sockfd = socket(dstRes->ai_family,
                 dstRes->ai_socktype, dstRes->ai_protocol);
         if (sockfd < 0)
         {
             sleep(1);
             continue;
         }

         //这块if是不是可以不要？
         //不理解
         //这个程序完成的时候， 把这部分的getaddrinfo提取到前面看看是否可以
         if (localServ) //这个client中，传的localServ == NULL
         {
              //用以代替gethostbyname, gethostbyaddr, 这两个只能用于IPv4
              //localHost可以是一个主机名或者一个地址串（IPv4点分10进制或IPv6的16进制）
              //localServ可以是一个十进制的端口号，也可以是服务名称，例如：ftp, http等
              //获取到的信息已经保存到了localRes 中
              if ((n = getaddrinfo(localHost, localServ, &hints, &localRes)) != 0)
              {
                   if (resSave)
                   {
                       freeaddrinfo(resSave);
                       resSave = NULL;
                   }

                   return RETURN_FAIL;
              }

              //设置一些属性，如地址复用，发送/接收缓冲区大小, 发送接收的超时时间
              set_sock_attr(sockfd, 1, 0, 0, 0, 0);

              if (bind(sockfd, localRes->ai_addr, localRes->ai_addrlen) == 0)
              {
                   break;       //success;
              }
         }

         if (connect(sockfd, dstRes->ai_addr, dstRes->ai_addrlen) == 0)
         {
             break;
         }

         close(sockfd);

//         dstRes = dstRes->ai_next;
    }while((dstRes = dstRes->ai_next) != NULL);

    //这句感觉不合理
    if (dstRes == NULL)
    {
         sockfd = RETURN_FAIL;
    }

    if (resSave != NULL)
    {
        freeaddrinfo(resSave);
        resSave = NULL;
    }

    if (localRes != NULL)
    {
         freeaddrinfo(localRes);
         localRes = NULL;
    }

    return sockfd;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/22
* Others     (其他): 翻转字符串
\*=======================================================================*/
static void reverse(char *str, int len)
{
     int i = 0;
     int j = len - 1;
     char temp = 0;

     while(i < j)
     {
          temp = str[i];
          str[i] = str[j];
          str[j] = temp;
          i++;
          j--;
     }
}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/22
* Others     (其他): 把数字转换为字符串
\*=======================================================================*/
void hz_itoa(int src_n, char *dst_s)
{
     int i = 0;
     int save_n = src_n;

     if (save_n < 0)
     {
          src_n = -src_n;
     }

     for(i = 0; src_n > 0; i++)
     {
          dst_s[i] = src_n % 10 + '0';
          src_n /= 10;
     }

     //负的端口号
     if (save_n < 0)
     {
          dst_s[i++] = '-';
     }
     dst_s[i] = '\0';

     reverse(dst_s, i);
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/22
* Others     (其他):
\*=======================================================================*/
int tcp_listen(const char *host, const char *serv, int *addrlen)
{
     int        listenfd = 0;
//     int        n = 0;
     struct addrinfo    hints;
     struct addrinfo    *res = NULL;
     struct addrinfo    *res_save = NULL;

     memset(&hints, 0, sizeof(struct addrinfo));
     hints.ai_flags     = AI_PASSIVE;   //套接字用于被动打开
     hints.ai_family    = AF_UNSPEC;    // 协议。。。
     hints.ai_socktype = SOCK_STREAM;   //tcp

     //成功返回0
     if (getaddrinfo(host, serv, &hints, &res) != 0)
     {
          return RETURN_FAIL;
     }

     res_save = res;
     for(; res != NULL; res = res->ai_next)
     {
         listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

         printf("listenfd = %d\n", listenfd);

         if (listenfd < 0)
         {
             sleep(3);
             continue;  //try next one
         }

         set_sock_attr(listenfd, 1, 0, 0, 0, 0);

         if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
         {
              break; //success
         }

         close(listenfd);
     }

     // 多余吗？
     if (res == NULL)
     {
          if (res_save != NULL)
          {
              freeaddrinfo(res_save);
              res_save = NULL;
          }

          return RETURN_FAIL;
     }

     // 这里本来是一个包裹函数
     if (listen(listenfd, 1024) < 0)
     {
          printf("listen error\n");
          return RETURN_FAIL;
     }

     if (addrlen != NULL)
     {
         *addrlen = res->ai_addrlen;    //返回协议地址长度
     }

     if (res_save != NULL)
     {
          freeaddrinfo(res_save);
     }

     return listenfd;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/22
* Others     (其他): select函数的相关操作
\*=======================================================================*/
int hz_select(int *fd_array, int fd_num, int fd_type, int time_out)
{
     int    maxfd = 0;
     int    i = 0;
     int    ret = 0;
     fd_set read_fd;
     fd_set write_fd;
     fd_set except_fd;
     fd_set *pread_fd = NULL;
     fd_set *pwrite_fd = NULL;
     fd_set *pexcept_fd = NULL;
     struct timeval     timeO;
     struct timeval     *ptime_out = NULL;

     if (fd_array == NULL || fd_num <= 0)
     {
          return RETURN_FAIL;
     }

     //设置timeval 结构
     if (time_out > 0)
     {
         timeO.tv_sec = time_out / 1000;
         timeO.tv_usec = (time_out % 1000) * 1000;
         ptime_out = &timeO;
     }

     //这里用按位与用得不错
     if (fd_type & SELECT_READ)
     {
         pread_fd = &read_fd;
         FD_ZERO(pread_fd);
     }

     if (fd_type & SELECT_WRITE)
     {
          pwrite_fd = &write_fd;
          FD_ZERO(pwrite_fd);
     }

     if (fd_type & SELECT_EXCEPT)
     {
         pexcept_fd = &except_fd;
         FD_ZERO(pexcept_fd);
     }

     //把需要监听的套接字放到监听set里面
     for (i = 0; i < fd_num; i++)
     {
          if(fd_array[i] <= 0)
          {
              continue;
          }

          maxfd = maxfd > fd_array[i] ? maxfd : fd_array[i];

          if (pread_fd != NULL)
          {
               FD_SET(fd_array[i], pread_fd);
          }

          if (pwrite_fd != NULL)
          {
              FD_SET(fd_array[i], pwrite_fd);
          }

          if (pexcept_fd != NULL)
          {
               FD_SET(fd_array[i], pexcept_fd);
          }
     }

     if (maxfd <= 0)
     {
          return RETURN_FAIL;
     }

     maxfd += 1;

     while(1)
     {
          printf("before select\n");
          ret = select(maxfd, pread_fd, pwrite_fd, pexcept_fd, ptime_out);

          if (ret < 0 && errno == EINTR)
          {
              continue;
          }
          else if (ret < 0)
          {
               return RETURN_FAIL;
          }
          else if (ret == 0)
          {
              return RETURN_OK;
          }
          else
          {
               //同一时间只处理一个connect
               for (i = 0; i < fd_num; i++)
               {
                   if (fd_array[i] <= 0)
                   {
                       continue;
                   }

                   if (pread_fd != NULL
                           && FD_ISSET(fd_array[i], pread_fd))
                   {
                        return fd_array[i] | SELECT_READ_MASK;
                   }
                   else if (pwrite_fd != NULL
                           && FD_ISSET(fd_array[i], pwrite_fd))
                   {
                       return fd_array[i] | SELECT_WRITE_MASK;
                   }
                   else if (pexcept_fd != NULL
                           && FD_ISSET(fd_array[i], pexcept_fd))
                   {
                        return fd_array[i] | SELECT_EXCEPT_MASK;
                   }
               }

               return RETURN_OK;
          }
     }

     return RETURN_FAIL;

}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 接受客户端连接
\*=======================================================================*/
int tcp_block_accept(int fd, struct sockaddr *sa, int *psa_len)
{
    int     ret = RETURN_FAIL;

    //这里的实现需要再考虑
    if ((ret = accept(fd, sa, (socklen_t *)psa_len)) < 0)
    {
        if (errno == ECONNABORTED)  //???
        {
             printf("errno == ECONNABORTED\n");
        }
    }

    return ret;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 设置套接字为非阻塞
\*=======================================================================*/
int set_sock_noblock(int sockfd)
{
     int    no_block = 1; //== 0代表阻塞

     if (sockfd <= 0)
     {
         return RETURN_FAIL;
     }

     //这里设置的非阻塞应该是读写时候的
     if (ioctl(sockfd, FIONBIO, &no_block) < 0)
     {
          return RETURN_FAIL;
     }

     printf("set %d noblock !!!\n", sockfd);

     return RETURN_OK;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 非阻塞接收
\*=======================================================================*/
int tcp_noblock_recv(int sockfd, char *recv_buf, int buf_size,
        int recv_size, int time_out)
{
     int            ret = 0;
     unsigned long  recved = 0;
     unsigned long  try_time = 0;
     int            size = recv_size;

     if (sockfd <= 0 || recv_buf == NULL || buf_size <= 0)
     {
          return -1;
     }

     if (recv_size <= 0 || buf_size < recv_size)
     {
         size = buf_size;
     }

     while(recved < size)
     {
          printf("before recv\n");
          if ((ret = recv(sockfd, recv_buf + recved, size - recved, 0)) == 0)
          {
              printf("recv ret == 0\n");
              return 0; //接收完
          }
          else if (ret < 0)
          {
              if (ECONNRESET == errno)
              {
                   printf("recv connect reset\n");
                   return -1;
              }
              else if (EWOULDBLOCK == errno || errno == EINTR || errno == EAGAIN)
              {
                  if (try_time++ < DEFAULT_RECV_TIMEOUT)
                  {
                      printf("recv usleep\n");
                      usleep(10000);
                      continue;
                  }
                  else
                  {
                       break;
                  }
              }

              return -1;
          }

          //成功接收
          try_time = 0;
          recved += ret;
          printf("recved len = %ld\n", recved);
          if (recv_size <= 0) //没有指定接收到长度，则收到一次就返回一次
          {
               break;
          }

     }

     return recved;
}
