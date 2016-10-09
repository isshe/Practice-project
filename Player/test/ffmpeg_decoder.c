/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : ffmpeg_decoder.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/09/28
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

#define __STDC_CONSTANT_MACROS
extern "C"
{
//#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
//#include <libswscale/swscale.h>     //裁剪
}
#define FILEPATH "Titanic.ts"

int main(int argc, char *argv[])
{

     AVFormatContext    *pFormatCtx;        //统筹结构,保存封装格式相关信息
     AVCodecContext     *pCodecCtx;         //保存视/音频编解码相关信息
     AVCodec            *pCodec;            //每种编解码器对应一个结构体
     AVFrame            *pFrame;            //解码后的结构
     AVFrame            *pFrameYUV;
     AVPacket           *pPacket;           //解码前
     struct SwsContext  *img_convert_ctx;   //头文件只有一行，但是实际上这个结构体十分复杂
     int                i = 0;
     int                videoindex = 0;     //为了检查哪个流是视频流，保存流数组下标
     int                y_size = 0;         //
     int                ret = 0;            //
     int                got_picture = 0;    //
     char               filepath[] = FILEPATH;  //
     int                frame_cnt;          //帧数计算

     printf("???\n");
     av_register_all();         //注册所有组件
     printf("!!!\n");

     avformat_network_init();   //初始化网络，貌似暂时没用到，可以试试删除

     pFormatCtx = avformat_alloc_context(); //分配内存

     //打开文件, 注意第一个参数是指针的指针
     if (avformat_open_input(&pFormatCtx, FILEPATH, NULL, NULL) != 0)
     {
          printf("Couldn't open input stream.\n");
          return (-1);
     }


     return 0;
}
