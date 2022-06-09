#include "Autoindex.hpp"
#include <dirent.h>

/*
					.--------------.
					| Constructors |
					'--------------'
*/

Autoindex::Autoindex(void) 
{
}

/*
		It was important to keep this function to keep the cannonical form
	of Autoindex. However, the programm doesn't use this copy constructor.
	Autoindex doesn't have variable.
*/
Autoindex::Autoindex(Autoindex const &src) 
{
	(void)src;
}


/*
					.------------.
					| Destructor |
					'------------'
*/

Autoindex::~Autoindex(void) 
{
}


/*
					.----------.
					| Operator |
					'----------'
*/

/*
		It was important to keep this function to keep the cannonical form
	of Autoindex. However, the programm doesn't use operator=(). Autoindex
	doesn't have variable.
*/
Autoindex	&Autoindex::operator=(Autoindex const &ref) 
{
	(void)ref;
	return *this;
}


/*
					.------------------.
					| Member functions |
					'------------------'
*/

str_t		 Autoindex::get_page(const char *route, const char *path)
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
			<h1>Index of ";

	if (dirName[0] != '/')
		dirName = "/" + dirName;

	page.append(route);
	page.append("</h1>\n\
	<p>\n");
	
	for (struct dirent *dir_entry = readdir(dir); dir_entry; dir_entry = readdir(dir)) 
	{
		page += get_link(str_t(dir_entry->d_name), route);
	}

	page +="\
		</body>\n\
	</html>\n";
	closedir(dir);
	return (page);
}

str_t		Autoindex::get_link(str_t const &dir_entry, str_t const &route)
{
	std::stringstream   ss;

	if (*route.rbegin() == '/')
		ss << "\t\t<p><a href=\"" + route + dir_entry + "\">" + dir_entry + "</a></p>\n";
	else
		ss << "\t\t<p><a href=\"" << route << "/" + dir_entry + "\">" + dir_entry + "</a></p>\n";
	return ss.str();
}
