CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter

all: client

clean:
	rm -f *.o

client: client.c client.h
	$(CC) $(CFLAGS) client.c -o client
