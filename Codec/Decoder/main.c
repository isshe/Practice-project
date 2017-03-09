/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : main.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/13
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

#include "aacinfo.h"

int main(int argc, char *argv[])
{
    AacInfo         info;
    unsigned long   *seek_table = NULL;
    int             seek_table_len = 0;
    char            *header = NULL;
    char            *object = NULL;

     if (argc < 2)
     {
         fprintf(stderr, "Usage: ./aacinfo aacfile.aac\n");
         return -1;
     }

     get_aac_info(argv[1], &info);

     fprintf(ERR_STREAM, "mpeg_version = %d\n", info.mpeg_version);
     fprintf(ERR_STREAM, "channels = %d\n", info.channels);
     fprintf(ERR_STREAM, "sample_rate = %d\n", info.sampling_rate);
     fprintf(ERR_STREAM, "frame_length = %d\n", info.frame_length);
     fprintf(ERR_STREAM, "object type = %d\n", info.object_type);
     fprintf(ERR_STREAM, "object_type_name = %s\n", info.object_type_name);

     return 0;
}
