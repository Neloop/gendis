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
	char hello[NET_STRING_LENGTH] = "gendis:hello";
	char hello_ret[NET_STRING_LENGTH] = { 0 };
	char hello_ret_def[NET_STRING_LENGTH] = "gendis:ehllo";
	char ver[NET_STRING_LENGTH] = "gendis:v1.0";
	char ver_ret[NET_STRING_LENGTH] = { 0 };
	char ver_ret_def[NET_STRING_LENGTH] = "gendis:ok";


	if (fcntl(con->fdsock, F_SETFL,
			fcntl(con->fdsock, F_GETFL, NULL) | O_NONBLOCK) == -1) {
		return (1);
	} // non-blocking socket


	if (net_write(con, hello, NET_STRING_LENGTH) == -1) {
		if (errno == EINPROGRESS) {
			fds[0].fd = con->fdsock;
			fds[0].events = POLLOUT;

			if ((poll_ret = poll(fds, 1, timeout)) == 0 ||
					poll_ret == -1) {
				return (1);
			}
		} else {
			return (1);
		}
	}

	while (net_read(con, hello_ret, NET_STRING_LENGTH) == -1) {
		if (errno == EINPROGRESS || errno == EAGAIN ||
				errno == EWOULDBLOCK) {
			fds[0].fd = con->fdsock;
			fds[0].events = POLLIN;

			if ((poll_ret = poll(fds, 1, timeout)) == 0 ||
					poll_ret == -1) {
				return (1);
			}
		} else {
			return (1);
		}
	}

	if (strncmp(hello_ret, hello_ret_def, NET_STRING_LENGTH) != 0) {
		return (1);
	}

	if (net_write(con, ver, NET_STRING_LENGTH) == -1) {
		if (errno == EINPROGRESS) {
			fds[0].fd = con->fdsock;
			fds[0].events = POLLOUT;

			if ((poll_ret = poll(fds, 1, timeout)) == 0 ||
					poll_ret == -1) {
				return (1);
			}
		} else {
			return (1);
		}
	}

	while (net_read(con, ver_ret, NET_STRING_LENGTH) == -1) {
		if (errno == EINPROGRESS || errno == EAGAIN ||
				errno == EWOULDBLOCK) {
			fds[0].fd = con->fdsock;
			fds[0].events = POLLIN;

			if ((poll_ret = poll(fds, 1, timeout)) == 0 ||
					poll_ret == -1) {
				return (1);
			}
		} else {
			return (1);
		}
	}

	if (strncmp(ver_ret, ver_ret_def, NET_STRING_LENGTH) != 0) {
		return (1);
	}


	if (fcntl(con->fdsock, F_SETFL,
			fcntl(con->fdsock, F_GETFL, NULL) &
				~O_NONBLOCK) == -1) {
		return (1);
	} // blocking socket again

	return (0);
}

int
handshake_server_ext(connection_info *con, int timeout)
{
	int poll_ret;
	struct pollfd fds[1];
	char hello[NET_STRING_LENGTH] = { 0 };
	char hello_def[NET_STRING_LENGTH] = "gendis:hello";
	char hello_ret[NET_STRING_LENGTH] = "gendis:ehllo";
	char ver[NET_STRING_LENGTH] = { 0 };
	char ver_def[NET_STRING_LENGTH] = "gendis:v1.0";
	char ver_ret[NET_STRING_LENGTH] = "gendis:ok";
	char gerr[NET_STRING_LENGTH] = "gendis:error";


	if (fcntl(con->fdsock, F_SETFL,
			fcntl(con->fdsock, F_GETFL, NULL) |
				O_NONBLOCK) == -1) {
		return (1);
	} // non-blocking socket


	while (net_read(con, hello, NET_STRING_LENGTH) == -1) {
		if (errno == EINPROGRESS || errno == EAGAIN ||
				errno == EWOULDBLOCK) {
			fds[0].fd = con->fdsock;
			fds[0].events = POLLIN;

			if ((poll_ret = poll(fds, 1, timeout)) == 0 ||
					poll_ret == -1) {
				return (1);
			}
		} else {
			return (1);
		}
	}

	if (strncmp(hello, hello_def, NET_STRING_LENGTH) == 0) {
		if (net_write(con, hello_ret, NET_STRING_LENGTH) == -1) {
			if (errno == EINPROGRESS) {
				fds[0].fd = con->fdsock;
				fds[0].events = POLLOUT;

				if ((poll_ret = poll(fds, 1, timeout)) == 0 ||
						poll_ret == -1) {
					return (1);
				}
			} else {
				return (1);
			}
		}
	} else {
		if (net_write(con, gerr, NET_STRING_LENGTH) == -1) {
			if (errno == EINPROGRESS) {
				fds[0].fd = con->fdsock;
				fds[0].events = POLLOUT;

				if ((poll_ret = poll(fds, 1, timeout)) == 0 ||
						poll_ret == -1) {
					return (1);
				}
			} else {
				return (1);
			}
		}

		return (1);
	}

	while (net_read(con, ver, NET_STRING_LENGTH) == -1) {
		if (errno == EINPROGRESS || errno == EAGAIN ||
				errno == EWOULDBLOCK) {
			fds[0].fd = con->fdsock;
			fds[0].events = POLLIN;

			if ((poll_ret = poll(fds, 1, timeout)) == 0 ||
					poll_ret == -1) {
				return (1);
			}
		} else {
			return (1);
		}
	}

	if (strncmp(ver, ver_def, NET_STRING_LENGTH) == 0) {
		if (net_write(con, ver_ret, NET_STRING_LENGTH) == -1) {
			if (errno == EINPROGRESS) {
				fds[0].fd = con->fdsock;
				fds[0].events = POLLOUT;

				if ((poll_ret = poll(fds, 1, timeout)) == 0 ||
						poll_ret == -1) {
					return (1);
				}
			} else {
				return (1);
			}
		}
	} else {
		if (net_write(con, gerr, NET_STRING_LENGTH) == -1) {
			if (errno == EINPROGRESS) {
				fds[0].fd = con->fdsock;
				fds[0].events = POLLOUT;

				if ((poll_ret = poll(fds, 1, timeout)) == 0 ||
						poll_ret == -1) {
					return (1);
				}
			} else {
				return (1);
			}
		}

		return (1);
	}


	if (fcntl(con->fdsock, F_SETFL,
			fcntl(con->fdsock, F_GETFL, NULL) &
				~O_NONBLOCK) == -1) {
		return (1);
	} // blocking socket again

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
