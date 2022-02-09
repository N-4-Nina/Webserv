#ifndef CGI_HPP
# define CGI_HPP

# include "../include/common.hpp"

class CGI
{
    public:
        CGI();
        ~CGI();

        void set_binary(str_t path);
        void set_input(str_t content); //  in a CGI-BIN environment, you read the data from STDIN

    private:
        str_t _binary;
        str_t _input;
};

#endif