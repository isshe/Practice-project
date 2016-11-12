/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : error_wrap.c
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
#include <stdio.h>
#include <stdlib.h>

void fatal_error(const char *str)
{
     perror(str);
     exit(1);
}

void warning(const char *str)
{
     perror(str);
}
