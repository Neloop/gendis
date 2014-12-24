#include "shared.h"

int net_write(connection_info *con, const void *buf, size_t count)
{
    return write(con->fdsock, buf, count);
}

int net_read(connection_info *con, void *buf, size_t count)
{
    return read(con->fdsock, buf, count);
}

int net_load_library(connection_info *con, char *lib_name)
{
    int ret;

    net_write(con, lib_name, strlen(lib_name));

    net_read(con, &ret, sizeof(int));
    ret = ntohl(ret);

    return 0;
}
