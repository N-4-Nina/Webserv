#include "CGI.hpp"
#include "Config.hpp"
#include "flags.hpp"
#include "EvMa.hpp"

// the capacity of a pipe
// https://man7.org/linux/man-pages/man7/pipe.7.html
#define CGI_BUF_SIZE 65536

CGI::CGI(EvMa *evma)
{
	_evma = evma;
	_pid = 0;
	_status = 0;
	_fd_io[0] = -1;
	_fd_io[1] = -1;
	_done = 0;
}

CGI::~CGI()
{
	//close_fd();
	//close(_fd_io[0]);
	//close(_fd_io[1]);
	//close(_save_io[0]);
	//close(_save_io[1]);
}

void CGI::close_fd()
{
	if (_fd_io[0] > 1)
		close(_fd_io[0]);
	if (_fd_io[1] > 1)
		close(_fd_io[1]);
}

void CGI::set_binary(str_t path)
{
	_binary = path;
}

void CGI::exec_cgi(str_t target, Request req, strMap headers_resp, FLAGS *flags, unsigned int *code)
{
	char **args = NULL;
	char **env = NULL;

	args = (char**)malloc(sizeof(char*) * 3);
	args[0] = strdup(_binary.c_str());
	args[1] = strdup(target.c_str());
	args[2] = 0;
	env = build_cgi_env(req, target, headers_resp);
	// DEBUG_display_cgi_env(env, args);

	pid_t pid;

	// tmpfile - creates a temporary binary file, open for update with a filename guaranteed to be different from any other existing file

	// fileno - map a stream pointer to a file descriptor
	//_fd_io[0] = fileno(file_in);
	if (pipe(_fd_io))
		fatal("error: pipe failed");
	//_fd_io[1] = fileno(file_out);

	if ((pid = fork()) == -1)
		fatal("error: fork failed on CGI: PID = -1");
	else if (pid == 0)
	{
	  // STDOUT become a copy of _fd_io[1], and, in case of POST, STDIN become a copy of _fd_io[0]

		close(_fd_io[0]);
		dup2(_fd_io[1], STDOUT_FILENO);
		close(_fd_io[1]);
		_evma->close_all();
		if (execve(_binary.c_str(), args, env) < 0)
		{
			std::cout << "execve failed\n";
			kill (getpid(), SIGKILL);
		}
	}

	_pid = pid;
	
	close(_fd_io[1]);
	//check(flags, code);
	(void)code;
	(void)flags;
	free_cgi(args, env);
}

void	CGI::check(FLAGS *flags, unsigned int *code)
{
	//std::cout << _pid << "\n";
	if (!_done && waitpid(_pid, &_status, WNOHANG | WUNTRACED))
		_done = true;
	if (_done)
	{
		if (WIFEXITED(_status))
		{
			lseek(_fd_io[1], 0, SEEK_SET);
			char	tmp[CGI_BUF_SIZE];
			int	ret = 1;

			memset(tmp, 0, CGI_BUF_SIZE);
			ret = read(_fd_io[0], tmp, CGI_BUF_SIZE - 1);
			{
				_body += tmp;
				memset(tmp, 0, CGI_BUF_SIZE);
			}
			if (ret == 0)
			{
				*flags |= RES_READY;
				close(_fd_io[0]);
				kill(_pid, SIGKILL);
			}
		}
		else
		{
			*code = 502;
			*flags |= RES_READY;
			close(_fd_io[0]);
			//kill(_pid, SIGKILL);
		}
	}
}


/*
* Ref: https://web.maths.unsw.edu.au/~lafaye/CCM/cgi/cgienv.htm
* Ref: https://fr.wikipedia.org/wiki/Variables_d%27environnement_CGI
* Ref: https://web.developpez.com/cgic.htm
*/

void CGI::get_host_port(Request req, strMap &envMap)
{
	for (strMap::iterator it = req.headers().begin() ; it != req.headers().end() ; ++it)
	{
		if (it->first == "host")
		{
			size_t points = it->second.find(":");
			size_t start = 0;
			size_t end = it->second.size() - 1;
			while (start != points)
				start++;
			envMap["SERVER_NAME"] = it->second.substr(0, start);
			envMap["SERVER_PORT"] = it->second.substr(points + 1, end);
		}
	}
}
char **CGI::build_cgi_env(Request req, str_t target, strMap headers_resp)
{
	char **env;
	strMap envMap;
(void)headers_resp;
	envMap["REDIRECT_STATUS"] = "200"; // needed with php, not mandatory with python i think, but not a bad thing to have it
	envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	envMap["SCRIPT_NAME"] = _script_name;
	//envMap["CONTENT_TYPE"] = "text/x-python";
	envMap["PATH_INFO"] = req._ressource;
	envMap["PATH_TRANSLATED"] = target;
	envMap["QUERY_STRING"] = req.query_string();
	envMap["REQUEST_URI"] = req._ressource;
	get_host_port(req, envMap);
	envMap["SERVER_PROTOCOL"] = SERVER_VERSION;
	envMap["SERVER_SOFTWARE"] = "webserv/1.0"; // server name and its version

	int i = -1;
	env = (char**)malloc(sizeof(char*) * (envMap.size() + 1));

	for (strMap::iterator it = envMap.begin() ; it != envMap.end() ; it++)
		env[++i] = (char*)strdup((it->first + "=" + it->second).c_str());

	env[++i] = 0;
	envMap.clear();
	return (env);
}

str_t	CGI::body()
{ return (_body); }

str_t	CGI::binary()
{ return (_binary); }

str_t	CGI::script_name()
{ return (_script_name); }

int		CGI::pid()
{ return (_pid); }

void	CGI::reset()
{
	_binary.clear();
	_body.clear();
	_script_name.clear();
	_pid = 0;
	_status = 0;
	_fd_io[0] = -1;
	_fd_io[1] = -1;
	_done = false;
}

void CGI::free_str_tab(char **str_tab)
{	
	int i = -1;
	while (str_tab[++i] != 0)
		free(str_tab[i]);
	free(str_tab);
}

void CGI::free_cgi(char **args, char **env)
{
	free_str_tab(args);
	free_str_tab(env);
	args = NULL;
	env = NULL;
}

void	CGI::set_script_name(str_t script_name)
{  _script_name = script_name; }

// print the env and args for testing
void DEBUG_display_cgi_env(char **env, char **args)
{
	std::cout << "\n\n---- CGI ENV VARIABLES ----" << std::endl;
	for (int i = 0 ; env[i] ; i++)
		std::cout << env[i] << std::endl;
	std::cout << "---- CGI ARGS EXECUTE, BIN + SCRIPT ----" << std::endl;
	for (int j = 0 ; args[j] ; j++)
		std::cout << args[j] << std::endl;
	std::cout << "---------------------------" << std::endl;
}
