CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter

client: client.c client.h message.o
	$(CC) $(CFLAGS) client.c -o client message.o -lpthread

message.o: message.c message.h
	$(CC) $(CFLAGS) message.c -o message.o -c

clean:
	rm -f *.o client
