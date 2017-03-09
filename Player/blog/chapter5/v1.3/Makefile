CC = g++

SOURCE = *.c ../../common/*.c
TARGET = audio_player_v1.3.out
LIBDIR = -L/usr/local/lib 
INCDIR = -I/usr/local/include/ -I../../common/ 
OPTION = -O2 -Wall -g 
LIB_FFMPEG = -lavformat -lavcodec -lavformat -lavutil -lswresample -lswscale
LIB_SDL2 = -lSDL2 -lSDL2main
LIB_OTHER = -lx264 -lx265 -lvpx -lmp3lame -lopus -lfdk-aac -lX11 -lva -lvdpau -lva-drm -lva-x11 -lvorbisenc -lvorbis -ltheoraenc -ltheoradec -ldl -lm -lpthread -lz 


all:
	$(CC) $(SOURCE) -o  $(TARGET) $(LIBDIR) $(INCDIR) $(OPTION) $(LIB_FFMPEG) $(LIB_OTHER) $(LIB_SDL2)
clean:
	rm -f *.out 
