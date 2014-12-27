CC = gcc
CFLAGS = -Wall
LFLAGS = -rdynamic
OBJFLAGS = -fPIC -c
LIBFLAGS = -fPIC -shared
LIBS = -ldl

gendis: main.c huffman.h huffman.c
	$(CC) $(CFLAGS) main.c huffman.c -o gendis

server: server.c shared.o internal.o
	$(CC) $(CFLAGS) $(LFLAGS) server.c shared.o internal.o -o server $(LIBS)

client: client.c shared.o internal.o
	$(CC) $(CFLAGS) $(LFLAGS) client.c shared.o internal.o -o client $(LIBS)

train: ./libs/train.c plugin_server.h plugin_client.h shared.o
	$(CC) $(CFLAGS) $(LIBFLAGS) ./libs/train.c shared.o -o ./libs/train.so -lncurses

shared.o: shared.h shared.c
	$(CC) $(CFLAGS) $(OBJFLAGS) shared.c -o $@

internal.o: internal.h internal.c
	$(CC) $(CFLAGS) $(OBJFLAGS) internal.c -o $@

clean:
	rm -fr *.o gendis server client ./libs/*.so
