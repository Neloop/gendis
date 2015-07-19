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
        {0, 0, 0, 0}
    };

    while (1) {
        opt = getopt_long(argc, argv, "hp:", long_options, &option_index);
        if (opt == -1) { break; }

        switch (opt) {
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
}

int
main(int argc, char ** argv)
{
    connection_info client;
    int sock, pid, gai_error_code;
    struct addrinfo *res, *resorig, hint;
    char numhost[NI_MAXHOST] = { 0 };
    socklen_t sz;
    int optval = 1;

    /* INITIALIZERS */

    con_info_init(&client);
    strcpy(port, DEFAULT_PORT); // loading of recommended default port

    /* PROGRAM */

    options(argc, argv);

    memset(&hint, 0, sizeof (hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;

    if ((gai_error_code = getaddrinfo(NULL, port, &hint, &resorig)) != 0) {
        printf("server: %s\n", gai_strerror(gai_error_code));
        exit(1);

    }
    for (res = resorig; res != NULL; res = res->ai_next) {
        if ((sock = socket(res->ai_family, res->ai_socktype,
                          res->ai_protocol)) == -1) {
            freeaddrinfo(resorig);
            err(1, "socket");
        }

        if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval,
                        sizeof (optval)) == -1)) {
            freeaddrinfo(resorig);
            close(sock);
            err(1, "setsockopt");
        }

        if (bind(sock, res->ai_addr, res->ai_addrlen) == 0) {
            break;
        } else {
            freeaddrinfo(resorig);
            close(sock);
            err(1, "bind");
        }

        if (res->ai_next == NULL) {
            printf("Creation of socket and binding failed. Exiting...\n");
            freeaddrinfo(resorig);
            exit(1);
        }
    }
    freeaddrinfo(resorig);

    listen(sock, SOMAXCONN);

    while (1) {
        printf("Waiting for connections...\n");

        sz = sizeof (client.remote_addr);
        client.fdsock = accept(sock, (struct sockaddr *)&client.remote_addr,
                               &sz);

        getnameinfo((struct sockaddr *)&client.remote_addr, sz, numhost,
                    sizeof (numhost), NULL, 0, NI_NUMERICHOST);
        printf("Connection from %s\n", numhost);

        strcpy(client.name, numhost);

        char lib_name[STRING_LENGTH] = { 0 };
        void *lib_handle;
        void (*symbol_run)(connection_info*);

        /*
         * Forking is just fine for server,
         * because server do not have to share infos
         * with other processes
         */
        switch (pid = fork()) {
            case -1: // error in forking
                err(1, NULL);
            case 0: // children process

                if (handshake_server(&client) == 0) {
                    printf("%s: Handshake accomplished.\n", numhost);
                } else {
                    fprintf(stderr,
                            "%s: Client did not accomplish handshake!\n",
                            numhost);
                    goto cleanup;
                }

                /*************************************/
                /* do all the work which client asks */
                /*************************************/

                while (1) {
                    char net_ok[NET_STRING_LENGTH] =
                            "net_load_lib:succesfuly_loaded";
                    char net_err[NET_STRING_LENGTH] =
                            "net_load_lib:error";
                    int ret_lib_load;
                    char string_exit[NET_STRING_LENGTH] = { 0 };

                    printf("%s: Waiting for job...\n", numhost);

                    net_read(&client, &string_exit, NET_STRING_LENGTH);
                    if (strcmp(string_exit, "net_load_lib:exit") == 0) {
                        break; }

                    // load library
                    if ((ret_lib_load = net_read(&client, &lib_name,
                                                 STRING_LENGTH)) == -1) {
                        net_write(&client, net_err, NET_STRING_LENGTH);
                        continue;
                    }
                    else if (ret_lib_load == 0) {
                        fprintf(stderr,
                                "%s: Connection closed by remote machine.\n",
                                numhost);
                        goto cleanup;
                    }

                    lib_handle = load_library(lib_name);

                    symbol_run = load_symbol(lib_handle, "run_server");

                    net_write(&client, net_ok, NET_STRING_LENGTH);

                    // run symbol
                    if (symbol_run != NULL) { symbol_run(&client); }
                    else {
                        printf("%s: Repeating loading library...\n", numhost);
                    }

                    close_library(lib_handle);
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

    return (0);

cleanup:
    close(sock);
    close(client.fdsock);
    exit(1);
}
