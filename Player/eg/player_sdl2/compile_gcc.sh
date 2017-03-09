#! /bin/sh

gcc simplest_ffmpeg_player.cpp -g -o simplest_ffmpeg_player.out -lSDL2main -lSDL2  -lavutil -lswscale  -lm -lpthread -lavformat -lavcodec
