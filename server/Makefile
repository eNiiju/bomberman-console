CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter

server: server.c server.h message.o
	$(CC) $(CFLAGS) server.c -o server message.o -lpthread

message.o: message.c message.h
	$(CC) $(CFLAGS) message.c -o message.o -c

clean:
	rm -f *.o server
