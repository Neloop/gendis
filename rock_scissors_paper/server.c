#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<poll.h>
#include<time.h>
#include<unistd.h>

#include "common.h"

#define PORT "4242"
#define HASH_LENGTH 32

int main(int agrc, char ** argv)
{
	srand(time(NULL));
	struct pollfd fds[1];
	struct sockaddr_storage client_addr;
	socklen_t sz;
	int sock, newsock, pid;
	struct addrinfo *res, *resorig, hint;

	uint32_t uid_client, uid_server = htonl(getuid()), nonce_client,
		nonce_server = htonl(rand()), key_client, key_server = htonl(rand());
	int hand = rand() % 3 + 1, result = 0;
	unsigned char* hash_server;
	unsigned char hash_client[HASH_LENGTH];
	unsigned char hand_client;

	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PORT, &hint, &resorig);
	for(res = resorig; res != NULL; res = res->ai_next)
	{
		sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(bind(sock, res->ai_addr, res->ai_addrlen) == 0)
		{ break; }
	}
	freeaddrinfo(resorig);

	listen(sock, SOMAXCONN);

	fds[0].fd = sock;
	fds[0].events = POLLIN | POLLOUT;

	//while(poll(fds, 1, 10000) > 0)
	while(1)
	{
		char numhost[NI_MAXHOST];

		sz = sizeof(client_addr);
		newsock = accept(sock, (struct sockaddr *)&client_addr, &sz);
		
		getnameinfo((struct sockaddr *)&client_addr, sz, numhost, sizeof(numhost), NULL, 0, NI_NUMERICHOST);
		write(1, "Connection from ", 16);
		write(1, numhost, strlen(numhost));
		write(1, "\n", 1);

		switch(pid = fork())
		{
			case -1: // error in forking
				exit(1);
			case 0: // children process
				srand(time(NULL));
				
				read(newsock, &uid_client, sizeof(uint32_t));
				read(newsock, &nonce_client, sizeof(uint32_t));

				write(newsock, &uid_server, sizeof(uint32_t));
				write(newsock, &nonce_server, sizeof(uint32_t));

				while(result == 0)
				{
					hand = rand() % 3 + 1;

					hash_server = sign(ntohl(uid_client), ntohl(uid_server),
						ntohl(nonce_client), ntohl(nonce_server),
						ntohl(key_server), (char)hand);

					write(newsock, hash_server, HASH_LENGTH);

					read(newsock, &hash_client, HASH_LENGTH);

					write(newsock, &key_server, sizeof(uint32_t));

					read(newsock, &key_client, sizeof(uint32_t));

					hand_client = verify(ntohl(uid_client), ntohl(uid_server),
						ntohl(nonce_client), ntohl(nonce_server),
						ntohl(key_client), hash_client);
					
					write(1, "client: ", 8);
					write(1, hand2str(hand_client), strlen(hand2str(hand_client)));
					write(1, ", server: ", 10);
					write(1, hand2str(hand), strlen(hand2str(hand)));
					
					switch (result = evaluate(hand_client, hand))
					{
						case 0:
							write(1, "\nIt's a draw!", 14);
							break;
						case 1:
							write(1, "\nClient wins!\n", 14);
							break;
						case 2:
							write(1, "\nServer wins!\n", 14);
							break;
						default:
							write(1, "\nError\n", 6);
							break;
					}
					write(1, "\n", 1);
				}

				exit(0);
			default: // parent process
				break;
		}
	}

	close(sock);

	return 0;
}
