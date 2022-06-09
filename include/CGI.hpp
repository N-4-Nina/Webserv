// when method == (POST || GET)
// set binary (path)
// execute
//      built env (meta-variables https://datatracker.ietf.org/doc/html/rfc3875#section-4.1) passed to execve
//      build execve 2nd arguments (array of string with path to binary (_binary) and arguments (target))
//      Pipe / Fork / Excve / Dup2 to write output to file and make CGI read the POST Body
//      in the parent, write BODY in STDIN of child with tube 
//      then, parse the CGI output, knowing that its output format will always be (whatever the CGI is) : headers ... \r\n\r\n ... body
// get output

#ifndef CGI_HPP
# define CGI_HPP

# include "common.hpp"
# include "Request.hpp"
# include "str_manips.hpp"
# include <sys/types.h>
# include <sys/wait.h>
# include <set>

class EvMa;

class CGI
{
	public:
		CGI(EvMa *evma = NULL);
		CGI(const CGI &ref);
		~CGI();

		CGI	&operator=(const CGI &ref);
		EvMa	*_evma;
		void	set_binary(str_t path);
		void	set_script_name(str_t script_name);
		void	exec_cgi(str_t target, Request req);
		void	check(FLAGS *flags, unsigned int *code);
		str_t 	body();
		str_t	binary();
		str_t	script_name();
		int		pid();
		void	reset();
		void	close_fd();

	private:
		str_t	_binary;
		str_t	_body;
		str_t	_script_name;
		int		_pid;
		int		_status;
		int		_fd_io[2];
		bool	_done;
		char	**build_cgi_env(Request req, str_t target);
		void	get_host_port(Request req, strMap &envMap);
		void	free_str_tab(char **str_tab);
		void	free_cgi(char **args, char **env);
};

void DEBUG_display_cgi_env(char **env, char **args);

#endif