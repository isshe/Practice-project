/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : server.c
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

//#include "net_interface.h"
//#include "header.h"
#include "net_upgrade_server.h"

int main(int argc, char *argv[])
{
    net_upgrade_svr_init();
    while(1)
    {
         sleep(1);
    }

    return 0;
}
