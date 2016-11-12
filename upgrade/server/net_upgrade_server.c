/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : net_upgrade_server.c
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
#include "net_upgrade_server.h"
#include "timer_list.h"
#include "net_api.h"
#include "net_upgrade.h"


net_upgrade_srv_t   g_upgrade_srv = {0};         //


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/21
* Others     (其他): 升级服务初始化
\*=======================================================================*/
int net_upgrade_svr_init(void)
{
     memset(&g_upgrade_srv, 0, sizeof(g_upgrade_srv));

     g_upgrade_srv.listen_port   = NET_UPGRADE_SVR_PORT;     //9090
     g_upgrade_srv.listen_fd     = -1;

     timer_list_init();

     //
     if (pthread_create(&g_upgrade_srv.tid, NULL, net_upgrade_task, NULL) < 0)
     {
          return -1;
     }

     g_upgrade_srv.init_ok = 1;

     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/24
* Others     (其他):
\*=======================================================================*/
static int net_upgrade_usage_effect_check(void)
{
     static int     timeout_count = 0;

     if (g_upgrade_srv.svr_busy != 1)
     {
         timeout_count = 0;
         return 0;
     }

     if (((g_upgrade_srv.session_id > 0) && (++timeout_count >= 15))
             | (g_upgrade_srv.session_id <= 0))
     {
          timeout_count = 0;
          g_upgrade_srv.session_id = 0;
          g_upgrade_srv.svr_busy = 0;
     }

     return 0;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/22
* Others     (其他): 升级服务监听线程：
*                       1. 接收升级客户端的连接，检查客户端发送的session信息，
*                           符合则允许客户端发送数据，否则关闭客户端连接。
*                       2. 如果之前请求了升级，升级服务处于占用状态，
*                           如果一段时间内没有相应的客户端过来连接，则释放占用
\*=======================================================================*/
void *net_upgrade_task(void *arg)
{
     char   server_port[32] = {0};
     int    addr_len        = 0;
     int    res             = 0;

     g_upgrade_srv.listen_fd = -1;

     hz_itoa(g_upgrade_srv.listen_port, server_port);
     debug("server port = %s\n", server_port);

     //创建一个监听套接字
     if ((g_upgrade_srv.listen_fd = tcp_listen(NULL, server_port, &addr_len)) < 0)
     {
          fprintf(stderr, "upgrade listen error: %d, %s\n",
                  g_upgrade_srv.listen_port, strerror(errno));
          return NULL;
     }
     else
     {
         fprintf(stderr, "upgrade server listen success\n");
     }

     g_upgrade_srv.listen_svr_run = 1;      //server running
     while(g_upgrade_srv.listen_svr_run == 1)
     {
          res = hz_select(&g_upgrade_srv.listen_fd, 1, SELECT_READ, 1000);
          if (res == (SELECT_READ_MASK | g_upgrade_srv.listen_fd))
          {
               printf("can read listen fd!\n");
               net_upgrade_proc();
               //
          }
          else if (res < 0)
          {
              break; //error
          }
          else if (res == 0)
          {
               //time out
               net_upgrade_usage_effect_check();
          }
     }

     close_socket(&g_upgrade_srv.listen_fd);
     printf("upgrade thread quit\n");

     return NULL;
}


int net_upgrade_proc(void)
{
     int    ret = 0;
     int    connect_fd = 0;
     int    addr_len = 0;
     char   recv_buffer[12] = {0};
     struct sockaddr    client_addr;

     addr_len = sizeof(struct sockaddr);
     memset(&client_addr, 0, addr_len);

     //接受连接
     if ((connect_fd = tcp_block_accept(g_upgrade_srv.listen_fd,
                     &client_addr, &addr_len)) < 0)
     {
          printf("upgrade server accept error: %s\n", strerror(errno));
          close_socket(&connect_fd);
          return -1;
     }

     debug("accept a connect\n");
     set_sock_noblock(connect_fd);

     //这里还有一些session id匹配代码, 不写
     //


     if ((ret = net_upgrade_recv_file(connect_fd)) < 0)
     {
          printf("upgrade server recv file error: %d\n", ret);
          net_upgrade_report(-1, NULL);
          close_socket(&connect_fd);
          return -1;
     }

     close_socket(&connect_fd);
     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 接收升级文件线程
\*=======================================================================*/
int net_upgrade_recv_file(int sockfd)
{
     int    ret = 0;
     int    recv_len = 0;
     int    total_recv = 0;
     char   recv_buffer[32768] = {0};

     //open upgrade
     if ((ret = net_upgrade_open(net_upgrade_report, NULL)) < 0)
     {
          debug("net_upgrade open error\n");
          return -1;
     }

     while(g_upgrade_srv.listen_svr_run == 1)
     {
          if ((ret = hz_select(&sockfd, 1, SELECT_READ, 10000)) < 0)
          {
              printf("upgrade server recv client file data time out or fail\n");
              break;
          }

          //接收文件数据
          if ((recv_len = tcp_noblock_recv(sockfd, recv_buffer, sizeof(recv_buffer), 0, 0)) < 0)
          {
               printf("upgrade server recv client file error\n");
               break;
          }

          //写接收到的数据
          if ((ret = net_upgrade_write(recv_buffer, recv_len)) != 0)
          {
               printf("upgrade server write file data error\n");
               break;
          }

          //写完
          if ((total_recv += recv_len) >= g_upgrade_srv.file_length)
          {
               printf("upgrade server recv file data finish\n");
               net_upgrade_write(recv_buffer, 0);
               net_upgrade_close();
               return 0;
          }
     }
     net_upgrade_close();
     return -1;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/23
* Others     (其他): 上报升级进度
\*=======================================================================*/
int net_upgrade_report(int nPercent, void *usrArg)
{
     int    upgrade_error = 0;

     //升级完成或则出错释放服务
     if ((nPercent >= 100) || (nPercent < 0))
     {
         g_upgrade_srv.session_id = 0;
         g_upgrade_srv.svr_busy = 0;
         upgrade_error = nPercent == 100 ? 0 : 1;
     }

     if (g_upgrade_srv.cmd_session == NULL)
         return -1;

     printf("upgrade percent : %d\n", nPercent);

     //return net_cmd_session_upgrade_report(g_upgrade_srv.cmd_session, nPercent, upgrade_error);
     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/24
* Others     (其他): close socket
\*=======================================================================*/
int close_socket(int *sockfd)
{
    if (sockfd != NULL && *sockfd > 0)
    {
        close(*sockfd);
        *sockfd = -1;
        return RETURN_OK;
    }

    return RETURN_FAIL;
}



