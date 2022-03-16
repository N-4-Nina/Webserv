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

class CGI
{
    public:
        CGI();
        ~CGI();

        void set_binary(str_t path);
        void exec_cgi(str_t target, Request req);
        str_t get_body(Request req);
        str_t  body();

    private:
        str_t _binary;
		str_t _body;
        
        char **build_cgi_env(Request req, size_t body_size);
        void get_host_port(Request req, strMap &envMap);
};

#endif