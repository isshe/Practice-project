// ffmpeg_sdl.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>

#include "SDL2/SDL.h"

#define SCREEN_W    640             //窗口的宽
#define SCREEN_H    360             //窗口的高
#define PIXEL_W     320             //窗口里内容的宽 (视频像素的宽）
#define PIXEL_H     180             //窗口里内容的高（视频像素的高）
#define BPP         12              //像素深度：指存储每个像素所用的位数（bit）
#define BUF_LEN     ((PIXEL_W) * (PIXEL_H) * (BPP) / 8)     //存一帧的需要空间

const int bpp = BPP;
int screen_w = SCREEN_W;
int screen_h = SCREEN_H;
const int pixel_w = PIXEL_W;
const int pixel_h = PIXEL_H;

unsigned char buffer[BUF_LEN];

int main(int argc, char* argv[])
{
	if(SDL_Init(SDL_INIT_VIDEO)) {  
		printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
		return -1;
	} 

	SDL_Window *screen; 
	//SDL 2.0 Support for multiple windows
	screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h,SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	if(!screen) {  
		printf("SDL: could not create window - exiting:%s\n",SDL_GetError());  
		return -1;
	}
	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);  

	Uint32 pixformat=0;
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	pixformat= SDL_PIXELFORMAT_IYUV;  

	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer,pixformat, SDL_TEXTUREACCESS_STREAMING,pixel_w,pixel_h);

	FILE *fp=NULL;
	fp=fopen("test_yuv420p_320x180.yuv","rb+");

	if(fp==NULL){
		printf("cannot open this file\n");
		return -1;
	}

	SDL_Rect sdlRect;  

	while(1){
			if (fread(buffer, 1, pixel_w*pixel_h*bpp/8, fp) != pixel_w*pixel_h*bpp/8){
				// Loop
				fseek(fp, 0, SEEK_SET);
				fread(buffer, 1, pixel_w*pixel_h*bpp/8, fp);
			}

			SDL_UpdateTexture( sdlTexture, NULL, buffer, pixel_w);  

			sdlRect.x = 0;  
			sdlRect.y = 0;  
			sdlRect.w = screen_w;  
			sdlRect.h = screen_h;  
			
			SDL_RenderClear( sdlRenderer );   
			SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, &sdlRect);  
			SDL_RenderPresent( sdlRenderer );  
			//Delay 40ms
			SDL_Delay(40);
			
	}
	SDL_Quit();
	return 0;
}

