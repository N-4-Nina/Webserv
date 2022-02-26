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
#include "Request.hpp"

class CGI
{
    public:
        CGI();
        ~CGI();

        void set_binary(str_t path);
     //   void set_input(str_t content); //  in a CGI-BIN environment, you read the data from STDIN

        void exec_cgi(str_t target, strMap headers, std::vector<str_t> body, unsigned int type);

    private:
        str_t _binary;
        // unsigned int _type;
		str_t _body;
    //    str_t _input;
        
        char **build_cgi_env(strMap headers, unsigned int type);
};

#endif