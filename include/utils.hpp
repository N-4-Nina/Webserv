#pragma once

#include "common.hpp"
time_t	time_in_ms(void);
void    fatal(str_t str);
void    assert(bool scal, str_t log);
int		unlock_socket(int fd);
void	initialize_mime_types();
void    log(Server *serv, Client *client, str_t err);
int     int_pow(int base, int exp);