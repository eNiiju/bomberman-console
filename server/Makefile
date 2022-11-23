CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter

server: server.c server.h memory.o
	$(CC) $(CFLAGS) server.c -o server memory.o -lpthread

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) memory.c -o memory.o -c

clean:
	rm -f *.o server
