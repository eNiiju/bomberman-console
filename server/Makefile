CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter
INCLUDE=../include

server: server.c server.h $(INCLUDE)/definitions.h message.o
	$(CC) $(CFLAGS) server.c -o server message.o -lpthread

message.o: message.c message.h $(INCLUDE)/definitions.h $(INCLUDE)/message_structures.h
	$(CC) $(CFLAGS) message.c -o message.o -c

clean:
	rm -f *.o server
