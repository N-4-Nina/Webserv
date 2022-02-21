#include "common.hpp"

int	unlock_socket(int fd)
{
	int flags;
	flags = fcntl(fd,  F_GETFL);
	//if (flags == -1)
	//	handle_error
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		fatal("unlock socket failed");
	return (0);
}
