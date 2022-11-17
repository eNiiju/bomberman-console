CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter

all: server

clean:
	rm -f *.o

server: server.c server.h
	$(CC) $(CFLAGS) server.c -o server -lpthread
