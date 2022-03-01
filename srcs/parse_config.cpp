#include <cstddef>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include "Config.hpp"
#include "str_manips.hpp"

// print vectors to debug
template<typename T>
void    printVec(std::vector<T> &v)
{
	size_t  last = v.size() - 1;

	std::cout << "{" ;
	for (size_t i = 0 ; i < v.size() ; ++i)
	{
		std::cout << v[i];
		if (i != last)
			std::cout << ", ";
	}
	std::cout << "}" << std::endl;
};

std::vector<size_t> get_server(std::string config)
{
    int brackets;
    std::vector<size_t> block;
    size_t start;
    size_t end;
    size_t server;

    brackets = 0;
    end = 0;
    start = config.find('{', 0);
    server = config.find("server", 0);
    
    if (start == std::string::npos || server == std::string::npos || start != (server + 7))
        return (block);
    
    for (size_t i = start; i < config.size(); i++)
    {
        if (config[i] == '{')
            brackets++;
        else if (config[i] == '}')
            brackets--;
        if (brackets == 0)
            end = i;
        if (end == i)
            break;
    }
    
    block.push_back(server);
    block.push_back(end);
    
    return (block);
}



void check_semicolon(str_t& config_final, size_t i, int nb_line, bool line_location)
{
    size_t j;
    size_t server_pos;

    j = i;
    if (j == 0 || (j > 0 && config_final[j - 1] == ';'))
        return;
    j--;

    if (config_final[j] == ' ' || config_final[j] == '\t'|| config_final[j] == '{' 
        || config_final[j] == '}' || config_final[j] == '\n')
    {
        while (j != 0 && (config_final[j] == ' ' || config_final[j] == '\t'
                            || config_final[j] == '{' || config_final[j] == '}'))
            j--;
        
        if (config_final[j] != '\n' && config_final[j] != ' ' && config_final[j] != '\t'
            && line_location == false)
            throw str_t("error (line " + to_string(nb_line) + "): endline problem");
        
        return;
    }

    if (j >= 5)
        server_pos = config_final.find("server", (j - 5));
    else
        throw str_t("error (line " + to_string(nb_line) + "): bad endline character");

    if (server_pos != (j - 5))
        throw str_t("error (line " + to_string(nb_line) + "): bad endline character");
}

void clean_config(str_t& config)
{
    int nb_line;
    bool line_location;
    size_t location;
    size_t double_space;

    nb_line = 1;
    line_location = false;

    for (size_t i = 0 ; i < config.size() ; i++)
    {
        location = config.find("location", i);
        if (i == location)
            line_location = true;
        if (config[i] == ';' && config[i + 1] != '\n')
            throw str_t("error (line " + to_string(nb_line) + "): semicolon is not end character");
        if (i > 0)
            if (config[i] == ';' && (config[i - 1] == ' ' || config[i - 1] == '\t'))
                throw str_t("error (line " + to_string(nb_line) + "): space or tab before semicolon");
        if (config[i] == '\n')
        {
            check_semicolon(config, i, nb_line, line_location);
            nb_line++;
            line_location = false;
        }
    }

    for (size_t i = 0 ; i < config.size() ; i++)
    {
        if (config[i] == '\n' || config[i] == '\t')
            config[i] = ' ';
    }

    double_space = config.find("  ");
    while (double_space != str_t::npos)
    {
        config.erase(double_space, 1);
        double_space = config.find("  ");
    }
}

config_v parse_config(const char* config)
{
  	// open the file
    std::ifstream config_open;
    config_open.open(config);
    if (!config_open)
        throw str_t("error: config doesn't open");

    // read the file
    std::stringstream config_read;
    str_t       config_final;
    config_read << config_open.rdbuf();
    config_final = config_read.str();
    if (config_final.empty())
        throw str_t("error: config is empty");

    // std::cout << config_final << std::endl << std::endl;
    // clean config
    clean_config(config_final);
    // std::cout << config_final << std::endl << std::endl;

    std::vector<size_t> block;
    block = get_server(config_final);
    if (block.empty() == true)
        throw std::string("error: no server found");

    // check what the block contains
    // std::cout << "Block:" << std::endl;
    // printVec(block);

    config_v new_config;
    {
        // fill it with a buffer 
        Config tmp_config(config_final.substr(block[0], (block[1] - block[0])));
        
        new_config.push_back(tmp_config);
        config_final.erase(block[0], block[1]);
        block = get_server(config_final);
   }

    return (new_config);
}

config_v	parsing_config(int argc, char **argv)
{
	config_v config;

	try
    {
        if (argc != 2)
            config = parse_config("miniConf.conf");
        else
            config = parse_config(argv[1]);
    }
	catch (str_t error)
        {fatal(error);}

    // print Config
    for (config_v::iterator it = config.begin() ; it < config.end() ; it++)
    {
        std::cout << *it << std::endl;
    }

	return (config);
}

// int	main(int argc, char **argv)
// {
// 	config_v config = parsing_config(argc, argv);
// 	return 0;
// }