#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "internal.h"

#define	LIBRARY	"plugin.so"

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
		opt = getopt_long(argc, argv, "hp:",
			long_options, &option_index);

		if (opt == -1) { break; }

		switch (opt) {
			case 0:
				break;
			case 'p':
				strncpy(port, optarg, STRING_LENGTH);
				break;
			case '?':
			default:
				help(argv[0]);
				break;
		}
	}
}

int
main(int argc, char ** argv)
{
	void *lib_handle;
	void (*symbol_run)(network_info*);
	int i, gai_error_code;
	network_info servers;
	struct addrinfo *res, *resorig, hint;

	/* INITIALIZERS */
	net_info_init(&servers);

	/* PROGRAM */
	options(argc, argv);

	memset(&hint, 0, sizeof (hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;

	while (1) {
		int newsock;
		char *atpos;
		char server_name[STRING_LENGTH] = { 0 };
		char numhost[NI_MAXHOST] = { 0 };
		socklen_t sz =
				sizeof (struct sockaddr_storage);
		strcpy(port, DEFAULT_PORT);

		printf("Enter name (ip/name@port) of remote worker server");
		printf(" or \"done\" to move on: [done]\n");
		read_line(STDIN_FILENO, server_name, STRING_LENGTH);

		atpos = strchr(server_name, '@');
		if (atpos != NULL) {
			strncpy(port, atpos + 1, STRING_LENGTH);
			memset(server_name + (atpos - server_name), 0,
				(server_name + STRING_LENGTH) - atpos);
		}

		if (strcmp(server_name, "done") == 0 ||
				strlen(server_name) == 0) {
			if (servers.count == 0) {
				printf("There are no connections,");
				printf(" try that again:\n");
				continue;
			}
			break;
		}

		if (servers.count >= SOMAXCONN) {
			printf("Too many servers... Forced continue!\n");
			break;
		}

		strncpy(servers.remote_connections[servers.count].name,
				server_name, STRING_LENGTH);

		if ((gai_error_code = getaddrinfo(server_name, port,
				&hint, &resorig)) != 0) {
			printf("client: %s\n", gai_strerror(gai_error_code));
			continue;
		}

		for (res = resorig; res != NULL; res = res->ai_next) {
			connection_info *temp_remote;
			temp_remote =
				&servers.remote_connections[servers.count];
			newsock = socket(res->ai_family,
				res->ai_socktype, res->ai_protocol);
			servers.remote_connections[servers.count].fdsock
					= newsock;

			if (connect(newsock, (struct sockaddr *)res->ai_addr,
						res->ai_addrlen) == 0) {
				memcpy(&temp_remote->remote_addr,
						res->ai_addr,
						sizeof (*res->ai_addr));

				getnameinfo((struct sockaddr *)res->ai_addr,
						sz, numhost,
						sizeof (numhost), NULL,
						0, NI_NUMERICHOST);
				printf("Connected to %s\n", numhost);

				if (handshake_client(temp_remote) == 0) {
					servers.count++;
					printf("%s: Handshake accomplished.\n",
						numhost);
				} else {
					fprintf(stderr,
							"%s: ", numhost);
					fprintf(stderr,
						"Server did not accomplish");
					fprintf(stderr,
						" handshake!\n");
					close(newsock);
				}

				break;
			}

			if (res->ai_next == NULL) {
				printf("%s: ", server_name);
				printf("Connection to server was not found.");
				printf(" Try it again:\n");
			}
		}

		freeaddrinfo(resorig);
	}

	/* LOADING LIB */

	char lib_name[STRING_LENGTH] = { 0 };

	while (1) {
		printf("Enter name of library to load ");
		printf("(in default ./libs/ folder) or ");
		printf("\"exit\" to stop program: [exit]\n");
		read_line(STDIN_FILENO, lib_name, STRING_LENGTH);

		if (strcmp(lib_name, "exit") == 0 || strlen(lib_name) == 0) {
			char ex[NET_STRING_LENGTH] = "net_load_lib:exit";
			for (i = 0; i < servers.count; ++i) {
				net_write(&servers.remote_connections[i],
					&ex, NET_STRING_LENGTH);
			}
			break;
		}

		if (strlen(basename(lib_name)) > (STRING_LENGTH - 14)) {
			printf("Library name too long. Repeating...\n");
			continue;
		}

		char lib_name_tmp[STRING_LENGTH] = { 0 };
		strcpy(lib_name_tmp, "./libs/lib");
		strncat(lib_name_tmp, basename(lib_name),
			strlen(basename(lib_name)));
		strncat(lib_name_tmp, ".so", 3);

		lib_handle = load_library(lib_name_tmp);
		symbol_run = load_symbol(lib_handle, "run_client");

		if (symbol_run != NULL) { symbol_run(&servers); }
		else { printf("Somewhere there was error. Repeating...\n"); }

		close_library(lib_handle);
	}


	/* CLENUP SECTION */

	for (i = 0; i < servers.count; ++i) {
		close(servers.remote_connections[i].fdsock);
	}

	return (0);
}
