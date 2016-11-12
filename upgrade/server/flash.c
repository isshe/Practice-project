/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : flash.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/25
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include "flash.h"

static pthread_mutex_t  g_flash_mutex = PTHREAD_MUTEX_INITIALIZER;
#define MEMGETINFO      _IOR('M', 1, struct mtd_info_user)
#define FLASH_ERASE     _IOW('M', 2, struct erase_info_user)

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 打开flash
\*=======================================================================*/
int flash_open(char *dev)
{
     int fd = -1;

     if (dev == NULL)
     {
         return -1;
     }

     pthread_mutex_lock(&g_flash_mutex);
     if ((fd = open(dev, O_RDWR)) < 0)
     {
          pthread_mutex_unlock(&g_flash_mutex);
          fd = -1;
     }

     //这里不要解锁，在close的时候再解锁

     return fd;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 关闭flash
\*=======================================================================*/
int flash_close(int fd)
{
     if (fd > 0)
     {
         close(fd);
         pthread_mutex_unlock(&g_flash_mutex);
     }

     return 0;
}

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 获取flash mtd 大小 (flash memory block size)
\*=======================================================================*/
int flash_mtd_size(int fd)
{
     mtd_info_t     info;
     int            ret = 0;

     if (fd < 0)
     {
         return -1;
     }

     if ((ret = ioctl(fd, MEMGETINFO, &info)) < 0)
     {
          return -1;
     }

     return info.size;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 擦除
\*=======================================================================*/
int flash_erase(int fd, unsigned int offset, int sec_size)
{
     int            ret = 0;
     erase_info_t   info;

     if (fd < 0 || sec_size < 0)
     {
         return -1;
     }

     info.start = offset;
     info.length = sec_size;

     if ((ret = ioctl(fd, FLASH_ERASE, &info)) < 0)
     {
          printf("flash erase fail: %d, %s\n", errno, strerror(errno));
     }

     return ret;
}


/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 写flash
\*=======================================================================*/
int flash_write(int fd, unsigned int offset, char *buf, int size)
{
     int    ret = 0;

     if (fd < 0 || buf == NULL || size < 0)
     {
         return -1;
     }

     if ((ret = lseek(fd, offset, SEEK_SET)) < 0)
     {
          return -2;
     }

     if (os_writen(fd, buf, size) != size)
     {
         printf("flash write fail(fd:%d, %p, %d)\n", fd, buf, size);
         return -3;
     }

     return 0;
}



/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/25
* Others     (其他): 写flash
\*=======================================================================*/
ssize_t os_writen(int fd, char *buf, int size)
{
     size_t     nleft = 0;
     ssize_t    nwritten = 0;
     char       *ptr = NULL;

     if (fd < 0 || buf == NULL || size < 0)
     {
         return -1;
     }

     ptr = buf;
     nleft = size;

     printf("os_write:%d, %p, %d\n", fd, buf, size);

     while(nleft > 0)
     {
          if ((nwritten = write(fd, ptr, nleft)) <= 0)
          {
              if (errno == EINTR)
              {
                  nwritten = 0;
              }
              else
              {
                   printf("write error:%d, %s\n", errno, strerror(errno));
                   return -1;
              }
          }

          if (errno == 5)
          {
               return -1;
          }

          nleft -= nwritten;
          ptr   += nwritten;

          printf("os write %d\n", nwritten);
     }

     return size;
}

