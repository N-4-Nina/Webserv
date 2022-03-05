#include "common.hpp"
#include "Response.hpp"

std::map<unsigned int, str_t>   Response::_codes;
str_t                           Response::_error_page[2];

void initialize_error_pages()
{
    /* INFORMATION*/
    Response::_codes[100] = "Continue";
    Response::_codes[101]= "Switching Protocols";
    
    /* SUCCESS */
    Response::_codes[200] = "Ok";
    Response::_codes[201] = "Created";
    Response::_codes[202] = "Accepted";
    Response::_codes[203] = "Non-Authoritative Information";
    Response::_codes[204] = "No Content";
    Response::_codes[205] = "Reset Content";
    Response::_codes[206] = "Partial Content";

    /* REDIRECTION */ 
    Response::_codes[300] = "Multiple choices";
    Response::_codes[301] = "Moved Permanently";
    Response::_codes[302] = "Found";
    Response::_codes[303] = "See Other";
    Response::_codes[304] = "Not Modified";
    Response::_codes[305] = "Use Proxy";
    Response::_codes[307] = "Temporary Redirect";

    /* CLIENT ERRORS */
    Response::_codes[400] = "Bad Request";
    Response::_codes[401] = "Unauthorized";
    Response::_codes[402] = "Payment Required";
    Response::_codes[403] = "Forbidden";
    Response::_codes[404] = "Not Found";
    Response::_codes[405] = "Method Not Allowed";
    Response::_codes[406] = "Not Acceptable";
    Response::_codes[407] = "Proxy Authentication Required";
    Response::_codes[408] = "Request Timeout";
    Response::_codes[409] = "Conflict";
    Response::_codes[410] = "Gone";
    Response::_codes[411] = "Length Required";
    Response::_codes[412] = "Precondition Failed";
    Response::_codes[413] = "Request Entity Too Large";
    Response::_codes[414] = "Request-URI Too Long";
    Response::_codes[415] = "Unsupported Media Type";
    Response::_codes[416] = "Requested Range Not Satisfiable";
    Response::_codes[417] = "Expectation Failed";

    /* SERVOR ERRORS */
    Response::_codes[500] = "Internal Server Error";
    Response::_codes[501] = "Not Implemented";
    Response::_codes[502] = "Bad Gateway";
    Response::_codes[503] = "Service Unavailable";
    Response::_codes[504] = "Gateway Timeout";
    Response::_codes[505] = "HTTP Version Not Supported";

    Response::_error_page[0] = "<!DOCTYPE html>\n<html>\n\n<head>\n\t<title>Webserv Error</title>\n</head>\n\n<body>\n\n<h1>";
    Response::_error_page[1] = "</h1>\n</body>\n</html> ";
}