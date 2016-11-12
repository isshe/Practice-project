CC = gcc 
CFLAGS = -O2 -Wall -g 
INC = -I../wrap/
SRC = ../wrap/*.c ./*.c 
LIB = -lpthread 

server: 
	$(CC) -o server.out $(CFLAGS) $(INC) $(SRC) $(LIB)

.PHONY: clean 
clean: 
	rm *.out 
