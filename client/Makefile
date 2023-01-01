CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter
INCLUDE=../include
CLIENT_LIBS=-lpthread -lncurses

client: client.c client.h $(INCLUDE)/definitions.h message.o display.o
	$(CC) $(CFLAGS) client.c -o client message.o display.o $(CLIENT_LIBS)

message.o: message.c message.h $(INCLUDE)/definitions.h $(INCLUDE)/message_structures.h
	$(CC) $(CFLAGS) message.c -o message.o -c

display.o: display.c display.h $(INCLUDE)/definitions.h
	$(CC) $(CFLAGS) display.c -o display.o -c -lncurses

clean:
	rm -f *.o client
