/*=========================================================================\
* Copyright(C)2016 Chudai.
*
* File name    : 2_SDL_pthread_event.c
* Version      : v1.0.0
* Author       : 初代
* Date         : 2016/09/29
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
#include <errno.h>
#include <string.h>

#include <SDL2/SDL.h>

#define SCREEN_W    480             //窗口的宽
#define SCREEN_H    272             //窗口的高
#define PIXEL_W     480             //视频像素的宽，要和视频文件相同才能显示正常
#define PIXEL_H     272             //像素的高
#define BPP         12              //像素深度：指存储每个像素所用的位数（bit）
#define BUF_LEN     ((PIXEL_W) * (PIXEL_H) * (BPP) / 8)     //存一帧的需要空间
#define FILENAME    "test_yuv420p_320x180.yuv"
#define MY_DEFINE_REFRESH_EVENT     (SDL_USEREVENT + 1)
#define MY_DEFINE_BREAK_EVENT       (SDL_USEREVENT + 2)

int thread_exit = 0;
static int refresh_func(void *arg)
{
    SDL_Event   event;
    thread_exit = 0;

    while(0 == thread_exit)
    {
        event.type = MY_DEFINE_REFRESH_EVENT;
        SDL_PushEvent(&event);      //发送一个事件，使主线程继续运行
        SDL_Delay(40);
    }

    //子线程退出后发送事件给主线程，使主线程也退出
    thread_exit = 0;
    event.type = MY_DEFINE_BREAK_EVENT;
    SDL_PushEvent(&event);
    return 0;
}

const int bpp = BPP;

int main(int argc, char *argv[])
{
    int screen_w = SCREEN_W;
    int screen_h = SCREEN_H;
    const int pixel_w = PIXEL_W;
    const int pixel_h = PIXEL_H;
    unsigned char buffer[BUF_LEN + 1];      //注意类型
    char            filename[256] = FILENAME;

    SDL_Window      *screen = NULL;         //窗口数据结构
    SDL_Renderer    *sdlRenderer = NULL;    //渲染器数据结构
    Uint32          pixformat = 0;
    SDL_Texture     *sdlTexture = NULL;     //主要操作的
    FILE            *fp = NULL;
    SDL_Rect        sdlRect;
//    SDL_Thread      *refresh_thread = NULL; //线程数据结构
    SDL_Event       event;                  //事件数据结构

    //注意：可以把文件传进来了，但是如果不使用ffmmpeg的函数还不知道怎么改像素值，以使视频正常显示！！！！！
    if (argc > 2)
    {
         printf("Usage: ./*.out videofile.yuv\n");
         return 0;
    }
    else if (argc == 2)
    {
         memcpy(filename, argv[1], strlen(argv[1]) + 1);
//         filename[strlen(argv[1])] = '\0';
    }
    printf("video file name: %s\n", filename);

    if (SDL_Init(SDL_INIT_VIDEO))
     {
         printf("Couldn't initialize SDL - %s\n", SDL_GetError());
         return (-1);
     }

    screen = SDL_CreateWindow("isshe Video Player SDL2",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            screen_w, screen_h,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!screen)
    {
         printf("SDL:Couldn't not create window error: %s\n", SDL_GetError());
         return (-1);
    }

    //创建渲染器，-1，0不懂什么意思，再看这个函数的定义
    sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

    //在pixels.h中，大概是指定输入数据格式？不懂！
    pixformat = SDL_PIXELFORMAT_IYUV;

    sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat,
            SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);

    //打开文件
    fp = fopen(filename, "r");
    if (NULL == fp)
    {
        printf("Open file error:%s\n", strerror(errno));
        return (-1);
    }

    //新建线程
//    refresh_thread =
    SDL_CreateThread(refresh_func, NULL, NULL);

    while(1)
    {
         //等待一个事件
         SDL_WaitEvent(&event);     //事件的信息存到结构中了

         //处理事件, 尝试使用自定义的事件
         if (event.type == MY_DEFINE_REFRESH_EVENT)
         {
              //读一帧
              if (fread(buffer, 1, BUF_LEN, fp) != BUF_LEN) //出错或结尾
              {
                  //重定位会文件头部
                  fseek(fp, 0, SEEK_SET);
                  continue;         //
              }

              //更新纹理，但是不懂最后一个参数，是一次更新一行吗？
              SDL_UpdateTexture(sdlTexture, NULL, buffer, pixel_w);

              sdlRect.x = 0;
              sdlRect.y = 0;
              sdlRect.w = screen_w;
              sdlRect.h = screen_h;

              SDL_RenderClear(sdlRenderer);
              //把数据从第二个参数复制到第一个参数
              SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
              SDL_RenderPresent(sdlRenderer);
         }
         else if (event.type == SDL_WINDOWEVENT)
         {
              //获取像素窗口的大小，窗口拉伸的时候用这个则会自动调整
              SDL_GetWindowSize(screen, &screen_w, &screen_h);
         }
         else if (event.type == SDL_QUIT)
         {
             thread_exit = 1;
         }
         else if (event.type == MY_DEFINE_BREAK_EVENT)    //线程结束，主线程也结束
         {
              break;
         }
    }

    fclose(fp);
    SDL_Quit();
    return 0;
}
