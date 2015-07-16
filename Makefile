CC = gcc
CFLAGS = -Wall
LFLAGS = -rdynamic
OBJFLAGS = -fPIC -c
LIBFLAGS = -fPIC -shared
LIBS = -ldl -lcrypto

gendis: server client

server: server.c shared.o internal.o common.o
	$(CC) $(CFLAGS) $(LFLAGS) server.c shared.o internal.o common.o -o server $(LIBS)

client: client.c shared.o internal.o common.o
	$(CC) $(CFLAGS) $(LFLAGS) client.c shared.o internal.o common.o -o client $(LIBS)

train: ./libs/train.c plugin_server.h plugin_client.h shared.o
	$(CC) $(CFLAGS) $(LIBFLAGS) ./libs/train.c shared.o -o ./libs/libtrain.so -lncurses

file: ./libs/file.c plugin_server.h plugin_client.h shared.o
	$(CC) $(CFLAGS) $(LIBFLAGS) ./libs/file.c shared.o -o ./libs/libfile.so

shared.o: shared.h shared.c
	$(CC) $(CFLAGS) $(OBJFLAGS) shared.c -o $@

internal.o: internal.h internal.c
	$(CC) $(CFLAGS) $(OBJFLAGS) internal.c -o $@

common.o: common.h common.c
	$(CC) $(CFLAGS) $(OBJFLAGS) common.c -o $@

clean:
	rm -fr *.o gendis server client ./libs/*.so
