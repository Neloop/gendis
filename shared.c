#include "shared.h"

void con_info_init(connection_info *con)
{
    memset(con->name, 0, STRING_LENGTH);
    con->fdsock = 0;
    memset(&con->remote_addr, 0, sizeof (con->remote_addr));
}

void net_info_init(network_info *con)
{
    int i;
    con->count = 0;
    
    for(i = 0; i < SOMAXCONN; ++i)
    {
        con_info_init(&con->remote_connections[i]);
    }
}

int
net_write(connection_info *con, const void *buf, size_t count)
{
    return (write(con->fdsock, buf, count));
}

int
net_read(connection_info *con, void *buf, size_t count)
{
    return (read(con->fdsock, buf, count));
}

int
net_load_library(connection_info *con, char *lib_name)
{
    char ret[NET_STRING_LENGTH] = { 0 };
    char cont[NET_STRING_LENGTH] = "net_load_lib:continue";
    char lib_name_temp[STRING_LENGTH] = { 0 };
    strcpy(lib_name_temp, lib_name);

    net_write(con, &cont, NET_STRING_LENGTH);

    net_write(con, lib_name_temp, STRING_LENGTH);

    net_read(con, &ret, NET_STRING_LENGTH);

    return (0);
}

int
net_write_file(connection_info *con, char *name, char *remote_name, uint offset, uint length)
{
    int fd;
    off_t end_of_file;
    uint written_bytes = 0;
    char response_name[NET_STRING_LENGTH] = { 0 };
    char response_length[NET_STRING_LENGTH] = { 0 };
    char response_end[NET_STRING_LENGTH] = { 0 };
    char remote_filename[STRING_LENGTH] = { 0 };

    strcpy(remote_filename, remote_name);

    if(strlen(name) >= 256){ // name of file is too long
        return (2);
    }
    if(strlen(remote_name) >= 256){ // remote name of file is too long
        return (2);
    }

    if((fd = open(name, O_RDONLY)) == -1){ // cannot open given file
        return (1);
    }

    if(length == 0){ // length of file was not specified
        if((end_of_file = lseek(fd, 0, SEEK_END)) == -1){
            close(fd);
            return (1);
        }

        length = end_of_file - offset;
    }

    if(lseek(fd, offset, SEEK_SET) == -1){
        close(fd);
        return (1);
    }

    net_write(con, remote_filename, STRING_LENGTH); // write filename of newly created remote file
    net_read(con, &response_name, NET_STRING_LENGTH); // response to filename

    if (strcmp(response_name, "net_read_file:ok") != 0) {
        close(fd);
        return (3);
    }

    net_write(con, &length, sizeof(uint)); // write file length to remote host
    net_read(con, &response_length, NET_STRING_LENGTH); // response to file length

    if (strcmp(response_length, "net_read_file:ok") != 0) {
        close(fd);
        return (3);
    }


    /* now we can write file to other side */
    while(written_bytes < length){
        int read_bytes;
        char read_data[STRING_LENGTH] = { 0 };

        if((read_bytes = read(fd, read_data, STRING_LENGTH)) == -1){}

        net_write(con, &read_data, read_bytes);
        written_bytes += read_bytes;
    }

    net_read(con, &response_end, NET_STRING_LENGTH);
    if (strcmp(response_end, "net_read_file:ok") != 0) {
        close(fd);
        return (4);
    }

    close(fd);

    return (0);
}

int
net_read_file(connection_info *con, char *name)
{
    int fd;
    uint file_length;
    uint read_bytes = 0;
    char ok_msg[NET_STRING_LENGTH] = "net_read_file:ok";
    char error_msg[NET_STRING_LENGTH] = "net_read_file:error";

    net_read(con, name, STRING_LENGTH);

    if((fd = open(name, O_CREAT | O_WRONLY | O_EXCL, S_IRWXU | S_IRUSR)) == -1){
        net_write(con, &error_msg, NET_STRING_LENGTH); // tell other side about error
        return (1);
    }
    net_write(con, &ok_msg, NET_STRING_LENGTH); // everything is ok so far

    net_read(con, &file_length, sizeof(uint)); // get file length from remote host
    net_write(con, &ok_msg, NET_STRING_LENGTH);


    /* everything is ready to read file from remote host */
    while(read_bytes < file_length){
        uint read_now;
        char read_data[STRING_LENGTH] = { 0 };

        read_now = net_read(con, &read_data, STRING_LENGTH);

        if((write(fd, &read_data, read_now)) == -1){}
        read_bytes += read_now;
    }

    net_write(con, &ok_msg, NET_STRING_LENGTH);

    close(fd);

    return (0);
}
