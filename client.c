#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "internal.h"

#define LIBRARY "plugin.so"

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
    void *lib_handle;
    void (*symbol_run)(network_info*);
    int i;
    network_info servers;
    struct addrinfo *res, *resorig, hint;
    strcpy(port, DEFAULT_PORT);

    options(argc, argv);

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;

    while(1)
    {
        int newsock;
        char server_name[STRING_LENGTH] = { 0 };
        char numhost[NI_MAXHOST] = { 0 };
        socklen_t sz = sizeof(servers.remote_connections[servers.count - 1].remote_addr);

        printf("Enter name of remote worker server or \"done\" to move on: [done]\n");
        read_line(stdin->_fileno, server_name, STRING_LENGTH);

        if(strcmp(server_name, "done") == 0 || strlen(server_name) == 0)
        {
            if(servers.count == 0)
            {
                printf("There are no connections, try that again:\n");
                continue;
            }
            break;
        }

        strcpy(servers.remote_connections[servers.count].name, server_name);
        servers.count++;

        if(getaddrinfo(server_name, port, &hint, &resorig) != 0)
        {
            warn(NULL);
            continue;
        }

        for(res = resorig; res != NULL; res = res->ai_next)
        {
            newsock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
            servers.remote_connections[servers.count - 1].fdsock = newsock;

            if(connect(newsock, (struct sockaddr *)res->ai_addr, res->ai_addrlen) == 0)
            {
                memcpy(&servers.remote_connections[servers.count - 1].remote_addr, res->ai_addr, sizeof(*res->ai_addr));

                getnameinfo((struct sockaddr *)res->ai_addr, sz, numhost, sizeof(numhost), NULL, 0, NI_NUMERICHOST);
                write(1, "Connected to ", 13);
                write(1, numhost, strlen(numhost));
                write(1, "\n", 1);

                if(handshake(&servers.remote_connections[servers.count - 1]) == 0)
                { printf("%s: Handshake accomplished.\n", numhost); }
                else
                {
                    fprintf(stderr, "%s: Server did not accomplish handshake!\n", numhost);
                    servers.count--;
                }

                break;
            }

            if(res->ai_next == NULL){ printf("%s: Connection to server was not found. Try it again:\n", numhost); servers.count--; }
        }

        freeaddrinfo(resorig);
    }

    /* LOADING LIB */

    char lib_name[STRING_LENGTH] = { 0 };

    while(1)
    {
        printf("Enter name of library to load or \"exit\" to stop program: [exit]\n");
        read_line(stdin->_fileno, lib_name, STRING_LENGTH);

        if(strcmp(lib_name, "exit") == 0 || strlen(lib_name) == 0)
        { break; }

        lib_handle = load_library(lib_name);
        symbol_run = load_symbol(lib_handle, "run_client");

        if(symbol_run != NULL){ symbol_run(&servers); }
        else{ printf("Somewhere there was error. Repeating...\n"); }

        close_library(lib_handle);
    }


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
