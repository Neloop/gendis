#include <stdio.h>

#include "internal.h"
#include "plugin_server.h"

int main(int argc, char ** argv)
{
    connection_info client;
    int sock, pid;
    struct addrinfo *res, *resorig, hint;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, DEFAULT_PORT, &hint, &resorig);
    for(res = resorig; res != NULL; res = res->ai_next)
    {
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(bind(sock, res->ai_addr, res->ai_addrlen) == 0)
        { break; }
    }
    freeaddrinfo(resorig);

    listen(sock, SOMAXCONN);

    printf("Waiting for connections...\n");

    while(1)
    {
        char numhost[NI_MAXHOST];
        socklen_t sz;

        sz = sizeof(client.remote_addr);
        client.fdsock = accept(sock, (struct sockaddr *)&client.remote_addr, &sz);

        getnameinfo((struct sockaddr *)&client.remote_addr, sz, numhost, sizeof(numhost), NULL, 0, NI_NUMERICHOST);
        write(1, "Connection from ", 16);
        write(1, numhost, strlen(numhost));
        write(1, "\n", 1);

        strcpy(client.name, numhost);

        char lib_name[STRING_LENGTH];
        void *lib_handle;
        void (*symbol_run)(connection_info*);

        /* Forking is just fine for server,
         * because server do not have to share infos
         * with other processes */
        switch(pid = fork())
        {
            case -1: // error in forking
                err(1, NULL);
            case 0: // children process

                if(handshake(&client) == 0)
                {
                    printf("Handshake accomplished.\n");
                }
                else
                {
                    fprintf(stderr, "Client did not accomplish handshake!\nExiting...\n");
                    goto cleanup;
                }

                printf("Hello from connected forked children\n");

                /*************************************/
                /* do all the work which client asks */
                /*************************************/

                while(1)
                {
                    int net_zero = htonl(0);
                    int ret;
                    // load library and symbol
                    if((ret = net_read(&client, &lib_name, STRING_LENGTH)) == -1)
                    { err(1, NULL); }
                    else if(ret == 0){ fprintf(stderr, "Connection closed by remote machine.\n"); goto cleanup; }

                    lib_handle = load_library(lib_name);

                    symbol_run = load_symbol(lib_handle, "run_server");

                    net_write(&client, &net_zero, sizeof(int));

                    // run symbol
                    if(symbol_run != NULL){ symbol_run(&client); }
                    else{ printf("Somewhere there was error\nRepeating procedure...\n"); continue; }
                    break;
                }

                close(client.fdsock);

                exit(0);
            default: // parrent process
                break;
        }
    }

    while (wait(NULL) > 0); // wait for all children

    close(sock);

    return 0;

cleanup:
    close(sock);
    close(client.fdsock);
    exit(1);
}
