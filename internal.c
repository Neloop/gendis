#include "internal.h"

int
handshake_client(connection_info *con)
{
    return (handshake_client_ext(con, 2000));
}

int
handshake_server(connection_info *con)
{
    return (handshake_server_ext(con, 2000));
}

int
handshake_client_ext(connection_info *con, int timeout)
{
    int poll_ret;
    struct pollfd fds[1];
    srand(time(NULL));
    uint32_t uid_client = htonl(getuid());
    uint32_t uid_server;
    uint32_t nonce_client = htonl(rand());
    uint32_t nonce_server;
    uint32_t key_client = htonl(rand());
    uint32_t key_server;
    int hand;
    unsigned char * hash_client;
    unsigned char hash_server[HASH_LENGTH];
    unsigned char hand_server;
    int result = 0;

    if (fcntl(con->fdsock, F_SETFL,
              fcntl(con->fdsock, F_GETFL, NULL) | O_NONBLOCK) == -1) {
        return (1);
    } // non-blocking socket

    if (net_write(con, &uid_client, sizeof (uint32_t)) == -1) {
        // write my uid to server
        if (errno == EINPROGRESS) {
            fds[0].fd = con->fdsock;
            fds[0].events = POLLOUT;

            if ((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1) {
                return (1);
            }
        } else {
            return (1);
        }
    }
    if (net_write(con, &nonce_client, sizeof (uint32_t)) == -1) {
        // write my nonce to server
        if (errno == EINPROGRESS) {
            fds[0].fd = con->fdsock;
            fds[0].events = POLLOUT;

            if ((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1) {
                return (1);
            }
        } else {
            return (1);
        }
    }

    while (net_read(con, &uid_server, sizeof (uint32_t)) == -1) {
        // read uid from server
        if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK) {
            fds[0].fd = con->fdsock;
            fds[0].events = POLLIN;

            if ((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1) {
                return (1);
            }
        } else {
            return (1);
        }
    }
    while (net_read(con, &nonce_server, sizeof (uint32_t)) == -1) {
        // read nonce from server
        if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK) {
            fds[0].fd = con->fdsock;
            fds[0].events = POLLIN;

            if ((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1) {
                return (1);
            }
        } else {
            return (1);
        }
    }

    if (fcntl(con->fdsock, F_SETFL,
              fcntl(con->fdsock, F_GETFL, NULL) & ~O_NONBLOCK) == -1) {
        return (1);
    } // blocking socket again

    while (result == 0) {
        hand = rand() % 3 + 1;

        if (net_read(con, &hash_server, HASH_LENGTH) == -1) {
            return (1);
        } // read server's hash

        hash_client = sign(ntohl(uid_client), ntohl(uid_server),
            ntohl(nonce_client), ntohl(nonce_server),
            ntohl(key_client), (char)hand); // crypt my hand

        if (net_write(con, hash_client, HASH_LENGTH) == -1) {
            return (1);
        } // write my hash to server

        if (net_read(con, &key_server, sizeof (int)) == -1) {
            return (1);
        } // read key from server

        if (net_write(con, &key_client, sizeof (int)) == -1) {
            return (1);
        } // write my key to server

        hand_server = verify(ntohl(uid_client), ntohl(uid_server),
            ntohl(nonce_client), ntohl(nonce_server),
            ntohl(key_server), hash_server); // get server's hand

        result = evaluate(hand, hand_server);
    }

    return (0);
}

int
handshake_server_ext(connection_info *con, int timeout)
{
    int poll_ret;
    struct pollfd fds[1];
    srand(time(NULL));
    uint32_t uid_client, uid_server = htonl(getuid()), nonce_client,
        nonce_server = htonl(rand()), key_client, key_server = htonl(rand());
    int hand = rand() % 3 + 1, result = 0;
    unsigned char * hash_server;
    unsigned char hash_client[HASH_LENGTH];
    unsigned char hand_client;

    srand(time(NULL));

    if (fcntl(con->fdsock, F_SETFL,
              fcntl(con->fdsock, F_GETFL, NULL) | O_NONBLOCK) == -1) {
        return (1);
    } // non-blocking socket

    while (net_read(con, &uid_client, sizeof (uint32_t)) == -1) {
        if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK) {
            fds[0].fd = con->fdsock;
            fds[0].events = POLLIN;

            if ((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1) {
                return (1);
            }
        } else {
            return (1);
        }
    }
    while (net_read(con, &nonce_client, sizeof (uint32_t)) == -1) {
        if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK) {
            fds[0].fd = con->fdsock;
            fds[0].events = POLLIN;

            if ((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1) {
                return (1);
            }
        } else {
            return (1);
        }
    }

    if (net_write(con, &uid_server, sizeof (uint32_t)) == -1) {
        if (errno == EINPROGRESS) {
            fds[0].fd = con->fdsock;
            fds[0].events = POLLOUT;

            if ((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1) {
                return (1);
            }
        } else {
            return (1);
        }
    }
    if (net_write(con, &nonce_server, sizeof (uint32_t)) == -1) {
        if (errno == EINPROGRESS) {
            fds[0].fd = con->fdsock;
            fds[0].events = POLLOUT;

            if ((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1) {
                return (1);
            }
        } else {
            return (1);
        }
    }

    if (fcntl(con->fdsock, F_SETFL,
              fcntl(con->fdsock, F_GETFL, NULL) & ~O_NONBLOCK) == -1) {
        return (1);
    } // blocking socket again

    while (result == 0) {
        hand = rand() % 3 + 1;

        hash_server = sign(ntohl(uid_client), ntohl(uid_server),
            ntohl(nonce_client), ntohl(nonce_server),
            ntohl(key_server), (char)hand);

        if (net_write(con, hash_server, HASH_LENGTH) == -1) {
            return (1);
        }

        if (net_read(con, &hash_client, HASH_LENGTH) == -1) {
            return (1);
        }

        if (net_write(con, &key_server, sizeof (uint32_t)) == -1) {
            return (1);
        }

        if (net_read(con, &key_client, sizeof (uint32_t)) == -1) {
            return (1);
        }

        hand_client = verify(ntohl(uid_client), ntohl(uid_server),
            ntohl(nonce_client), ntohl(nonce_server),
            ntohl(key_client), hash_client);

        result = evaluate(hand_client, hand);
    }

    return (0);
}

void
*load_library(char *name)
{
    void *handle;

    handle = dlopen(name, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return (NULL);
    }

    return (handle);
}

void
*load_symbol(void *lib, char *name)
{
    void *symbol;
    char *error;

    symbol = dlsym(lib, name);
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        return (NULL);
    }

    return (symbol);
}

int
close_library(void *lib)
{
    if (lib == NULL) {
        return (1);
    } else {
        return (dlclose(lib));
    }
}
