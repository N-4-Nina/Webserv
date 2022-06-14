#include "Config.hpp"


/*
					.--------------.
					| Constructors |
					'--------------'
*/

Config::Config(str_t config) : 
 _ports(),  _server_name(1, "webserv"), 
_client_max_body_size(1000000), _root("/"), _index(1, "index.html"),
_autoindex("off")
{
	_server_name.clear();
	set_ports(search_config(config, "listen"));
	set_server_name(search_config(config, "server_name"));
	set_error_page(config);
	set_client_max(search_config(config, "client_max_body_size"));
	set_root(search_config(config, "root"));
	set_index(search_config(config, "index"));
	set_autoindex(search_config(config, "autoindex"));
	set_location(config);
}

Config::Config(const Config &ref)
{
	*this = ref;
}


/*
					.------------.
					| Destructor |
					'------------'
*/

Config::~Config(void) {}


/*
					.-----------.
					| Operators |
					'-----------'
*/

Config &Config::operator=(const Config &ref)
{
	if (this != &ref)
	{
		_ports = ref._ports;
		_server_name = ref._server_name;
		_error_page = ref._error_page;
		_client_max_body_size = ref._client_max_body_size;
		_root = ref._root;
		_index = ref._index;
		_autoindex = ref._autoindex;
		_location = ref._location;
	}
	return (*this);
}


/*
					.---------.
					| Setters |
					'---------'
*/

void Config::set_ports(str_t line)
{
	size_t	space;

	if (line == "")
		return ;

	space = line.find(" ");

	while (space != str_t::npos)
	{
		if (line.find(" ", space + 1) != str_t::npos)
			this->_ports.push_back(atoi(line.substr(space + 1, line.find(' ', space + 1) - space - 1).c_str()));
		else
			this->_ports.push_back(atoi(line.substr(space + 1).c_str()));
		
		space = line.find(' ', space + 1);
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
			_error_page[code] = (value[0] == '/') ? value.substr(1, str_t::npos) : value;
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
		throw str_t("error: no root for /");
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
	std::vector<size_t>		   tmp;
	std::string				   block;
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
					.---------.
					| Getters |
					'---------'
*/

std::vector<int> &Config::ports() { return ( _ports); }

std::vector<str_t> &Config::server_name() { return (this->_server_name); }

strMap Config::error_page() const { return (this->_error_page); }

size_t Config::client_max() const { return (this->_client_max_body_size); }

str_t Config::root() const { return (this->_root); }

std::list<str_t> &Config::index() { return (this->_index); }

str_t Config::autoindex() const { return (this->_autoindex); }

location_v &Config::location() { return (this->_location); }


/*
					.------------------.
					| Member functions |
					'------------------'
*/

str_t	Config::search_config(str_t config, str_t key)
{
	size_t begin;
	size_t end;
	size_t duplicate;
	str_t line;
	std::vector<size_t> locations;
	int i = 1;

	if (key == "root")
	{
		if (search_root(config, key) == 2)
			config = config.substr(config.find("location / {"), str_t::npos);
		else if (search_root(config, key) == 1)
			throw str_t("error: no root for /");
	}


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
	size_t			  pos = 0;
	size_t			  space = 0;
	size_t			  end = 0;
	size_t			  other = 0;
	int				 i = 0;

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

int		Config::search_root(str_t config, str_t key)
{
	size_t root_pos = config.find(key);
	size_t loca_pos = config.find("location");

	if (loca_pos < root_pos)
	{
		if (config.find("location / {") != str_t::npos)
			return (2);
		else
			return (1);
	}
	else
		return (0);
}
