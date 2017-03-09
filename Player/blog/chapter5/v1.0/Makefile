CC = g++

LIBDIR = -L/usr/local/lib 
INCDIR = -I/usr/local/include/ -I../common/ -I./
OPTION = -O2 -Wall -g 
LIB_FFMPEG = -lavformat -lavcodec -lavformat -lavutil -lswresample -lswscale
LIB_SDL2 = -lSDL2 -lSDL2main
LIB_OTHER = -lx264 -lx265 -lvpx -lmp3lame -lopus -lfdk-aac -lX11 -lva -lvdpau -lva-drm -lva-x11 -lvorbisenc -lvorbis -ltheoraenc -ltheoradec -ldl -lm -lpthread -lz 

ffmpeg_decoder: 
	$(CC) audio_player_v1.0.c -o audio_player_v1.0.out $(LIBDIR) $(INCDIR) $(OPTION) $(LIB_FFMPEG) $(LIB_OTHER) $(LIB_SDL2)

v1.0.1:
	$(CC) audio_player_v1.0.2.c  -o audio_player_v1.0.1.out $(LIBDIR) $(INCDIR) $(OPTION) $(LIB_FFMPEG) $(LIB_OTHER) $(LIB_SDL2)
clean:
	rm -f *.out 
