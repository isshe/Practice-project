/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : aacinfo.h
* Version      : v1.0.0
* Author       : i.sshe
* Github       : github.com/isshe
* Date         : 2016/10/14
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

#ifndef _AACINFO_H_
#define _AACINFO_H_

#ifdef __cplusplus
extern "C"{
#endif

/*=========================================================================*\
 * #include#                                                               *
\*=========================================================================*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/*=========================================================================*\
 * #define#                                                                *
\*=========================================================================*/
#define ERR_STREAM      stderr


/*=========================================================================*\
 * #enum#                                                                  *
\*=========================================================================*/

/*=========================================================================*\
 * #struct#                                                                *
\*=========================================================================*/
typedef struct
{
     int mpeg_version;
     int channels;
     int sampling_rate;
     int bit_rate;
     int frame_length;
     int object_type;
     char object_type_name[16];
     int header_type;
     long file_length;
}AacInfo, *pAacInfo;

/*=========================================================================*\
 * #function#                                                              *
\*=========================================================================*/

/*=====================================================================\
* Function   (名称): show_aac_info()
* Description(功能): 获取aac文件的信息
* Called By  (被调): 1.
* Call_B file(被文): 1.
* Calls list (调用): 1.
* Calls file (调文): 1.
* Input      (输入): 1.
* Output     (输出): 1.
* Return     (返回):
*         success  :
*         failure  :
* Change log (修改): 1.
* Others     (其他): 1.
\*=====================================================================*/
void get_aac_info(char *filename, AacInfo *info);


#ifdef __cplusplus
}
#endif

#endif

