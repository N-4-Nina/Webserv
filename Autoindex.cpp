
# include "Autoindex.hpp"
# include "dirent.h"

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

str_t         Autoindex::getPage(const char *path, str_t const &host, int port) 
{
    
    str_t dirName(path);
    // std::cout << "path " << path << "\n\n\n" ;
    // std::cout << "path " << host << port << "\n\n\n" ;
    DIR *dir = opendir(path);
    str_t page =\
    "<!DOCTYPE html>\n\
    <html>\n\
    <head>\n\
            <title>" + dirName + "</title>\n\
    </head>\n\
    <body>\n\
    <h1>INDEX</h1>\n\
    <p>\n";

    if (dir == NULL) 
{
        std::cerr << "error: could not open [" << path << "]" << std::endl;
        return "";
    }
    if (dirName[0] != '/')
        dirName = "/" + dirName;
    for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) 
    {
        page += Autoindex::getLink(str_t(dirEntry->d_name), dirName, host, port);
    }
    page +="\
    </p>\n\
    </body>\n\
    </html>\n";
    closedir(dir);
    return page;
}

str_t         Autoindex::getLink(str_t const &dirEntry, str_t const &dirName, str_t const &host, int port) 
{
    std::stringstream   ss;
    ss << "\t\t<p><a href=\"http://" + host + ":" <<\
        port << dirName + "/" + dirEntry + "\">" + dirEntry + "</a></p>\n";
    return ss.str();
}
