#include <stdio.h>
#include <getopt.h>

#include "internal.h"
#include "plugin_server.h"

char port[STRING_LENGTH] = { 0 };

static void help(char *name)
{
    fprintf(stderr, "Usage: %s [-h, --help] [-p, --port port]\n", name);
    exit(1);
}

static void options(int argc, char ** argv)
{
    int opt;
    int option_index;

    struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"port", required_argument, NULL, 'p'},
        {0,0,0,0}
    };

    while(1)
    {
        opt = getopt_long(argc, argv, "hp:", long_options, &option_index);
        if(opt == -1){ break; }

        switch(opt)
        {
            case 0:
                break;
            case 'p':
                strcpy(port, optarg);
                break;
            default:
                help(argv[0]);
                break;
        }
    }

    return;
}

int main(int argc, char ** argv)
{
    connection_info client;
    int sock, pid;
    struct addrinfo *res, *resorig, hint;
    strcpy(port, DEFAULT_PORT);

    options(argc, argv);

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &hint, &resorig);
    for(res = resorig; res != NULL; res = res->ai_next)
    {
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(bind(sock, res->ai_addr, res->ai_addrlen) == 0)
        { break; }
    }
    freeaddrinfo(resorig);

    listen(sock, SOMAXCONN);

    while(1)
    {
        char numhost[NI_MAXHOST];
        socklen_t sz;

        printf("Waiting for connections...\n");

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
                    printf("%s: Handshake accomplished.\n", numhost);
                }
                else
                {
                    fprintf(stderr, "%s: Client did not accomplish handshake!\n", numhost);
                    goto cleanup;
                }

                /*************************************/
                /* do all the work which client asks */
                /*************************************/

                while(1)
                {
                    int net_zero = htonl(0);
                    int ret_lib_load;

                    printf("%s: Waiting for job...\n", numhost);

                    // load library and symbol
                    if((ret_lib_load = net_read(&client, &lib_name, STRING_LENGTH)) == -1)
                    { err(1, NULL); }
                    else if(ret_lib_load == 0){ fprintf(stderr, "%s: Connection closed by remote machine.\n", numhost); goto cleanup; }

                    lib_handle = load_library(lib_name);

                    symbol_run = load_symbol(lib_handle, "run_server");

                    net_write(&client, &net_zero, sizeof(int));

                    // run symbol
                    if(symbol_run != NULL){ symbol_run(&client); }
                    else{ printf("%s: Repeating loading library...\n", numhost); }
                }

                close(client.fdsock);

                printf("%s: Connection exited succesfuly.\n", numhost);

                exit(0);
            default: // parrent process
                break;
        }
    }

    printf("Waiting for all children processes.\n");

    while (wait(NULL) > 0); // wait for all children

    close(sock);

    return 0;

cleanup:
    close(sock);
    close(client.fdsock);
    exit(1);
}
