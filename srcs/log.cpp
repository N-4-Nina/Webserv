#include "common.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "str_manips.hpp"

#define	YELLOW	"\e[33m"
#define BLUE	"\e[34m"
#define MAGENTA	"\e[35m"
#define CYAN	"\e[36m"
#define RESET	"\e[0m"
#define LINE	"\n_____________________________________________________\n"
#define ENDLINE	"\n‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n"

void    log(Server *serv, Client *client, str_t err)
{
	int len = 53 - err.length();
	if (len < 0)
		len = 0;
	std::cout << LINE << "|[ " << YELLOW << to_string(time_in_ms()) << " ms  " << RESET << "|"\
<< "\t" << BLUE << "Server Id:" << serv->id() << "  |\t" << CYAN << "Client fd: " << client->fd() << " ]|\n|"\
<< MAGENTA << err <<  RESET << str_t(len, ' ') << '|' << ENDLINE;
	
}