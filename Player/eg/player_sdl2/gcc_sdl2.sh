#! /bin/sh
gcc simplest_ffmpeg_player.c -g -o simplest_ffmpeg_player.out -lSDL2main -lSDL2 -lavformat -lavcodec -lavutil -lswscale  -lm -lpthread   -lvorbisenc -lX11 -lvorbis
