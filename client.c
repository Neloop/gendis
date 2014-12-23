#include <stdio.h>
#include <string.h>

//#include "plugin_client.h"
#include "internal.h"

#define LIBRARY "plugin.so"

int main(int argc, char ** argv)
{
    void *lib_handle;
    void (*symbol_run)(network_info*);
    int i;
    network_info servers;
    struct addrinfo *res, *resorig, hint;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;

    //while(1)
    {
        char numhost[NI_MAXHOST];
        socklen_t sz = sizeof(servers.remote_connections[servers.count - 1].remote_addr);

        servers.remote_connections[servers.count].name = "localhost";
        servers.count++;

        getaddrinfo(servers.remote_connections[servers.count - 1].name, DEFAULT_PORT, &hint, &resorig);

        for(res = resorig; res != NULL; res = res->ai_next)
        {
            servers.remote_connections[servers.count - 1].fdsock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

            if(connect(servers.remote_connections[servers.count - 1].fdsock, (struct sockaddr *)res->ai_addr, res->ai_addrlen) == 0)
            {
                memcpy(&servers.remote_connections[servers.count - 1].remote_addr, res->ai_addr, sizeof(*res->ai_addr));

                getnameinfo((struct sockaddr *)&servers.remote_connections[servers.count - 1].remote_addr, sz, numhost, sizeof(numhost), NULL, 0, NI_NUMERICHOST);
                write(1, "Connected to ", 13);
                write(1, numhost, strlen(numhost));
                write(1, "\n", 1);

                if(handshake(&servers.remote_connections[servers.count - 1]) == 0)
                {
                    printf("Handshake accomplished.\n");
                }
                else
                {
                    fprintf(stderr, "Server did not accomplish handshake!\n");
                    servers.count--;
                }

                break;
            }
        }

        freeaddrinfo(resorig);
    }

    /* LOADING LIB */

    sleep(10);

    lib_handle = load_library(LIBRARY);
    symbol_run = load_symbol(lib_handle, "run");

    if(symbol_run != NULL){ symbol_run(&servers); }
    else{ printf("Somewhere there was error\nExiting...\n"); }

    close_library(lib_handle);


    /* CLENUP SECTION */

    for(i = 0; i < servers.count; ++i)
    {
        close(servers.remote_connections[i].fdsock);
    }

    return 0;

    /*cleanup:
    freeaddrinfo(resorig);
    for(i = 0; i < servers.count; ++i)
    {
        close(servers.remote_connections[i].fdsock);
    }
    exit(1);*/
}
