#include "Config.hpp"

Config::Config(str_t config) : 
_host("localhost"), _ports(1, 80),  _server_name(1, "webserv"), 
_client_max_body_size(1000000), _root("/"), _index(1, "index.html"),
_autoindex("off")
{
	size_t pos = -1;
	str_t tmp;

	while ((pos = config.find("listen", pos + 1)) != str_t::npos)
	{
		tmp = config.substr(pos - 6);
		set_host_port(search_config(tmp, "listen"));
	}
	set_server_name(search_config(config, "server_name"));
	set_error_page(config);
	set_client_max(search_config(config, "client_max_body_size"));
	set_root(search_config(config, "root"));
	set_index(search_config(config, "index"));
	set_location(config);
}

Config::Config(const Config &ref)
{
	*this = ref;
}

Config::~Config(void) {}

/*
* Setters
*/

void Config::set_host_port(str_t line)
{
	int		tmp_port = 0;
	size_t	space_pos;
	size_t	split_pos;
	std::stringstream strst;

	if (line == "")
		return ;

	space_pos = line.find(" ");
	split_pos = line.find(":");

	if (split_pos == str_t::npos)
	{
		for (size_t i = space_pos + 1 ; i < line.size() ; i++)
		{
			if (!std::isdigit(line[i]))
			{
				this->_host = line.substr(space_pos + 1);
				if (this->_host != "localhost")
					throw str_t("error: wrong IP");
				tmp_port = 80;
				for (std::vector<int>::iterator it =  _ports.begin() ; it !=  _ports.end() ; ++it)
				{
					if (tmp_port == *it)
						return ;
				}
				 _ports.push_back(tmp_port);
			}
		}
		if (tmp_port != 80)
		{
			this->_host = "localhost";
			strst << line.substr(space_pos + 1);
			strst >> tmp_port;

			for (std::vector<int>::iterator it =  _ports.begin() ; it !=  _ports.end() ; ++it)
			{
				if (tmp_port == *it)
					throw str_t("error: duplicate port");
			}
			 _ports.pop_back();
			 _ports.push_back(tmp_port);
		}
	}
	else
	{
		this->_host = line.substr(space_pos + 1, (split_pos - space_pos - 1));

		if (this->_host != "localhost")
			throw str_t("error: wrong ip");
		
		strst << line.substr(split_pos + 1);
		strst >> tmp_port;

		for (std::vector<int>::iterator it =  _ports.begin() ; it !=  _ports.end() ; ++it)
		{
			if (tmp_port == *it)
				return ;
		}
		 _ports.push_back(tmp_port);
	}
}


void Config::set_server_name(str_t line)
{
	size_t space;

	if (line == "")
		return ;
	
	space = line.find(" ");

	while (space != str_t::npos)
	{
		if (line.find(" ", space + 1) != str_t::npos)
			this->_server_name.push_back(line.substr(space + 1, line.find(' ', space + 1) - space - 1));
		else
			this->_server_name.push_back(line.substr(space + 1));
		space = line.find(' ', space + 1);
	}
	if (_server_name.size())
		_host = _server_name[0];
}
	
void Config::set_error_page(str_t config)
{
	size_t begin;
	size_t end;
	size_t begin_code;
	size_t end_code;
	str_t line;
	str_t code;
	str_t value;

	begin = 0;

	while ((begin = config.find("error_page", begin + 1)) != str_t::npos)
	{
		if (config[begin + 10] != ' ')
			throw str_t("error: missing space for error_page");
		
		end = config.find(";", begin);
		line = config.substr(begin, (end - begin));
		value = line.substr(line.find_last_of(" ") + 1);
		begin_code = line.find(" ") + 1;
		end_code = begin_code;

		while ((end_code = line.find(" ", end_code + 1)) != str_t::npos)
		{
			code = line.substr(begin_code, end_code - begin_code);
			this->_error_page[code] = value;
			begin_code = end_code + 1;
		}
		begin = end;
	}
	return ;
}

void Config::set_client_max(str_t line)
{
	std::stringstream strst;

	if (line == "")
		return ;
	else
	{
		strst << line.substr(line.find(" ") + 1);
		strst >> this->_client_max_body_size;
	}
}

void Config::set_root(str_t line)
{
	if (line == "")
		throw str_t("error: root can't be find");
	this->_root = line.substr(line.find(" ") + 1);
}

void Config::set_index(str_t line)
{
	size_t space;

	if (line == "")
		return;
	
	space = line.find(" ");
	int clear = 1;
	while (space != std::string::npos)
	{
		if (line.find(" ", space + 1) != std::string::npos)
			this->_index.push_back(line.substr(space + 1, line.find(' ', space + 1) - space - 1));
		else
			this->_index.push_back(line.substr(space + 1));
		if (clear && !(clear = 0))
			_index.erase(_index.begin());
		space = line.find(' ', space + 1);
	}
}

