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
 * @brief handshake
 * @param con
 * @return 0 if handshake was succesful
 */
int handshake_client_ext(connection_info *con, int timeout);
/**
 * @brief handshake_server
 * @param con
 * @return 0 if handshake was succesful
 */
int handshake_server_ext(connection_info *con, int timeout);

/**
 * @brief handshake
 * @param con
 * @return 0 if handshake was succesful
 */
int handshake_client(connection_info *con);
/**
 * @brief handshake_server
 * @param con
 * @return 0 if handshake was succesful
 */
int handshake_server(connection_info *con);

/**
 * @brief load_library dynamicly load *.so lib using libdl
 * If there was error during opening it writes error message to stderr
 * @param name of the library ready to load
 * @return NULL if there was error during opening library, otherwise returns pointer to lib handle
 */
void* load_library(char *name);

void* load_symbol(void *lib, char *name);

int close_library(void *lib);

int read_line(int fd, char *buf, size_t count);

#endif // INTERNAL_H
