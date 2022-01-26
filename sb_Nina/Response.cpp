#include "../include/Response.hpp"

Response::Response(void)
{
}

Response::Response(const Response &ref)
{
}

Response	&Response::operator=(const Response &ref)
{
	return (*this);
}

Response::~Response(void)
{
}

void			Response::set_status(unsigned int s)
{
	_status = s;
}

unsigned int	Response::status()
{
	return (_status);
}

void			Response::send()
{

}