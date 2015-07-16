#include "../plugin_client.h"
#include "../plugin_server.h"

void
run_client(network_info *con)
{
    int i;

    for (i = 0; i < con->count; ++i) {
        net_load_library(&con->remote_connections[i], "./libs/libfile.so");
    }

    net_write_file(&con->remote_connections[con->count - 1], "huffman.c", "huffman.cc", 1, 0);
}

void
run_server(connection_info *con)
{
    char file_name[STRING_LENGTH] = { 0 };
    net_read_file(con, file_name);
}
