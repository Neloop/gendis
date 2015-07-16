#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <err.h>
#include <sys/wait.h>
#include<fcntl.h>

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

void con_info_init(connection_info *con);
void net_info_init(network_info *con);

int net_load_library(connection_info *con, char *lib_name);

int net_write(connection_info* con, const void *buf, size_t count);

int net_read(connection_info* con, void *buf, size_t count);

/**
 * @brief net_write_file
 * @param con
 * @param name of file which will be sent to remote host (max 255 chars)
 * @param remote_name
 * @param offset
 * @param length zero if all file from given offset should be sent
 * @return 0 on success, 1 file not found or cannot be opened, 2 for long name, 3 remote file problems, 4 for problems with connection
 */
int net_write_file(connection_info *con, char *name, char *remote_name, uint offset, uint length);

/**
 * @brief net_read_file
 * @param con
 * @param name - out parameter of name given from remote host (max 255 chars)
 * @return 0 on success, 1 file cannot be opened, 2 problems with connection
 */
int net_read_file(connection_info *con, char *name);

#endif // SHARED_H
