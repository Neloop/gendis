CC = gcc
CFLAGS = -Wall
LFLAGS = -rdynamic
OBJFLAGS = -fPIC -c
LIBFLAGS = -fPIC -shared
LIBS = -ldl -lcrypto

gendis: server client

server: server.c shared.o internal.o
	$(CC) $(CFLAGS) $(LFLAGS) server.c shared.o internal.o -o server $(LIBS)

client: client.c shared.o internal.o
	$(CC) $(CFLAGS) $(LFLAGS) client.c shared.o internal.o -o client $(LIBS)

train: ./libs/train.c plugin_server.h plugin_client.h shared.o
	$(CC) $(CFLAGS) $(LIBFLAGS) ./libs/train.c shared.o -o ./libs/libtrain.so -lncurses

file: ./libs/file.c plugin_server.h plugin_client.h shared.o
	$(CC) $(CFLAGS) $(LIBFLAGS) ./libs/file.c shared.o -o ./libs/libfile.so

huffman: ./libs/huffman.c plugin_server.h plugin_client.h shared.o
	$(CC) $(CFLAGS) $(LIBFLAGS) ./libs/huffman.c shared.o -o ./libs/libhuffman.so

shared.o: shared.h shared.c
	$(CC) $(CFLAGS) $(OBJFLAGS) shared.c -o $@

internal.o: internal.h internal.c
	$(CC) $(CFLAGS) $(OBJFLAGS) internal.c -o $@

lines:
	@cat internal.h plugin_client.h plugin_server.h shared.h ./libs/file.c ./libs/train.c ./libs/huffman.c client.c internal.c server.c shared.c | wc -l

clean:
	rm -fr *.o server client ./libs/*.so
