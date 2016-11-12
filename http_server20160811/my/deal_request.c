/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : deal_request.c
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
#include "deal_request.h"
#include "wrap_sock.h"
#include "wrap_error.h"
#include "return_status.h"


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/15
* Others     (其他): 获取一行
\*=======================================================================*/
int get_line(int sockfd, char *buf, int bufsize)
{
     int    i = 0;
     char   ch = '\0';
     int    recvlen = 0;

     while((i < bufsize - 1) && (ch != '\n'))
     {
         recvlen = recv(sockfd, &ch, 1, 0);
         if (recvlen > 0)         //接收到了数据
         {
              if (ch == '\r')
              {
                  //先预读取接下来的一个字符，如果是换行\n，
                  //则从tcp缓冲区读出来， 否则不读。
                  //MSG_PEEK标志的作用是复制tcp缓冲区的内容到ch，
                  //但不删除tcp缓冲区的内容。
                  recvlen = recv(sockfd, &ch, 1, MSG_PEEK);
                  if ((recvlen > 0) && (ch == '\n'))
                  {
                      recv(sockfd, &ch, 1, 0);
                  }
                  else
                  {
                       ch = '\n';
                  }
              }
              buf[i] = ch;
              i++;
        }
         else
         {
              //接收完了或者出错了
              ch = '\n';
         }
     }
     buf[i] = '\0';

     return i;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/15
* Others     (其他): 执行cgi
\*=======================================================================*/
void execute_cgi(int clientfd, const char *path,
        const char *method, const char *query_string)
{
     char       buf[1024];
     int        cgi_output[2];      //pipe
     int        cgi_input[2];       //pipe
     pid_t      pid;
     int        status = 0;
     int        i = 0;
     char       ch = '\0';
     int        recvlen = 0;        //
     int        content_length = -1;        //正文长度

     //下面两行不懂
     buf[0] = 'A';
     buf[1] = '\0';

     if (strcasecmp(method, "GET") == 0)
     {
         while((recvlen = recv(clientfd, buf, sizeof(buf) - 1, 0) > 0)
                 && (strcmp("\n", buf) != 0))
         {
             printf("!!!GET:\n%s", buf);         // 这里
         }
     }
     else   //POST
     {
         while((recvlen = get_line(clientfd, buf, sizeof(buf)) > 0)
                 && strcmp("\n", buf))         //最后以一行空行结尾
         {
              //除了获取正文长度外，还可以获取其他信息
              buf[15] = '\0'; //第16个位置
              if (strcasecmp(buf, "Content-Length:") == 0)
              {
                  content_length = atoi(&buf[16]);
                  printf("content_length = %d\n", content_length);
              }
         }

         if ( content_length == -1 )
         {
              bad_request_400(clientfd);
              return;
         }
     }

     sprintf(buf, "HTTP/1.0 200 OK\r\n");       ///
     send(clientfd, buf, strlen(buf), 0);

     if (pipe(cgi_output) < 0)
     {
         cannot_execute_500(clientfd);
         return;
     }
     if (pipe(cgi_input) < 0)
     {
         cannot_execute_500(clientfd);
         return;
     }

     //fork()
     if ((pid = fork()) < 0)
     {
          cannot_execute_500(clientfd);
          return;
     }

     if (pid == 0)         //child
     {
         char   meth_env[255];
         char   query_env[255];
         char   length_env[255];

         //复制套接字, 为什么要这两个管道不懂！！！
         //0, 1分别是标准输入、标准输出，dup2的时候，会把0，1关闭。
         //所以此时如果用printf（）标准输入的话，会看不到。
         dup2(cgi_output[1], 1);
         dup2(cgi_input[0], 0);
         close(cgi_output[0]);
         close(cgi_input[1]);

         sprintf(meth_env, "REQUEST_METHOD=%s", method);
         putenv(meth_env);
         fprintf(stderr, "!!!method = %s\n", method);
         if (strcasecmp(method, "GET") == 0)
         {
              sprintf(query_env, "QUERY_STRING=%s", query_string);
              putenv(query_env);
              fprintf(stderr, "!!!query_string = %s\n", query_string);
         }
         else   //POST
         {
             sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
             putenv(length_env);
             fprintf(stderr, "!!!content_length!! = %d\n", content_length);
         }

         //这里什么意思？
         fprintf(stderr, "!!!before excel! path = %s\n", path);
         sleep(1);
         execl(path, path, NULL);   //path不是一个html文件吗？如何执行？
         exit(0);                   //可以不要吧，都不会回来了
     }
     else       //parent
     {
         close(cgi_output[1]);
         close(cgi_input[0]);

         if (strcasecmp(method, "POST") == 0)
         {
             for (i = 0; i < content_length; i++)
             {
                  recv(clientfd, &ch, 1, 0);
                  write(cgi_input[1], &ch, 1);          //???
             }
         }

         while(read(cgi_output[0], &ch, 1) > 0)
         {
              send(clientfd, &ch, 1, 0);
         }

         close(cgi_output[0]);
         close(cgi_input[1]);
         waitpid(pid, &status, 0);
     }

}


void send_file(int clientfd, FILE *resource)
{
    char buf[1024];

    while(fgets(buf, sizeof(buf), resource) != NULL)
    {
        send(clientfd, buf, strlen(buf), 0);
    }
}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/15
* Others     (其他): 发送文件给客户端
\*=======================================================================*/
void serve_file(int clientfd, const char *filename)
{
    FILE    *resource = NULL;
    int     recvlen = 0;
    char    buf[1024];

    buf[0] = 'A';
    buf[1] = '\0';

    while((recvlen = get_line(clientfd, buf, sizeof(buf)) > 0)
                && strcmp("\n", buf) != 0)
    {
         ;          ///
    }

    resource = fopen(filename, "r");
    if (resource == NULL)
    {
        not_found_404(clientfd);
    }
    else
    {
         headers(clientfd, filename);
         send_file(clientfd, resource);
    }

    fclose(resource);
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/15
* Others     (其他): 接受请求，处理请求
\*=======================================================================*/
void *accept_request(void *fd)
{
     char       buf[1024];
     char       recvlen = 0;
     char       method[16];
     char       url[255];
     char       path[512];
     int        i = 0;
     int        j = 0;
     int        cgi = 0;
     int        clientfd = (int)fd;
     struct stat st;

     printf("clientfd = %d\n", clientfd);

     recvlen = get_line(clientfd, buf, sizeof(buf));
     printf("第一行为：%s", buf);

     //get method
     i = 0;
     while(!isspace((int)buf[i]) && (i < sizeof(method) - 1))
     {
          method[i] = buf[i];
          i++;
     }
     method[i] = '\0';

     //501错误
     if (strcasecmp(method, "GET") != 0 && strcasecmp(method, "POST") != 0)
     {
         unimplemented_501(clientfd);
         return (void*)0;
     }

     //get url
     j = 0;
     while(isspace((int)buf[i]) && (i < sizeof(buf)))   //去空格
     {
          i++;
     }

     while(!isspace((int)buf[i]) && (i < sizeof(buf)) && (j < sizeof(url) - 1))
     {
         url[j] = buf[i];
         j++;
         i++;
     }
     url[j] = '\0';

     if (strcasecmp(method, "POST") == 0)
     {
          cgi = 1;
     }

     //问号（？）后面跟的是参数
     if (strcasecmp(method, "GET") == 0)
     {
         j = 0;
         while(url[j] != '?' && url[j] != '\0')
         {
              j++;
         }

         if (url[j] == '?')
         {
             cgi = 1;
             url[j] = '\0';
             j++;           //指向下一个，为后面准备
         }
     }

     sprintf(path, "htdocs%s", url);
     if (path[strlen(path) - 1] == '/')
     {
          strcat(path, "index.html");
     }

     if (stat(path, &st) == -1)         //获取信息错误
     {
         //刚刚只是读了第一行，头部还剩下很多内容
          printf("fd = %d\n", clientfd);
          while((recvlen > 0) && strcmp("\n", buf) != 0)
          {
              recvlen = recv(clientfd, buf, sizeof(buf) - 1, 0);
              printf("stat = -1: %s", buf);
          }
          not_found_404(clientfd);
     }
     else
     {
         if (S_ISDIR(st.st_mode))
          {
              strcat(path, "/index.html");
          }

          //检查执行权限
          if ((st.st_mode & S_IXUSR)
                || (st.st_mode & S_IXGRP)
                || (st.st_mode & S_IXOTH))
          {
               cgi = 1;
          }

          if (cgi == 1)
          {
              printf("!!!execute_cgi clientfd = %d\n", clientfd);
              execute_cgi(clientfd, path, method, &url[j]);
          }
          else
          {
              printf("!!!serve_file clientfd = %d\n", clientfd);
              serve_file(clientfd, path);
          }
     }

     printf("close clientfd = %d\n", clientfd);
     close(clientfd);
}
