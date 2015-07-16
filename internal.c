#include "internal.h"

int
handshake_client(connection_info *con)
{
    return handshake_client_ext(con, 2000);
}

int
handshake_server(connection_info *con)
{
    return handshake_server_ext(con, 2000);
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
    unsigned char* hash_client;
    unsigned char hash_server[HASH_LENGTH];
    unsigned char hand_server;
    int result = 0;

    if(fcntl(con->fdsock, F_SETFL, fcntl(con->fdsock, F_GETFL, NULL) | O_NONBLOCK) == -1){
        return (1);
    } // non-blocking socket

    if(write(con->fdsock, &uid_client, sizeof(uint32_t)) == -1){ // write my uid to server
        if(errno == EINPROGRESS){
            fds[0].fd = con->fdsock;
            fds[0].events = POLLOUT;

            if((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1){
                return (1);
            }
        }
        else{
            return (1);
        }
    }
    if(write(con->fdsock, &nonce_client, sizeof(uint32_t)) == -1){ // write my nonce to server
        if(errno == EINPROGRESS){
            fds[0].fd = con->fdsock;
            fds[0].events = POLLOUT;

            if((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1){
                return (1);
            }
        }
        else{
            return (1);
        }
    }

    while(read(con->fdsock, &uid_server, sizeof(uint32_t)) == -1){ // read uid from server
        if(errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK){
            fds[0].fd = con->fdsock;
            fds[0].events = POLLIN;

            if((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1){
                return (1);
            }
        }
        else{
            return (1);
        }
    }
    while(read(con->fdsock, &nonce_server, sizeof(uint32_t)) == -1){ // read nonce from server
        if(errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK){
            fds[0].fd = con->fdsock;
            fds[0].events = POLLIN;

            if((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1){
                return (1);
            }
        }
        else{
            return (1);
        }
    }

    if(fcntl(con->fdsock, F_SETFL, fcntl(con->fdsock, F_GETFL, NULL) & ~O_NONBLOCK) == -1){
        return (1);
    } // blocking socket again

    while(result == 0)
    {
        hand = rand() % 3 + 1;

        if(read(con->fdsock, &hash_server, HASH_LENGTH) == -1){ // read server's hash
            return (1);
        }

        hash_client = sign(ntohl(uid_client), ntohl(uid_server),
            ntohl(nonce_client), ntohl(nonce_server),
            ntohl(key_client), (char)hand); // crypt my hand

        if(write(con->fdsock, hash_client, HASH_LENGTH) == -1){ // write my hash to server
            return (1);
        }

        if(read(con->fdsock, &key_server, sizeof(int)) == -1){ // read key from server
            return (1);
        }

        if(write(con->fdsock, &key_client, sizeof(int)) == -1){ // write my key to server
            return (1);
        }

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
    unsigned char* hash_server;
    unsigned char hash_client[HASH_LENGTH];
    unsigned char hand_client;

    srand(time(NULL));

    if(fcntl(con->fdsock, F_SETFL, fcntl(con->fdsock, F_GETFL, NULL) | O_NONBLOCK) == -1){
        return (1);
    } // non-blocking socket

    while(read(con->fdsock, &uid_client, sizeof(uint32_t)) == -1){
        if(errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK){
            fds[0].fd = con->fdsock;
            fds[0].events = POLLIN;

            if((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1){
                return (1);
            }
        }
        else{
            return (1);
        }
    }
    while(read(con->fdsock, &nonce_client, sizeof(uint32_t)) == -1){
        if(errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK){
            fds[0].fd = con->fdsock;
            fds[0].events = POLLIN;

            if((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1){
                return (1);
            }
        }
        else{
            return (1);
        }
    }

    if(write(con->fdsock, &uid_server, sizeof(uint32_t)) == -1){
        if(errno == EINPROGRESS){
            fds[0].fd = con->fdsock;
            fds[0].events = POLLOUT;

            if((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1){
                return (1);
            }
        }
        else{
            return (1);
        }
    }
    if(write(con->fdsock, &nonce_server, sizeof(uint32_t)) == -1){
        if(errno == EINPROGRESS){
            fds[0].fd = con->fdsock;
            fds[0].events = POLLOUT;

            if((poll_ret = poll(fds, 1, timeout)) == 0 || poll_ret == -1){
                return (1);
            }
        }
        else{
            return (1);
        }
    }

    if(fcntl(con->fdsock, F_SETFL, fcntl(con->fdsock, F_GETFL, NULL) & ~O_NONBLOCK) == -1){
        return (1);
    } // blocking socket again

    while(result == 0)
    {
        hand = rand() % 3 + 1;

        hash_server = sign(ntohl(uid_client), ntohl(uid_server),
            ntohl(nonce_client), ntohl(nonce_server),
            ntohl(key_server), (char)hand);

        if(write(con->fdsock, hash_server, HASH_LENGTH) == -1){
            return (1);
        }

        if(read(con->fdsock, &hash_client, HASH_LENGTH) == -1){
            return (1);
        }

        if(write(con->fdsock, &key_server, sizeof(uint32_t)) == -1){
            return (1);
        }

        if(read(con->fdsock, &key_client, sizeof(uint32_t)) == -1){
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

int
read_line(int fd, char *buf, size_t count)
{
    int ret = read(fd, buf, count);

    if (ret <= 0) {
        return (ret);
    }

    if (buf[ret - 1] == '\n') { buf[ret - 1] = 0; }

    return (--ret);
}
