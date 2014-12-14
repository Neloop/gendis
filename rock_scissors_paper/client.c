#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<time.h>

#include "common.h"

#define HASH_LENGTH 32 

int main(int argc, char** argv)
{
	srand(time(NULL));
	int fd;
	struct addrinfo *r, *rorig, hi;
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

	memset(&hi, 0, sizeof(hi));
	hi.ai_family = AF_UNSPEC;

	getaddrinfo("u-pl9", "4242", &hi, &rorig);
	
	for(r = rorig; r != NULL; r = r->ai_next)
	{
		fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if(connect(fd, (struct sockaddr *)r->ai_addr, r->ai_addrlen) == 0){ break; }
	}

	freeaddrinfo(rorig);

	write(fd, &uid_client, sizeof(int)); // write my uid to server
	write(fd, &nonce_client, sizeof(int)); // write my nonce to server

	read(fd, &uid_server, sizeof(int)); // read uid from server
	read(fd, &nonce_server, sizeof(int)); // read nonce from server
	
	while(result == 0)
	{
		hand = rand() % 3 + 1;

		read(fd, &hash_server, HASH_LENGTH); // read server's hash

		hash_client = sign(ntohl(uid_client), ntohl(uid_server), 
			ntohl(nonce_client), ntohl(nonce_server), 
			ntohl(key_client), (char)hand); // crypt my hand
	
		write(fd, hash_client, HASH_LENGTH); // write my hash to server
	
		read(fd, &key_server, sizeof(int)); // read key from server

		write(fd, &key_client, sizeof(int)); // write my key to server

		hand_server = verify(ntohl(uid_client), ntohl(uid_server), 
			ntohl(nonce_client), ntohl(nonce_server), 
			ntohl(key_server), hash_server); // get server's hand
	
		/* Write result to standard output  */
		write(1, "client: ", 8);
		write(1, hand2str(hand), strlen(hand2str(hand)));
		write(1, ", server: ", 10);
		write(1, hand2str(hand_server), strlen(hand2str(hand_server)));
		
		switch (result = evaluate(hand, hand_server))
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

	close(fd);

	return 0;
}
