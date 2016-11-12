/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : request.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/08/12
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include "include_file.h"



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/12
* Others     (其他): 返回客户端出错信息(501), 代表功能没有实现
\*=======================================================================*/
void unimplemented(int client_sockfd)
{
     char   buf[1024];
     sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "Server: isshe hello\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "Content-Type: text/html\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "</TITLE></HEAD>\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "</BODY></HTML>\r\n");
     send(client_sockfd, buf, strlen(buf), 0);

}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/12
* Others     (其他): 返回客户端出错信息(404), 页面没找到
\*=======================================================================*/
void unimplemented(int client_sockfd)
{
     char   buf[1024];
     sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "Server: isshe hello\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "Content-Type: text/html\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "</TITLE></HEAD>\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "</BODY></HTML>\r\n");
     send(client_sockfd, buf, strlen(buf), 0);

}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/12
* Others     (其他): get_line()
\*=======================================================================*/
int get_line(int sockfd, char *buf, int size)
{
     int    i = 0;
     char   c = '\0';
     int    n = 0;

     while((i < size - 1) && (c != '\n'))
     {
          n = recv(sockfd, &c, 1, 0);
          if (n > 0)
          {
              if (c == '\r')
              {
                   n = recv(sockfd, &c, 1, MSG_PEEK);
                   if ((n > 0) && (c == '\n'))
                   {
                       recv(sockfd, &c, 1, 0);
                   }
                   else
                   {
                        c = '\n';
                   }
              }
              buf[i] = c;
              i++;
          }
          else
          {
               c = '\n';
          }
     }
     buf[i] = '\0';

     return (i);
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/14
* Others     (其他): 发送http头（包含文件的信息）
\*=======================================================================*/
void headers(int client_sockfd, const char *filename)
{
     char buf[1024];
     (void) filename; //可以用文件名来决定文件的类型

     strcpy(buf, "HTTP/1.0 200 OK\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     strcpy(buf, "isshe");
     send(client_sockfd, buf, strlen(buf), 0);
     sprintf(buf, "Content-Type: text/html\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
     strcpy(buf, "\r\n");
     send(client_sockfd, buf, strlen(buf), 0);
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/14
* Others     (其他): 发送一个文件的内容
\*=======================================================================*/
void cat(int client_sockfd, FILE *resource)
{
    char    buf[1024];

    fgets(buf, sizeof(buf), resource);
    while(!feof(resource))
    {
         send(client, buf, strlen(buf), 0);
         fgets(buf, sizeof(buf), resource);
    }
}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/14
* Others     (其他): 发送一个文件
\*=======================================================================*/
void serve_file(int client_sockfd, const char *filename)
{
     FILE   *resource = NULL;
     int    numchars = 1;
     char   buf[1024];

     buf[0] = 'A';
     buf[1] = '\0';
     while((numchars > 0) && strcmp("\n", buf))
     {
          numchars = get_line(client_sockfd, buf, sizeof(buf));
     }

     resource = fopen(filename, "r");
     if (resource == NULL)
     {
         not_found(client_sockfd);
     }
     else
     {
          headers(client, filename);
          cat(client, resource);
     }

     fclose(resource);
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/14
* Others     (其他): 400错误：错误的请求
\*=======================================================================*/
void bad_request(int client_sockfd)
{
    char    buf[1024];

    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client_sockfd, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type:text/html\r\n");
    send(client_sockfd, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client_sockfd, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request, ");
    send(client_sockfd, buf, sizeof(buf), 0);
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    send(client_sockfd, buf, sizeof(buf), 0);
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/14
* Others     (其他): 500错误：网络服务错误
\*=======================================================================*/
void cannot_execute(int client_sockfd)
{
     char   buf[1024];

     sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
     send(client_sockfd, strlen(buf), 0);
     sprintf(buf, "Content-Type: text/html\r\n");
     send(client_sockfd, strlen(buf), 0);
     sprintf(buf, "\r\n");
     send(client_sockfd, strlen(buf), 0);
     sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
     send(client_sockfd, strlen(buf), 0);
}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/14
* Others     (其他): 运行一个cgi脚本
\*=======================================================================*/
void execute_cgi(int client_sockfd, const char *path,
        const char *method, const char *query_string)
{
     char   buf[1024];
     int    cgi_output[2];
     int    cgi_input[2];
     pid_t  pid;
     int    status = 0;
     int    i = 0;
     char   c = 0;
     int    numchars = 1;
     int content_length = -1;

     //???
     buf[0] = 'A';
     buf[1] = '\0';
     if (strcasecmp(method, "GET") == 0)
     {
          while((numchars > 0) && strcmp("\n", buf))
          {
              numchars = get_line(client_sockfd, buf, sizeof(buf));
          }
     }
     else   //POST
     {
          numchars = get_line(client, buf, sizeof(buf));
          while((numchars > 0) && strcmp("\n", buf))
          {
              buf[15] = '\0';
              if (strcasecmp(buf, "Content-Length:") == 0)
              {
                  content_length = atoi(buf[16]);
              }
              numchars = get_line(client, buf, sizeof(buf));
          }

          if (content_length == -1)
          {
               bad_request(client_sockfd);
               return ;
          }
     }

     sprintf(buf, "HTTP/1.0 200 OK\r\n");
     send(client_sockfd, buf, strlen(buf), 0);

     if (pipe(cgi_output) < 0)
     {
          cannot_execute(client_sockfd);
          return;
     }

     if (pipe(cgi_input) < 0)
     {
         cannot_execute(client_sockfd);
         return ;
     }

     if ((pid = fork()) < 0)
     {
          cannot_excute(client_sockfd);
          return;
     }

     if (pid == 0)      //子进程：CGI脚本
     {
         char   meth_env[255];
         char   query_env[255];
         char   length_env[255];

         //注意这里的管道是子进程自己的。
         dup2(cgi_output[1], 1);
         dups(cgi_input[0], 0);
         close(cgi_output[0]);
         close(cgi_input[1]);

         sprintf(meth_env, "REQUEST_METHOD=%s", method);
         putenv(meth_env);

         if (strcasecmp(method, "GET") == 0)
         {
              sprintf(query_env, "QUERY_STRING=%s", query_string);
              putenv(query_env);
         }
         else   //POST
         {
             sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
             putenv(length_env);
         }

         execl(path, path, NULL);       //???
         exit(0);
     }
     else       //父进程
     {
          close(cgi_output[1]);
          close(cgi_input[0]);
          if (strcasecmp(method, "POST") == 0)
          {
              for (i = 0; i < content_length; i++)
              {
                  recv(client_sockfd, &c, 1, 0);
                  write(cgi_input[1], &c, 1);
              }
          }
          while(read(cgi_output[0], &c, 1) > 0)
          {
              send(client, &c, 1, 0);
          }

          close(cgi_output[0]);
          close(cgi_input[1]);
          waitpid(pid, &status, 0);
     }
}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/08/12
* Others     (其他): accept_request()
\*=======================================================================*/
void accept_request(int client_sockfd)
{
     char   buf[1024];
     int    numchars;
     char   method[255];
     char   url[255];
     char   path[255];
     size_t i = 0;
     size_t j = 0;
     struct stat    st;
     int    cgi = 0;
     char   *query_string = NULL;

     numchars = get_line(client, buf, sizeof(buf));
     i = 0;
     j = 0;
     while(!isspace((int)buf[j]) && (i < sizeof(method) - 1))
     {
          method[i] = buf[j];
          i++;
          j++;
     }
     method[i] = '\0';

     if ((strcasecmp(method, "GET" ) != 0)
        && (strcasecmp(method, "POST") != 0))
     {
          unimplement(client_sockfd);
          return ;
     }

     //以下处理GET和POST方法
     if (strcasecmp(method, "POST") == 0)
     {
          cgi = 1;
     }
     i = 0;
     while(isspace(buf[j]) && (j < sizeof(buf)))    //去掉空格
     {
          j++;
     }

     while(!isspace(buf[j])
             && (i < sizeof(url) - 1)
             && (j < sizeof(buf)))
     {
          url[i] = buf[j];
          i++;
          j++;
     }
     url[i] = '\0';

     if (strcasecmp(method, "GET") == 0)
     {
          query_string = url;
     }
     while((*query_string != '?') && (*query_string != '\0'))
     {
          query_string++;
     }
     if (*query_string == "?")
     {
          cgi = 1;
          *query_string = '\0';
          query_string++;
     }

     sprintf(path, "htdocs%s", url);        //???
     if (path[strlen(path) - 1] == '/')
     {
         strcat(path, "index.html");        //默认页
     }

     if (stat(path, &st) == -1)
     {
          while((numchars > 0) && strcmp("\n", buf))    //
          {
               numchars = get_line(client_sockfd, buf, sizeof(buf));
          }
          not_found(client_sockfd);
     }
     else
     {
         if ((st.st_mode & S_IFMT) == S_IFDIR)
         {
              strcat(path, "/index.html");
         }

         if ((st.st_mode & S_IXUSR)
                 || (st.st_mode & S_IXGRP)
                 || (st.st_mode & S_IXOTH))
         {
              cgi = 1;
         }

         if (!cgi)      //cgi == 0
         {
             serv_file(client_sockfd, path);
         }
         else
         {
              execute_cgi(client_sockfd, path, mothod, query_string);
         }
     }
     close(client_sockfd);
}
