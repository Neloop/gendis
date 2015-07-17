#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdio.h>
#include <dlfcn.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<time.h>
#include<fcntl.h>
#include<poll.h>
#include<errno.h>
#include<libgen.h>

#include "shared.h"
#include "common.h"

#define HASH_LENGTH 32


/**
 * @brief handshake rock-scissors-paper
 * @param con
 * @return 0 if handshake was succesful
 */
int handshake_client_ext(connection_info *con, int timeout);
/**
 * @brief handshake_server rock-scissors-paper
 * @param con
 * @return 0 if handshake was succesful
 */
int handshake_server_ext(connection_info *con, int timeout);

/**
 * @brief handshake rock-scissors-paper, with default 2s timeout
 * @param con
 * @return 0 if handshake was succesful
 */
int handshake_client(connection_info *con);
/**
 * @brief handshake_server rock-scissors-paper, with default 2s timeout
 * @param con
 * @return 0 if handshake was succesful
 */
int handshake_server(connection_info *con);

/**
 * @brief load_library dynamicaly load *.so lib using libdl
 * If there was error during opening it writes error message to stderr
 * @param name of the library ready to load
 * @return NULL if there was error during opening library, otherwise returns pointer to lib handle
 */
void* load_library(char *name);

/**
 * @brief load_symbol load symbol from given lib using libdl
 * If there was error during opening it writes error message to stderr
 * @param lib handle of loaded library
 * @param name name of symbol to load
 * @return pointer to loaded symbol
 */
void* load_symbol(void *lib, char *name);

/**
 * @brief close_library close given library handle
 * @param lib
 * @return
 */
int close_library(void *lib);

/**
 * @brief read_line from given file descriptor reads until it reaches newline character or bytes count
 * @param fd
 * @param buf
 * @param count
 * @return
 */
int read_line(int fd, char *buf, size_t count);

#endif // INTERNAL_H
