#! /bin/sh 
g++ ffmpeg_decoder.c -o ffmpeg_decoder -O2 -Wall -g  -L/usr/local/lib -lavformat -lavcodec -lavformat -lavutil -lswresample -lx264 -lx265 -lvpx -lmp3lame -lopus -lfdk-aac -lX11 -lva -lvdpau -lva-drm -lva-x11 -lvorbisenc -lvorbis -ltheoraenc -ltheoradec -ldl -lm -lpthread -lz 
