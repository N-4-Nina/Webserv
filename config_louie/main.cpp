#include <vector>
#include <map>
#include <string>
#include <iostream>

std::vector<Config> parse_config(const char *config_file)
{
	
}

std::vector<Config>	parsing_config(int argc, char **argv)
{
	std::vector<Config> config;
	config = parse_config(argv[1]);
}

int	main(int argc, char **argv)
{
	std::vector<Config> config = parsing_config(argc, argv);
	return 0;
}