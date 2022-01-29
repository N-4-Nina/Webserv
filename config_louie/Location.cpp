#include "Location.hpp"

/*
*   set_fastcgi_pass(): defines the address of a CGI server
*           a domain name or an IP address, and a port, ex: fastcgi_pass  127.0.0.1:9000;
*   set_fastcgi_param(): array directive that can be used to set parameters to values
*           most often, this is used in conjunction with variables to set FastCGI parameters
*           to values specific to the request
*/

Location::Location(str_t block)
{
    this->_path = block.substr(0, block.find(" "));
    set_autoindex(search_config(block, "autoindex"));
    set_index(search_config(block, "index"));
    set_fastcgi_pass(search_config(block, "fastcgi_pass"));
    set_fastcgi_param(search_config(block, "fastcgi_param"));
}

Location::Location(const Location &ref)
{
    *this = ref;
}

Location &Location::operator=(const Location &ref)
{
    *this = ref;
    return (*this);
}

Location::~Location() {}

void Location::set_autoindex(str_t line)
{
    str_t tmp;

    if (line == "")
    {
        this->_autoindex = "off";
        return ;
    }
    tmp = line.substr(line.find(" ") + 1);
    if (tmp == "on" || tmp == "off")
        this->_autoindex = tmp;
    else
        throw str_t("error: bad arguement for autoindex");
}

/*
* Setters
*/

void Location::set_index(str_t line)
{
    size_t space;

    if (line == "")
        return ;
    space = line.find(" ");
    while (space != str_t::npos)
    {
        if (line.find(" ", space + 1) != str_t::npos)
            this->_index.push_back(line.substr(space + 1, line.find(' ', space + 1) - space - 1));
        else
            this->_index.push_back(line.substr(space + 1));
        space = line.find(' ', space + 1);
    }
}

void Location::set_fastcgi_pass(str_t line)
{
    if (line == "")
        this->_fastcgi_pass = "";
    else
        this->_fastcgi_pass = line.substr(line.find(" ") + 1);
}


void Location::set_fastcgi_param(str_t line)
{
    size_t space;
    size_t space2;
    size_t end;

    if (line == "")
        return ;
    space = line.find(" ");
    space2 = space + 1;
    
    while (line[space2] != ' ') { space2++; };

    end = space2 + 1;
    while (line[end] != str_t::npos) { end++; }

    this->_fastcgi_param.insert(std::pair<str_t, str_t>(line.substr(space + 1, line.find(' ', space + 1)), line.substr(space2 + 1, end)));
}

/*
* Getters
*/

str_t Location::autoindex() const { return (this->_autoindex); }

std::list<str_t> Location::index() const { return (this->_index); }

str_t Location::fastcgi_pass() const { return (this->_fastcgi_pass); }

strMap Location::fastcgi_param() const { return (this->_fastcgi_param); }

str_t Location::path() const { return (this->_path); }

/*
* Member functions
*/

str_t Location::search_config(str_t config, str_t key)
{
    size_t  begin;
    size_t  end;
    size_t  duplicate;
    str_t   line;

    begin = config.find(key, 2);
    if (begin == str_t::npos)
        return ("");
    
    while (config[begin - 2] != ';' && config[begin - 2] != '{' && config[begin - 2] != '}'
            && config[begin - 2] != str_t::npos)
    {
        begin = config.find(key, begin + 1);
        if (begin == str_t::npos)
            return ("");
    }

    if (config[begin + key.size()] != ' ')
        throw str_t("error: missing space for " + key);
    
    end = config.find(";", begin);
    duplicate = end;
    
    while ((duplicate = config.find(key, duplicate + 1)) != str_t::npos)
    {
        if (config[duplicate - 2] == ';' || config[duplicate - 2] == '}')
            throw str_t("error: duplicate key: " + key);
    }

    return (line = config.substr(begin, (end - begin)));
}
