#include "common.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "str_manips.hpp"

#define	YELLOW	"\e[33m"
#define BLUE	"\e[34m"
#define MAGENTA	"\e[35m"
#define CYAN	"\e[36m"
#define RESET	"\e[0m"
#define LINE	"\n______________________________________________________________________\n"
#define ENDLINE	"\n‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n"

void	log(Server *serv, Client *client, str_t err)
{
	char	date[100];
	time_t	now;
	tm		*brk;
	int 	len = 69 - err.length();

	time(&now);
	brk = localtime(&now);
	strftime(date, 99, "%B %d %Y %T", brk);

	if (len < 0)
		len = 0;
	str_t id = (serv == NULL) ? "?" : to_string(serv->id());
	std::cout << LINE << "|[ " << YELLOW << date << RESET << " |\t"\
<< "\t" << BLUE << "Server Id: " << id << "  |\t" << CYAN << "Client fd: " << client->fd() << " ]|\n|"\
<< MAGENTA << err <<  RESET << str_t(len, ' ') << '|' << ENDLINE;
}
