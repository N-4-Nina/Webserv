#include "CGI.hpp"
#include "Config.hpp"

// print the env and args for testing
void display_cgi_env(char **env, char **args)
{
    std::cout << "---- CGI ENV VARIABLES ----" << std::endl;
    for (int i = 0 ; env[i] ; i++)
        std::cout << env[i] << std::endl;
    std::cout << "---- CGI ARGS EXECUTE ----" << std::endl;
    for (int j = 0 ; args[j] ; j++)
        std::cout << args[j] << std::endl;
    std::cout << "---------------------------" << std::endl;
}

CGI::CGI() {}

CGI::~CGI() {}

void CGI::set_binary(str_t path)
{
    _binary = path;
}

// void CGI::set_input(str_t content)
// {
//     _input = content;
// }

void free_str_tab(char **str_tab)
{	
	int i = -1;
	while (str_tab[++i] != 0)
		free(str_tab[i]);
	free(str_tab);
}

void free_cgi(char **args, char **env)
{
    free_str_tab(args);
    free_str_tab(env);
    args = NULL;
    env = NULL;
}

void CGI::exec_cgi(str_t target, strMap headers, std::vector<str_t> body, unsigned int type)
{    
	char **args = NULL;
    char **env = NULL;
    std::ostringstream output;

    int i = body.size();
    while (i > 0)
    {
        output << body.back();
        output << "\n";
        i--;
    }
	_body = output.str();
   
    std::cout << _body << std::endl;
    

    args = (char**)malloc(sizeof(char*) * 3);
    args[0] = strdup(_binary.c_str());
    args[1] = strdup(target.c_str());
    args[2] = 0;

// add request to build a complete env
    env = build_cgi_env(headers, type);
	// display_cgi_env(env, args);

    pid_t pid;
	int save_stdin;
	int save_stdout;

	// save stdin and out to turn them back to normal after
	save_stdin = dup(STDIN_FILENO);
	save_stdout = dup(STDOUT_FILENO);

	// tmpfile - creates a temporary binary file, open for update with a filename guaranteed to be different from any other existing file
	FILE	*file_in = tmpfile();
	FILE	*file_out = tmpfile();
	// fileno - map a stream pointer to a file descriptor
	long	fd_in = fileno(file_in);
	long	fd_out = fileno(file_out);
	// int		ret = 1;

	write(fd_in, _body.c_str(), _body.size());
	lseek(fd_in, 0, SEEK_SET);

	if ((pid = fork()) == -1)
        throw str_t("error: fork failed on CGI: PID = -1");
	else if (pid == 0)
	{

		// STDOUT become a copy of fd_out, and, in case of POST, STDIN become a copy of fd_in
		dup2(fd_in, STDIN_FILENO);
		dup2(fd_out, STDOUT_FILENO);

		if (execve(_binary.c_str(), args, env) < 0)
			exit(-1);				// bail d\erreur à renvoyer 

	}
	// else
	// {

	// }

    free_cgi(args, env);
}

/*
* --- variables related to the server ---
* GATEWAY_INTERFACE       : CGI Version
* SERVER_SOFTWARE         : Serveur
* SERVER_NAME             : host name, alias DNS or IP address
* --- variables related to the request ---
* SERVER_PROTOCOL         : Protocole
* SERVER_PORT             : Port
* REQUEST_METHOD          : Method, generally GET or POST
* PATH_INFO               : Supp script's path give by the client. EX: ifthe server hosts "/cgi-bin/myscript.cgi" and the client asks for the url "http://server.org/cgi-bin/myscript.cgi/mysearch", PATH_INFO will contain "mysearch"
* PATH_TRANSLATED         : getcwd + root (sans le .) + uri
* SCRIPT_NAME             : virtual path to the executed script. EX: « /cgi-bin/script.cgi »
* QUERY_STRING            : The URL-encoded information that is sent with GET method request, everything that follows the "?" in the URL. Else, empty
* CONTENT_LENGTH          : If method=POST, contained the CGIstring's lenght. Else 0
* REMOTE_ADDR             : client IP
* CONTENT_TYPE            : content type attached to the request. EX: a form send with POST
* REQUEST_URI             : requested URI
* REDIRECT_STATUS         : to make php-cgi work
* --- variables from the client ---
* All variables that are sent by the client are also passed to the CGI script, after the
* server has added the "HTTP_" prefix
* Ref: https://fr.wikipedia.org/wiki/Variables_d%27environnement_CGI
* Ref: https://web.developpez.com/cgic.htm
*/

char **CGI::build_cgi_env(strMap headers, unsigned int type)
{
    (void)headers;
    (void)type;

    char **env = NULL;
    // strMap envMap;
	// // unsigned int type;

    // /* Get and set CGI informations */
    // envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
    // envMap["SERVER_SOFTWARE"] = "webserv";
    // envMap["SERVER_NAME"] = "127.0.0.1";
    // envMap["SERVER_PROTOCOL"] = SERVER_VERSION;
    // envMap["SERVER_PORT"] = ""; // depuis le port recup dans le request??? need to convert char* to string, using constructor of std::string?

	// // type = req.type();
	// str_t method_type = to_string(type);
	// envMap["REQUEST_METHOD"] = method_type; // from request
    
	// envMap["PATH_INFO"] = ""; // uri from request
    // envMap["PATH_TRANSLATED"] = ""; // need request location and uri
    // envMap["SCRIPT_NAME"] = ""; // need from request
    // envMap["QUERY_STRING"] = ""; // "
    // envMap["CONTENT_LENGTH"] = ""; // body size
    // envMap["REMOTE_ADDR"] = ""; // client ip
    // envMap["CONTENT_TYPE"] = ""; // content type from req
    // envMap["REQUEST_URI"] = ""; // uri from req
    // //envMap["REDIRECT_STATUS"] = "200"; //

    // /* Request headers pass to CGI */
    // envMap["HTTP_EXAMPLE"] = "EXAMPLE"; // needs HTTP prefix
    // // to code

    // int i = -1;
    // env = (char**)malloc(sizeof(char*) * (envMap.size() + 1));

    // for (strMap::iterator it = envMap.begin() ; it != envMap.end() ; it++)
    //     env[++i] = (char*)strdup((it->first + "=" + it->second).c_str());

    // env[++i] = 0;
    // envMap.clear();
    return (env);
}

// int main ()
// {
//     CGI cgi;
// 	Request req("uwu", 2);

//     // set binary (path) for the cgi
//     cgi.set_binary("BINARY NAME");
//     // execute cgi
//     cgi.exec_cgi("CGI ARG", req);

//     return 0;
// }