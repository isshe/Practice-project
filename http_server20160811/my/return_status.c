/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : return_status.c
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
#include "return_status.h"


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/14
* Others     (其他): 发送http头（包含文件的信息）
\*=======================================================================*/
void headers(int clientfd, const char *filename)
{
     char buf[1024];
     (void) filename; //可以用文件名来决定文件的类型

     strcpy(buf, "HTTP/1.0 200 OK\r\n");
     send(clientfd, buf, strlen(buf), 0);
     strcpy(buf, "isshe");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "Content-Type: text/html\r\n");
     send(clientfd, buf, strlen(buf), 0);
     strcpy(buf, "\r\n");
     send(clientfd, buf, strlen(buf), 0);
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/12
* Others     (其他): 返回客户端出错信息(501), 代表功能没有实现
\*=======================================================================*/
void unimplemented_501(int clientfd)
{
     char   buf[1024];
     sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "Server: isshe hello\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "Content-Type: text/html\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "</TITLE></HEAD>\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "</BODY></HTML>\r\n");
     send(clientfd, buf, strlen(buf), 0);

}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/12
* Others     (其他): 返回客户端出错信息(404), 页面没找到
\*=======================================================================*/
void not_found_404(int clientfd)
{
     char   buf[1024];
     sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "Server: isshe hello\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "Content-Type: text/html\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "</TITLE></HEAD>\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "</BODY></HTML>\r\n");
     send(clientfd, buf, strlen(buf), 0);
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/14
* Others     (其他): 400错误：错误的请求
\*=======================================================================*/
void bad_request_400(int clientfd)
{
    char    buf[1024];

    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(clientfd, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type:text/html\r\n");
    send(clientfd, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(clientfd, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request, ");
    send(clientfd, buf, sizeof(buf), 0);
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    send(clientfd, buf, sizeof(buf), 0);
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/14
* Others     (其他): 500错误：网络服务错误
\*=======================================================================*/
void cannot_execute_500(int clientfd)
{
     char   buf[1024];

     sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "Content-Type: text/html\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "\r\n");
     send(clientfd, buf, strlen(buf), 0);
     sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
     send(clientfd, buf, strlen(buf), 0);
}
