CC = gcc
CFLAGS = -Wall
OBJFLAGS = -fPIC -c
LIBFLAGS = -fPIC -shared
LIBS = -ldl

gendis: main.c huffman.h huffman.c
	$(CC) $(CFLAGS) main.c huffman.c -o gendis

server: server.c shared.o internal.o
	$(CC) $(CFLAGS) server.c shared.o internal.o -o server $(LIBS)

client: client.c shared.o internal.o
	$(CC) $(CFLAGS) client.c shared.o internal.o -o client $(LIBS)

shared.o: shared.h shared.c
	$(CC) $(CFLAGS) $(OBJFLAGS) shared.c -o $@

internal.o: internal.h internal.c
	$(CC) $(CFLAGS) $(OBJFLAGS) internal.c -o $@

clean:
	rm -fr *.o gendis server client
