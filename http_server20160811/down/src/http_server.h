/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : http_server.h
* Version      : v1.0.0
* Author       : i.sshe
* Github       : github.com/isshe
* Date         : 2016/08/14
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#ifdef __cplusplus
extern "C"{
#endif

/*=========================================================================*\
 * #include#                                                               *
\*=========================================================================*/

/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/

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
* Function   (名称): not_found()
* Description(功能): 没找到页面
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
void not_found(int client_sockfd);


/*=====================================================================\
* Function   (名称): serve_file()
* Description(功能): 发送一个文件
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
void serve_file(int client_sockfd, const char *filename);



/*=====================================================================\
* Function   (名称): headers()
* Description(功能): http 头（返回关于一个文件的信息）
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
void headers(int client, const char *filename);


/*=====================================================================\
* Function   (名称): cat()
* Description(功能): 发送文件内容
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
void cat(int client_sockfd, FILE *resource);


/*=====================================================================\
* Function   (名称): execute_cgi()
* Description(功能): 运行一个cgi脚本。这需要设置相应的环境变量
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
void execute_cgi(int client_sockfd, const char *path,
        const char *method, const char *query_string);



/*=====================================================================\
* Function   (名称): bad_request()
* Description(功能): 错误的请求（400）
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
void bad_request(int client_sockfd);

#ifdef __cplusplus
}
#endif

#endif

