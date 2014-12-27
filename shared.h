#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <err.h>
#include <sys/wait.h>

#define DEFAULT_PORT "4369"
#define STRING_LENGTH 256
#define NET_STRING_LENGTH 32

struct connection_info
{
    char name[STRING_LENGTH];
    int fdsock;
    struct sockaddr_storage remote_addr;
};

struct network_info
{
    int count;
    struct connection_info remote_connections[SOMAXCONN];
};

typedef struct connection_info connection_info;
typedef struct network_info network_info;

int net_load_library(connection_info *con, char *lib_name);

int net_write(connection_info* con, const void *buf, size_t count);

int net_read(connection_info* con, void *buf, size_t count);

#endif // SHARED_H
