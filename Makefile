CC=gcc

all: server client

server: server.c shared.c
	$(CC) server.c shared.c -lm -o server.out

client: client.c shared.c
	$(CC) client.c shared.c -o client.out
