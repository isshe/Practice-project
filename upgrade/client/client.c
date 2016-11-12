/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : client.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/08/09
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
#include "client.h"

static int get_file_length(const char *pathname)
{
    int fd;
    struct stat buf;

    if ((fd = open(pathname, O_RDONLY)) < 0)
    {
         return -1;
    }

    if (fstat(fd, &buf) < 0)
    {
        close(fd);
        return -1;
    }

    close(fd);

    if (S_ISREG(buf.st_mode))
    {
         return buf.st_size;
    }

    return -1;
}

/* 这个也可以，但是会改变文件指针位置，所以还是不够好
static int get_file_length(const char *pathname)
{
	FILE 	*fp = NULL;
	long 	ret = 0;
	
	if ((fp = fopen(pathname, "r")) == NULL)
	{
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	
	ret = ftell(fp);
	
	return ret;
}
*/
static void send_file(int sockfd, const char *filename)
{
    int     fd = 0;
    int     len = 0;
    int     size = 0;
    char    buffer[1024] = {0};

    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        fprintf(stderr, "open fail!\n");
        return ;
    }

    if ((size = get_file_length(filename)) < 0)
    {
         fprintf(stderr, "get file length fail\n");
    }
    else
    {
        fprintf(stderr, "get file length: %d\n", size);
    }

    while(size > 0)
    {
         if ((len = read(fd, buffer, 1024)) <= 0)
         {
             break;
         }
         send(sockfd, buffer, len, 0);
    }
}

int main(int argc, char *argv[])
{
    int     fd = 0;

    if ((fd = tcp_block_connect(NULL, NULL, "127.0.0.1", "9090")) < 0)
    {
         printf("connect error(%d, %s)\n", errno, strerror(errno));
         return -1;
    }
    else
    {
         printf("connect success\n");
    }

    //send file data
    send_file(fd, FILENAME);

	printf("sleeping\n");
	sleep(3);
    close(fd);

    return 0;
}
