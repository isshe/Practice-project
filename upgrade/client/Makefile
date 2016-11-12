CC = gcc 
INC = -I../wrap/ -I./ 
SRC = ../wrap/*.c ./*.c 

OBJ = client.o net_api.o 
CFLAGS = -O2 -Wall 

client : 
	$(CC) -o client.out $(CFLAGS) $(INC) $(SRC)

.PHONY : clean 
clean: 
	rm -f client.out 
	rm -f $(OBJ)
