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

#define	DEFAULT_PORT	"4369"
#define	STRING_LENGTH	256
#define	NET_STRING_LENGTH	32

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

/**
 * @brief con_info_init
 * Initialize given connection_info structure to default
 * @param con struct to initialize
 */
void con_info_init(connection_info *con);
/**
 * @brief net_info_init
 * Initialize given network_info structure to default
 * @param con struct to initialize
 */
void net_info_init(network_info *con);

/**
 * @brief net_load_library
 * Should be called on client to load library on server.
 * This function has to be called on client before all writes to server.
 * @param con connection to server
 * @param lib_name name of library to load
 * @return 0 when library was succesfully loaded on server, 1 otherwise
 */
int net_load_library(connection_info *con, char *lib_name);

/**
 * @brief net_write
 * Write data from buff with count size to remote connection con
 * @param con remote host
 * @param buf data to write
 * @param count length of data to write
 * @return return value from write() function
 */
int net_write(connection_info* con, const void *buf, size_t count);

/**
 * @brief net_read
 * Read data to buff in count length from remote connection con
 * @param con remote host
 * @param buf destination of data
 * @param count number of bytes which will be read
 * @return return value from read() function
 */
int net_read(connection_info* con, void *buf, size_t count);

/**
 * @brief net_write_file
 * Writes file to remote host con
 * @param con remote host
 * @param name of file which will be sent to remote host (max 255 chars)
 * @param remote_name name of file on server
 * @param offset offset in file
 * @param length zero if all file from given offset should be sent
 * @return 0 on success, 1 file not found or cannot be opened, 2 for long name, 3 remote file problems, 4 for problems with connection
 */
int net_write_file(connection_info *con, char *name, char *remote_name,
				   uint offset, uint length);

/**
 * @brief net_read_file
 * Reads file from remote host
 * @param con remote host
 * @param name out parameter of name given from remote host (max 255 chars)
 * @return 0 on success, 1 file cannot be opened, 2 problems with connection
 */
int net_read_file(connection_info *con, char *name);

/**
 * @brief read_line
 * From given file descriptor reads until it reaches newline character or bytes count
 * @param fd descriptor from which will be read
 * @param buf destination of read data
 * @param count max length of read data
 * @return number of read data (without \n char) or -1 on error
 */
int read_line(int fd, char *buf, size_t count);

#endif // SHARED_H
