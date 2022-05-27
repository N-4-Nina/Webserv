
# include "Autoindex.hpp"
# include <dirent.h>

Autoindex::Autoindex(void) 
{}

Autoindex::Autoindex(Autoindex const &src) 
{
    (void)src;
}

Autoindex::~Autoindex(void) 
{}

Autoindex	&Autoindex::operator=(Autoindex const &src) 
{
    (void)src;
    return *this;
}

str_t         Autoindex::getPage(const char *route, const char *path, str_t const &host, int port)
{
    str_t dirName(path);
    DIR *dir = opendir(path);

    if (dir == NULL) 
    {
        std::cerr << "error: could not open [" << path << "]" << std::endl;
        return "";
    }

	str_t page =\
    "<!DOCTYPE html>\n\
    <html>\n\
    <head>\n\
            <title>webserv</title>\n\
    </head>\n\
    <body>\n\
    <h1>INDEX</h1>\n\
    <p>\n";


    if (dirName[0] != '/')
        dirName = "/" + dirName;

    for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) 
    {
        page += getLink(str_t(dirEntry->d_name), host, port, route);
    }

    page +="\
    </body>\n\
    </html>\n";
    closedir(dir);
    return (page);
}

str_t		Autoindex::getLink(str_t const &dirEntry, str_t const &host, int port, str_t const &route)
{
	std::stringstream   ss;
    (void)host;
    (void)port;
    if (*route.rbegin() == '/')
		ss << "\t\t<p><a href=\"" + route + dirEntry + "\">" + dirEntry + "</a></p>\n";
	else
		ss << "\t\t<p><a href=\"" << route << "/" + dirEntry + "\">" + dirEntry + "</a></p>\n";
	std::cout << "ss: " << ss.str() << std::endl;
    return ss.str();
}
