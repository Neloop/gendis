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

    net_write(con, &cont, NET_STRING_LENGTH);

    net_write(con, lib_name, strlen(lib_name));

    net_read(con, &ret, NET_STRING_LENGTH);

    return (0);
}
