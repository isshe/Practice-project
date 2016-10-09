CC = gcc 

LIBFFMPEG = -lavformat \
			-lavcodec \
			-lavutil \

LIBSDL2 = -lSDL2 \
		 -lSDL2main \

LIBCOMM = -lm \
		  -lpthread \

all:
	$(CC) test_sdl.c -o test_sdl.out -g -Wall -O2 $(LIBSDL2) 
	$(CC) test_ffmpeg.c -o test_ffmpeg.out -g -Wall -O2 $(LIBFFMPEG) $(LIBCOMM) 

clean:
	rm -f test_sdl.out test_ffmpeg.out
