#include "common.hpp"
#include "Request.hpp"
#include "EvMa.hpp"
#include "Config.hpp"
#include "utils.hpp"

void initialize_error_pages();

/*
                    .---------.
                    | Enforce |
                    '---------'
*/

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


/*
                    .------.
                    | Init |
                    '------'
*/

int check_duplicate(std::vector<Config> &conf)
{
    std::map<int, int> used;
    int i = 0;

    for (std::vector<Config>::iterator it = conf.begin(); it != conf.end(); it++)
    {
        for (std::vector<int>::iterator pit = it->ports().begin(); pit != it->ports().end(); pit++)
        {
            if (used.count(*pit) && used[*pit] != i)
            {
                std::cout << "\nerror: Found duplicate port in config file.\n";
                return (1);
            }
            used[*pit] = i;
        }
        i++;
    }
    return (0);
}

/*
                    .-------------.
                    | entry point |
                    '-------------'
*/

int main(int argc, char **argv)
{
    strMap          req_headers;

    std::vector<Config> conf = parsing_config(argc, argv);
    
    if (check_duplicate(conf))
        return (1);
    
    EvMa	ev(conf);
    initialize_mime_types();
    initialize_error_pages();
    ev.loop();
}