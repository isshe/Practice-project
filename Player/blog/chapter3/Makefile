CC = g++

LIBDIR = -L/usr/local/lib 
INCDIR = -I/usr/local/include/ 
OPTION = -O2 -Wall -g 
LIB_FFMPEG = -lavformat -lavcodec -lavformat -lavutil -lswresample -lswscale
LIB_SDL2 = -lSDL2 -lSDL2main
LIB_OTHER = -lx264 -lx265 -lvpx -lmp3lame -lopus -lfdk-aac -lX11 -lva -lvdpau -lva-drm -lva-x11 -lvorbisenc -lvorbis -ltheoraenc -ltheoradec -ldl -lm -lpthread -lz 

ffmpeg_decoder: 
	$(CC) ffmpeg_decoder.c -o ffmpeg_decoder.out $(LIBDIR) $(INCDIR) $(OPTION) $(LIB_FFMPEG) $(LIB_OTHER) 



clean:
	rm -f *.out 
	rm -f *.info *.H264 *.yuv
