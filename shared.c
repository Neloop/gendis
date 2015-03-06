#include "shared.h"

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

    net_write(con, &cont, NET_STRING_LENGTH);

    net_write(con, lib_name, strlen(lib_name));

    net_read(con, &ret, NET_STRING_LENGTH);

    return (0);
}

int
net_write_file(connection_info *con, char *name)
{
    return 0;
}

int
net_read_file(connection_info *con, char *name)
{
    return 0;
}
