#include "common.hpp"
#include "Response.hpp"

/*
	List taken from :
	https://docs.w3cub.com/http/basics_of_http/mime_types/complete_list_of_mime_types.html
*/

strMap	Response::_mimeTypes;

void	initialize_mime_types()
{
	strMap &types = Response::_mimeTypes;
	
	types[".aac"] = "audio/aac";
	types[".abw"] = "application/x-abiword";
	types[".arc"] = "application/octet-stream";
	types[".avi"] = "video/x-msvideo";
	types[".azw"] = "application/vnd.amazon.ebook";
	types[".bin"] = "application/octet-stream";
	types[".bz"] = "application/x-bzip";
	types[".bz2"] = "application/x-bzip2";
	types[".csh"] = "application/x-csh";
	types[".css"] = "text/css";
	types[".csv"] = "text/csv";
	types[".doc"] = "application/msword";
	types[".epub"] = "application/epub+zip";
	types[".gif"] = "image/gif";
	types[".htm"] = "text/html";
	types[".html"] = "text/html";
	types[".ico"] = "image/x-icon";
	types[".ics"] = "text/calendar";
	types[".jar"] = "Temporary Redirect";
	types[".jpeg"] = "image/jpeg";
	types[".jpg"] = "image/jpeg";
	types[".js"] = "application/js";
	types[".json"] = "application/json";
	types[".mid"] = "audio/midi";
	types[".midi"] = "audio/midi";
	types[".mpeg"] = "video/mpeg";
	types[".mpkg"] = "application/vnd.apple.installer+xml";
	types[".odp"] = "application/vnd.oasis.opendocument.presentation";
	types[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	types[".odt"] = "application/vnd.oasis.opendocument.text";
	types[".oga"] = "audio/ogg";
	types[".ogv"] = "video/ogg";
	types[".ogx"] = "application/ogg";
	types[".png"] = "image/png";
	types[".pdf"] = "application/pdf";
	types[".ppt"] = "application/vnd.ms-powerpoint";
	types[".rar"] = "application/x-rar-compressed";
	types[".rtf"] = "application/rtf";
	types[".sh"] = "application/x-sh";
	types[".svg"] = "image/svg+xml";
	types[".swf"] = "application/x-shockwave-flash";
	types[".tar"] = "application/x-tar";
	types[".tif"] = "image/tiff";
	types[".tiff"] = "image/tiff";
	types[".ttf"] = "application/x-font-ttf";
	types[".txt"] = "text/plain";
	types[".vsd"] = " application/vnd.visio";
	types[".wav"] = "audio/x-wav";
	types[".weba"] = "audio/webm";
	types[".webm"] = "video/webm";
	types[".webp"] = "image/webp";
	types[".woff"] = "application/x-font-woff";
	types[".xhtml"] = "application/xhtml+xml";
	types[".xls"] = "application/vnd.ms-excel";
	types[".xml"] = "application/xml";
	types[".xul"] = "application/vnd.mozilla.xul+xml";
	types[".zip"] = "application/zip";
	types[".3gp"] = "video/3gpp audio/3gpp";
	types[".3g2"] = "video/3gpp2 audio/3gpp2";
	types[".7z"] = "application/x-7z-compressed";
}