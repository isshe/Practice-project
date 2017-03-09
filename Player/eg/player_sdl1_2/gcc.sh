#! /bin/sh
gcc simplest_ffmpeg_player.c -g -o smp.out -lSDLmain -lSDL -lavutil -lavformat -lavcodec  -lswscale  -lm -lpthread -lz -ldl -lX11 -lvorbisenc -lvorbis  -lmp3lame -lx264 -lvpx -lopus -lx264 -lx265 -lfdk-aac -lpostproc -ltheora


#-L/home/isshe/ffmpeg/build/lib -I/home/isshe/ffmpeg/build/include 
