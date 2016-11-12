/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : main.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/08/11
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include "error_wrap.h"
#include "include_file.h"

int main(void)
{
    int     server_sockfd = -1;
    int     client_sockfd = -1;
    u_short     port = 0;
    int     client_name_len = 0;
    struct  sockaddr_in     client_name;
    pthread_t   newthread;

    client_name_len = sizeof(client_name);

    server_sockfd = server_socket(port);

    while (1)
    {
        client_sockfd = accept(server_sockfd,
                            (struct sockaddr *) & client_name,
                            &client_name_len);
        if (client_sockfd == -1)
        {
             fatal_error("accept");
        }

        if (pthread_create(&newthread, NULL,
                    accept_request, client_sockfd) != 0)
        {
             warning("pthread_create");
        }
    }

    close(server_sockfd);

    return 0;
}
