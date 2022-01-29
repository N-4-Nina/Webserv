#include <cstddef>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include "Config.hpp"

std::vector<Config> parse_config(const char *config)
{
	// open the file
    std::ifstream config_open;
    config_open.open(config);
    if (!config_open)
        throw std::string("error: config doesn't open");

    // read the file
    std::stringstream config_read;
    std::string       config_final;
    config_read << config_open.rdbuf();
    config_final = config_read.str();
    if (config_final.empty())
        throw std::string("error: config is empty");

    // need to transform characters and erase duplicate space

    std::cout << config_final << std::endl << std::endl;

    // create a object Config
    std::vector<Config> new_config;
	// need to fill this object
    return (new_config);
}

std::vector<Config>	parsing_config(int argc, char **argv)
{
	std::vector<Config> config;

	try
    {
        if (argc != 2)
            config = parse_config("miniConf.conf");
        else
            config = parse_config(argv[1]);
    }
	catch (std::string error)
    {
        std::cerr << error << std::endl;
        return std::vector<Config>();
    }
    // for (std::vector<Config>::iterator it = config.begin() ; it < config.end() ; it++)
    // {
    //     std::cout << *it << std::endl;
    // }

	return (config);
}

int	main(int argc, char **argv)
{
	std::vector<Config> config = parsing_config(argc, argv);
	return 0;
}