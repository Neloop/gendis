client: common.o client.o
	gcc -lcrypto -Wall -m64 client.o common.o -o client

server: common.o server.o
	gcc -lcrypto -Wall -m64 server.o common.o -o server

common.o: common.c common.h
	gcc -c -Wall -m64 common.c -o common.o

client.o: client.c
	gcc -c -Wall -m64 client.c -o client.o

server.o: server.c
	gcc -c -Wall -m64 server.c -o server.o