void Config::set_autoindex(str_t line)
{
	str_t tmp;

	if (line == "")
		return ;
	tmp = line.substr(line.find(" ") + 1);
	if (tmp == "on" || tmp == "off")
		this->_autoindex = tmp;
	else
		throw str_t("error: bad arguement for autoindex");
}

void Config::set_location(str_t line)
{
	std::vector<size_t>           tmp;
	std::string                   block;
	std::vector<size_t>::iterator it;

	tmp = search_location(line);
	
	while (!tmp.empty())
	{
		it = tmp.begin();
		block = line.substr(tmp[0], (tmp[1] - tmp[0]));
		Location new_location(block);
		this->_location.push_back(new_location);
		tmp.erase(tmp.begin(), tmp.begin() + 2);
	}
}

/*
* Getters
*/

str_t Config::host() const { return (this->_host ); }

std::vector<int> Config::ports() const { return ( _ports); }

std::vector<str_t> Config::server_name() const { return (this->_server_name); }

strMap Config::error_page() const { return (this->_error_page); }

size_t Config::client_max() const { return (this->_client_max_body_size); }

str_t Config::root() const { return (this->_root); }

std::list<str_t> Config::index() const { return (this->_index); }

str_t Config::autoindex() const { return (this->_autoindex); }

std::vector<Location> Config::location() const { return (this->_location); }

/*
* Member functions
*/

str_t Config::search_config(str_t config, str_t key)
{
	size_t begin;
	size_t end;
	size_t duplicate;
	str_t line;
	std::vector<size_t> locations;
	int i = 1;

	begin = config.find(key);
	if (begin == str_t::npos)
		return ("");

	while (config[begin - 2] != ';' && config[begin - 2] != '{' && config[begin - 2] != '}'
			&& begin != str_t::npos)
	{
		begin = config.find(key, begin + 1);
		if (begin == str_t::npos)
			return ("");
	}
	if (begin == str_t::npos)
		return ("");
	
	if (config[begin + key.size()] != ' ')
		throw str_t("error: missing space for " + key);
	
	end = config.find(";", begin);
	duplicate = end;

	locations = search_location(config);

	while ((duplicate = config.find(key, duplicate + 1)) != str_t::npos)
	{
		for (std::vector<size_t>::iterator it = locations.begin() ; it != locations.end() ; ++it)
		{
			if (duplicate > *it && duplicate < *(it + 1) && config[*it] != '}')
			{
				i = 0 ;
				break ;
			}
			i++;
		}
		if ((config[duplicate - 2] == ';' || config[duplicate - 2] == '}') && i != 0 && key != "listen")
			throw str_t("error: duplicate key: " + key);
		i = 1;
	}
	return (line = config.substr(begin, (end - begin)));
}

std::vector<size_t> Config::search_location(str_t config)
{
	std::vector<size_t> locations;
	size_t              pos = 0;
	size_t              space = 0;
	size_t              end = 0;
	size_t              other = 0;
	int                 i = 0;

	while ((pos = config.find("location", pos + 1)) != str_t::npos)
	{
		if (config[pos + 8] != ' ')
			throw str_t("error: missing space for location");
		
		space = config.find(" ", pos + 9);

		if (config[space + 1] != '{')
			throw str_t("error: no { for location");

		end = config.find("}", space);
		other = config.find("location", pos + 1);

		if (other != str_t::npos && end > other)
			throw str_t("error: another location in location");
		
		locations.push_back(pos + 9);
		locations.push_back(end);
		
		i = i + 2;
	}
	return (locations);
}

/*
* Non-member functions
*/

std::ostream& operator<<(std::ostream& os, Config const& src)
{
	os << std::endl;
	os << "<----- CONFIGURATION ----->" << std::endl << std::endl;

	os << "host: " << src.host() << std::endl;
	
	os << "ports: " << std::endl;
	std::vector<int> ports = src.ports();
	for (std::vector<int>::iterator it = ports.begin() ; it != ports.end() ; ++it)
		os << "\t- " << *it << std::endl;
	
	os << "server_name: " << std::endl;
	std::vector<std::string> names = src.server_name();
	for (std::vector<std::string>::iterator it = names.begin() ; it != names.end() ; ++it)
		os << "\t- " << *it << std::endl;
	
	os << "error_page: " << std::endl;
	strMap error_pages = src.error_page();
	if (error_pages.empty() == false)
		for (strMap::const_iterator it = error_pages.begin() ; it != error_pages.end() ; ++it)
			os << "\t- " << it->first << " " << it->second << std::endl;

	os << "client_max_body_size: " << src.client_max() << std::endl;
	
	os << "root: " << src.root() << std::endl;
	
	os << "index: " << std::endl;
	std::list<std::string> index = src.index();
	for (std::list<std::string>::iterator it = index.begin() ; it != index.end() ; ++it)
		os << "\t- " << *it << std::endl;
	
	os << "location: " << std::endl;
	std::vector<Location> location = src.location();
	for (std::vector<Location>::iterator it = location.begin() ; it != location.end() ; ++it)
		os << *it << std::endl;
	
	return (os);
}