#include "common.hpp"
#include "Request.hpp"
#include "EvMa.hpp"
#include "Config.hpp"


void initialize_error_pages();

void    fatal(str_t str)
{
    std::cerr << str << "\n";
    exit(EXIT_FAILURE);
}

void    assert(bool scal, str_t log)
{
    if (!scal)
        fatal(log);
}


int main(int argc, char **argv)
{
    strMap          req_headers;

    std::vector<Config> conf = parsing_config(argc, argv);
    std::cout << conf[0];
    EvMa	ev(conf);
    initialize_mime_types();
    initialize_error_pages();
    ev.loop();
}