CC = gcc
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all: client server server_2

client: client.o common.o
	$(CC) $(LFLAGS) client.o common.o -o client

server: server.o common.o
	$(CC) $(LFLAGS) server.o common.o -o server

server_2: server_2.o common.o
	$(CC) $(LFLAGS) server_2.o common.o -o server_2


client.o: client.c common.h
	$(CC) $(CFLAGS) client.c

server_2.o: server_2.c common.h
	$(CC) $(CFLAGS) server_2.c

server.o: server.c common.h
	$(CC) $(CFLAGS) server.c

common.o: common.h common.c
	$(CC) $(CFLAGS) common.c

clean:
	rm -rf *.o *~ client server server_2
