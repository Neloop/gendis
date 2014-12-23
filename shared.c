#include "shared.h"

int net_write(connection_info *con, const void *buf, size_t count)
{
    return write(con->fdsock, buf, count);
}

int net_read(connection_info *con, void *buf, size_t count)
{
    return read(con->fdsock, buf, count);
}
