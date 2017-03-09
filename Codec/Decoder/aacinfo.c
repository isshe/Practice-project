/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : aacinfo.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/10/14
* Description  :
* Function list: 1.
*                2.
*                3.
* History      :
\*=========================================================================*/

/*-----------------------------------------------------------*
 * 头文件                                                    *
 *-----------------------------------------------------------*/
#include "aacinfo.h"

/*======================================================================\
* Author     (作者): i.sshe
* Date       (日期): 2016/10/14
* Others     (其他): show_aac_info()
\*=======================================================================*/
static int sample_rate_array[] = {96000, 88200, 64000, 48000, 41000,
                                    32000, 24000, 22050, 16000,
                                    12000, 11025, 8000};

void get_aac_info(char *filename, AacInfo *info)
{
     FILE   *fd = NULL;
     char   buffer[10];
     size_t read_len = 0;


     memset(info, 0, sizeof(AacInfo));
     if ((fd = fopen(filename, "r")) == NULL)
     {
          fprintf(ERR_STREAM, "fopen error\n");
          return ;
     }

     fseek(fd, 0, SEEK_END);
     info->file_length = ftell(fd);
     fseek(fd, 0, SEEK_SET);

     if ((read_len = fread(buffer, 1, 10, fd)) != 10)
     {
          if (feof(fd) != 0)
          {
              fprintf(ERR_STREAM, "EOF!\n");
          }
          else
          {
               fprintf(ERR_STREAM, "fread error\n");
          }
     }

     if (memcmp(buffer, "ADIF", 4) == 0)
     {
          fprintf(ERR_STREAM, "ADIF 格式!\n");
          return ;
     }
     else
     {
         fprintf(ERR_STREAM, "ADTS 格式\n");
         fprintf(ERR_STREAM, "buffer = %x\n", buffer[1]);
         //syncword
         if (((buffer[0] & 0xFF)== 0xFF) && ((buffer[1] & 0xF6) == 0xF0))
         {
              fprintf(ERR_STREAM, "found syncword\n");

              //mpeg_version
              int ID = buffer[1] & 0x08;
              if (ID == 0)
              {
                   info->mpeg_version = 4;
              }
              else
              {
                  info->mpeg_version = 2;
              }

              // channels, 共取3位
              info->channels = ((buffer[2] & 0x01) << 2)
                                |((buffer[3] & 0xC0) >> 6);

              //sample rate
              int sample_rate_index = (buffer[2] & 0x3C) >> 2;
              info->sampling_rate = sample_rate_array[sample_rate_index];

              //object type
              info->object_type = (buffer[2] & 0xC0) >> 6; //profile
              switch(info->object_type)
              {
                  case 0:
                      memcpy(info->object_type_name, "MAIN", 4);
                      break;
                  case 1:
                      memcpy(info->object_type_name, "LC", 2);
                      break;
                  case 2:
                      memcpy(info->object_type_name, "SSR", 3);
                      break;
                  case 3:
                      memcpy(info->object_type_name, "LTP", 3);
                      break;
                  default:
                      break;
              }

              //variable
              //frame_length
              info->frame_length |= ((unsigned int)buffer[3] & 0x3) << 11;
              info->frame_length |= ((unsigned int)buffer[4]) << 3;
              info->frame_length |= (buffer[5] >> 5);
         }
     }

     fclose(fd);
}
