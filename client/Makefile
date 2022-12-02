CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter
INCLUDE=../include

client: client.c client.h $(INCLUDE)/definitions.h message.o
	$(CC) $(CFLAGS) client.c -o client message.o -lpthread

message.o: message.c message.h $(INCLUDE)/definitions.h $(INCLUDE)/message_structures.h
	$(CC) $(CFLAGS) message.c -o message.o -c

clean:
	rm -f *.o client
