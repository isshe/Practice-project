/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : net_upgrade_server.h
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

#ifndef _NET_UPGRADE_SERVER_H_
#define _NET_UPGRADE_SERVER_H_

#ifdef __cplusplus
extern "C"{
#endif

/*=========================================================================*\
 * #include#                                                               *
\*=========================================================================*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>

/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/
#define NET_UPGRADE_SVR_PORT        9090


/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/
typedef struct _net_upgrade_srv_t_
{
     int        init_ok;        //服务是否已经初始化
     int        listen_fd;      //升级监听的sockfd
     int        listen_port;    //升级绑定的本地端口
     int        listen_svr_run; //监听升级线程是否在运行
     int        svr_busy;       //升级服务线程正在运行：
                                // 1. 无升级相关操作
                                // 2. 已请求升级，未发送升级文件
                                // 3. 正在执行升级接收和擦写flash
     pthread_t  tid;            //升级线程的id
     pthread_mutex_t    mutex;  //互斥信号
     int        session_id;     //升级的session的id 号
     int        file_length;    //文件长度
     void       *cmd_session;   //升级命令的sessin
}net_upgrade_srv_t;


/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/


/*=====================================================================\
* Function   (名称):
* Description(功能):
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
int net_upgrade_svr_init(void);


/*=====================================================================\
* Function   (名称): net_upgrade_task()
* Description(功能): 升级服务监听线程
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
void *net_upgrade_task(void *arg);



/*=====================================================================\
* Function   (名称): net_upgrade_proc()
* Description(功能): 接受客户端连接，判断是否启动接收升级文件
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
int net_upgrade_proc(void);



/*=====================================================================\
* Function   (名称): net_upgrade_recv_file()
* Description(功能): 接收升级文件
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
int net_upgrade_recv_file(int sockfd);


/*=====================================================================\
* Function   (名称): net_upgrade_report()
* Description(功能): 显示进度?!疑问
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
int net_upgrade_report(int nPercent, void *usrArg);


/*=====================================================================\
* Function   (名称): close_socket()
* Description(功能): 关闭套接字
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
int close_socket(int *sockfd);


#ifdef __cplusplus
}
#endif

#endif

