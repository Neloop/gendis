CC = gcc
CFLAGS = -Wall
OBJFLAGS = -fPIC -c
LIBFLAGS = -fPIC -shared

gendis: main.c huffman.h huffman.c
	$(CC) $(CFLAGS) main.c huffman.c -o gendis

clean:
	rm -fr gendis
