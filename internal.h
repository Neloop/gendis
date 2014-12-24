#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdio.h>
#include <dlfcn.h>

#include "shared.h"


/**
 * @brief handshake
 * @param con
 * @return 0 if handshake was succesful
 */
int handshake(connection_info *con);

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
