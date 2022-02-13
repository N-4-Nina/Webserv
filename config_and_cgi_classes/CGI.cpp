#include "CGI.hpp"

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

void CGI::set_input(str_t content)
{
    _input = content;
}

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

void CGI::exec_cgi(str_t target)
{
    char **args = NULL;
    char **env = NULL;

    pid_t pid;

    args = (char**)malloc(sizeof(char*) * 3);
    args[0] = strdup(_binary.c_str());
    args[1] = strdup(target.c_str());
    args[2] = 0;

// add request to build a complete env
    env = build_cgi_env();

    // display_cgi_env(env, args);

    if ((pid = fork()) == -1)
        throw str_t("error: fork failed on CGI: PID = -1");
    else if (pid == 0)
    {
        
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

char **CGI::build_cgi_env()
{
    char **env;
    strMap envMap;

    /* Get and set CGI informations */
    envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
    envMap["SERVER_SOFTWARE"] = "webserv";
    envMap["SERVER_NAME"] = "127.0.0.1";
    envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
    envMap["SERVER_PORT"] = ""; // depuis le port recup dans le request??? need to convert char* to string, using constructor of std::string?
    envMap["REQUEST_METHOD"] = ""; // from request
    envMap["PATH_INFO"] = ""; // uri from request
    envMap["PATH_TRANSLATED"] = ""; // need request location and uri
    envMap["SCRIPT_NAME"] = ""; // need from request
    envMap["QUERY_STRING"] = ""; // "
    envMap["CONTENT_LENGTH"] = ""; // body size
    envMap["REMOTE_ADDR"] = ""; // client ip
    envMap["CONTENT_TYPE"] = ""; // content type from req
    envMap["REQUEST_URI"] = ""; // uri from req
    //envMap["REDIRECT_STATUS"] = "200"; //

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

int main ()
{
    CGI cgi;

    // set binary (path) for the cgi
    cgi.set_binary("BINARY NAME");
    // execute cgi
    cgi.exec_cgi("CGI ARG");

    return 0;
}