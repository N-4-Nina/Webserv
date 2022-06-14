#include "Location.hpp"
#include "str_manips.hpp"
#include "flags.hpp"
#include "utils.hpp"

/*
					.--------------.
					| Constructors |
					'--------------'
*/

Location::Location(str_t block)
{
	_flags = 0;
	this->_route = block.substr(0, block.find(" "));
	set_root(search_config(block, "root"));
	set_autoindex(search_config(block, "autoindex"));
	set_index(search_config(block, "index"));
	set_methods(search_config(block, "methods"));
	set_cgi_path(search_config(block, "cgi_path"));
	set_cgi_extension(search_config(block, "cgi_extension"));
	set_upload_pass(search_config(block, "upload_pass"));
	set_upload_path(search_config(block, "upload_path"));
	set_redir(search_config(block, "return"));
	if (_cgi_extension != "")
		_flags |= LOC_CGI;
}

Location::Location(const Location &ref)
{
	_index = ref._index;
	_methods = ref._methods;
	_root = ref._root;
	_route = ref._route;
	_cgi_path = ref._cgi_path;
	_cgi_extension = ref._cgi_extension;
	_flags = ref._flags;
	_upload_path = ref._upload_path;
	_redir = ref._redir;
}


/*
					.------------.
					| Destructor |
					'------------'
*/

Location::~Location()
{
}


/*
					.-----------.
					| Operators |
					'-----------'
*/

Location &Location::operator=(const Location &ref)
{
	if (this != &ref)
	{
		_index = ref._index;
		_root = ref._root;
		_route = ref._route;
		_cgi_path = ref._cgi_path;
		_cgi_extension = ref._cgi_extension;
		_flags = ref._flags;
		_upload_path = ref._upload_path;
		_redir = ref._redir;
	}
	return (*this);
}


/*
					.---------.
					| Setters |
					'---------'
*/

void Location::set_root(str_t line)
{
	if (line == "")
		this->_flags |= LOC_ROOT;
	else
		this->_root = line.substr(line.find(" ") + 1);
	append_slash(_root);
}

void Location::set_autoindex(str_t line)
{
	str_t tmp;
	tmp = line.substr(line.find(" ") + 1);
	if (line == "" || tmp == "off");   
	else if (tmp == "on")
		this->_flags |= LOC_AUTO;
	else
		throw str_t("error: bad arguement for autoindex");
}

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
		_flags |= LOC_INDEX;
	}
}

void Location::set_methods(str_t line)
{
	size_t space;

	if (line == "")
		return ;
	space = line.find(" ");
	while (space != str_t::npos)
	{
		if (line.find(" ", space + 1) != str_t::npos)
			this->add_method(line.substr(space + 1, line.find(' ', space + 1) - space - 1));
		else
			this->add_method(line.substr(space + 1));
		space = line.find(' ', space + 1);
		_flags |= LOC_METHOD;
	}
}

void Location::add_method(str_t str)
{
	size_t found = 3;
	static str_t strTypes[3] = {"get", "post", "delete"};

	for (size_t i = 0; i <= 2; i++)
	{
		if (str == strTypes[i])
			found = i;
	}
	if (found == 3)
		throw str_t("error: bad method in conf");
	else
	{
		_methods |= int_pow(2, found);
	}
}

void Location::set_cgi_path(str_t line)
{
   if (line == "")
	   this->_cgi_path = "";
	else
		this->_cgi_path = line.substr(line.find(" ") + 1);
}

void Location::set_cgi_extension(str_t line)
{
   if (line == "")
	   this->_cgi_extension = "";
	else
		this->_cgi_extension = line.substr(line.find(" ") + 1);
}

void Location::set_upload_pass(str_t line)
{
	if (line == "")
		return;
	str_t tmp = line.substr(line.find(" ") + 1);
	if (tmp == "true")
		_flags |= LOC_UPLOAD;
	else if (tmp == "false");
	else
		std::cout << "bad value for upload pass";
}

void Location::set_upload_path(str_t line)
{
	if (line == "" || !(_flags & LOC_UPLOAD))
		return;
	else
	{
		_upload_path = line.substr(line.find(" ") + 1);
		struct stat s;
		
		if ( stat(_upload_path.c_str(), &s) == 0 )
		{
		    if ( !(s.st_mode & S_IFDIR) )
		    {
				/*	The upload directory is actually not a directory, so we unset
					the upload flag. Beware, it causes the route to behave like 
					a non-upload route. */
		        _flags = _flags & ~LOC_UPLOAD;	// log: did not find upload directory
				return;
		    }
		}
		else if (mkdir(_upload_path.c_str(), 0777))
		{
			/*	directory does not exist and cannot be created, so we unset the
				flag as well. */
			_flags = _flags & ~LOC_UPLOAD;	// log: did not find upload directory
			return;
		};
		append_slash(_upload_path);
	}
}

/*
		_redir is a pair (see common.hpp), which contained a long and a string
	(pair<long, string>), to access the redirection code and its url.
*/
void Location::set_redir(str_t line)
{
    size_t space_pos;
    size_t space_pos_bis;

    space_pos = 0;
    space_pos_bis = 0;
    if (line == "")
    {
        this->_redir.first = 0;
        this->_redir.second = "";
    }
    else
    {
        space_pos = line.find(" ");
        space_pos_bis = line.find(" ", space_pos + 1);
        if (space_pos == str_t::npos || space_pos_bis == str_t::npos ||
            line.find(" ", space_pos_bis + 1) != str_t::npos)
            throw str_t("Error: Wrong number argument for return");
        this->_redir.first = strtol(line.substr(space_pos + 1, (space_pos_bis - space_pos - 1)).c_str(), NULL, 10);
        if ( this->_redir.first < 300 ||  this->_redir.first > 310)
				throw str_t("Redirection status should be between 300 and 310");
		this->_redir.second = line.substr(space_pos_bis + 1);
		_flags |= LOC_REDIR;
    }
}


/*
					.---------.
					| Getters |
					'---------'
*/

std::list<str_t>	&Location::index() { return (_index); }

FLAGS				&Location::methods() { return (_methods); }

str_t Location::cgi_path() const { return (this->_cgi_path); }

str_t Location::cgi_extension() const { return (this->_cgi_extension); }

str_t Location::root() const { return (this->_root); }

str_t Location::route() const { return (this->_route); }

str_t Location::upload_path() const { return (_upload_path); }

FLAGS	Location::flags() const { return (this->_flags); }

/*
		typedef std::pair<long, str_t>  Redir;
		see common.hpp
*/
Redir Location::redir() const { return (this->_redir); }


/*
					.------------------.
					| Member functions |
					'------------------'
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
			&& begin - 2 != str_t::npos)
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
