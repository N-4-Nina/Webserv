#include "CGI.hpp"

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