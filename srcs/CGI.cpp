#include "CGI.hpp"
#include "Config.hpp"

// the capacity of a pipe
// https://man7.org/linux/man-pages/man7/pipe.7.html
#define CGI_BUF_SIZE 65536


CGI::CGI()
{}

CGI::~CGI()
{}

void CGI::set_binary(str_t path)
{
    _binary = path;
}

void CGI::exec_cgi(str_t target, Request req)
{
	char **args = NULL;
    char **env = NULL;
    int ret = 1;
    char tmp[CGI_BUF_SIZE];

    _body = get_body(req);

    args = (char**)malloc(sizeof(char*) * 3);
    args[0] = strdup(_binary.c_str());
    args[1] = strdup(target.c_str());
    args[2] = 0;
    env = build_cgi_env(req, _body.size());
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

    write(fd_in, _body.c_str(), _body.size());
    lseek(fd_in, 0, SEEK_SET);

//    std::cout << "\n\nbinary: " << _binary << std::endl;
//     std::cout << "_body: " << target << std::endl;

    if ((pid = fork()) == -1)
        fatal("error: fork failed on CGI: PID = -1");
    else if (pid == 0)
    {
      // STDOUT become a copy of fd_out, and, in case of POST, STDIN become a copy of fd_in
        dup2(fd_in, STDIN_FILENO);
        dup2(fd_out, STDOUT_FILENO);

        if (execve(_binary.c_str(), args, env) < 0)
            fatal("execve failed\n");
    }
    else
    {
        waitpid(-1, NULL, 0);
        lseek(fd_out, 0, SEEK_SET);

        while (ret > 0)
        {
            memset(tmp, 0, CGI_BUF_SIZE);
            ret = read(fd_out, tmp, CGI_BUF_SIZE - 1);
           _body += tmp;
        }

        close(fd_out);
        close(fd_in);

        dup2(save_stdin, STDIN_FILENO);
        dup2(save_stdout, STDOUT_FILENO);
    }
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

char **CGI::build_cgi_env(Request req, size_t body_size)
{
    char **env;
    strMap envMap;

    get_host_port(req, envMap);
    envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
    envMap["SERVER_SOFTWARE"] = "webserv/1.0"; // le nom du serveur et sa versoin
    envMap["SERVER_PROTOCOL"] = SERVER_VERSION;
    if (req.type() == 0)
        envMap["REQUEST_METHOD"] = "R_GET";
    else if (req.type() == 1)
        envMap["REQUEST_METHOD"] = "R_POST";
    else
        fatal("CGI can't work with another method than GET or POST"); // ne devrait pas etre delete dans tous les cas (cgi fonctionnent justes avec get et post)
    envMap["REQUEST_URI"] = req._ressource;
	envMap["PATH_INFO"] = req._ressource;
    envMap["PATH_TRANSLATED"] = ""; // need request location and uri
    envMap["SCRIPT_NAME"] = ""; // need from request
    envMap["CONTENT_LENGTH"] = to_string(body_size); // body size
    envMap["QUERY_STRING"] = "first_name=ZARA&last_name=ALI"; // "
    // envMap["CONTENT_TYPE"] = ""; // content type from req
    // envMap["REDIRECT_STATUS"] = "200"; //
    // envMap["REMOTE_ADDR"] = ""; // client ip

    /* Request headers pass to CGI */
    envMap["HTTP_EXAMPLE"] = "EXAMPLE"; // needs HTTP prefix
    // to code

    int i = -1;
    env = (char**)malloc(sizeof(char*) * (envMap.size() + 1));

    for (strMap::iterator it = envMap.begin() ; it != envMap.end() ; it++)
        env[++i] = (char*)strdup((it->first + "=" + it->second).c_str());

    env[++i] = 0;
    envMap.clear();
    return (env);
}

str_t CGI::body()
{ return (_body); }

str_t CGI::binary()
{ return (_binary); }

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

str_t CGI::get_body(Request req)
{
    str_t body;

    std::vector<str_t> bob = req.body();
    for (std::vector<str_t>::iterator itb = bob.begin(); itb != bob.end(); itb++)
        body = body + *itb;
    return (body);
}

// print the env and args for testing
void display_cgi_env(char **env, char **args)
{
    std::cout << "---- CGI ENV VARIABLES ----" << std::endl;
    for (int i = 0 ; env[i] ; i++)
        std::cout << env[i] << std::endl;
    std::cout << "---- CGI ARGS EXECUTE, BIN + SCRIPT ----" << std::endl;
    for (int j = 0 ; args[j] ; j++)
        std::cout << args[j] << std::endl;
    std::cout << "---------------------------" << std::endl;
}
