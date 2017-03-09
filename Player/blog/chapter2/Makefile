CC = g++

LIBDIR = -L/usr/local/lib 
INCDIR = -I/usr/local/include/ 
OPTION = -O2 -Wall -g 
LIB_FFMPEG = -lavformat -lavcodec -lavformat -lavutil -lswresample
LIB_SDL2 = -lSDL2 -lSDL2main
LIB_OTHER = -lx264 -lx265 -lvpx -lmp3lame -lopus -lfdk-aac -lX11 -lva -lvdpau -lva-drm -lva-x11 -lvorbisenc -lvorbis -ltheoraenc -ltheoradec -ldl -lm -lpthread -lz 

all:
	$(CC) 1_SDL_create_window.c -o 1_SDL_create_window.out $(OPTION) $(LIB_SDL2) 
	$(CC) 2_SDL_pthread_event.c -o 2_SDL_pthread_event.out \
			$(OPTION) $(LIBDIR) $(INCDIR) $(LIB_SDL2)

ffmpeg_decoder: 
	$(CC) ffmpeg_decoder.c -o ffmpeg_decoder.out \
			$(LIBDIR) $(INCDIR) $(OPTION) $(LIB_FFMPEG) $(LIB_OTHER) 

1_SDL_create_window:
	$(CC) 1_SDL_create_window.c -o 1_SDL_create_window.out $(OPTION) $(LIB_SDL2) 

2_SDL_pthread_event:
	$(CC) 2_SDL_pthread_event.c -o 2_SDL_pthread_event.out \
			$(OPTION) $(LIBDIR) $(INCDIR) $(LIB_SDL2)

clean:
	rm -f *.out 
