/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : flash.h
* Version      : v1.0.0
* Author       : i.sshe
* Github       : github.com/isshe
* Date         : 2016/10/25
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

#ifndef _FLASH_H_
#define _FLASH_H_

#ifdef __cplusplus
extern "C"{
#endif

/*=========================================================================*\
 * #include#                                                               *
\*=========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "net_api.h"

/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/

/*=========================================================================*\
 * #enum#                                                                  *
\*=========================================================================*/

/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/
typedef struct mtd_info_user
{
     unsigned char      type;
     unsigned int       flags;
     unsigned int       size;       //total size of mtd
     unsigned int       erasesize;
     unsigned int       oobblock;   //size of OOB block (e.g. 512)
     unsigned int       oobsize;    // Amount of OOB data per block (eg 16)
     unsigned int       ecctype;
     unsigned int       eccsize;
}mtd_info_t;

typedef struct erase_info_user
{
    unsigned int start;
    unsigned int length;
}erase_info_t;

/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/


int flash_open(char *dev);
int flash_close(int fd);
int flash_mtd_size(int fd);
int flash_erase(int fd, unsigned int offset, int sec_size);
int flash_write(int fd, unsigned int offset, char *buf, int size);
ssize_t os_writen(int fd, char *buf, int size);




#ifdef __cplusplus
}
#endif

#endif

