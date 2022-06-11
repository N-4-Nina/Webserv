#include "common.hpp"
#include "utils.hpp"

int	unlock_socket(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		fatal("unlock socket failed");
	return (0);
}
