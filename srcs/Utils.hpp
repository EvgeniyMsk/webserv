#ifndef UTILS_HPP
#define UTILS_HPP

enum method_w
{
	GET, 
	HEAD,
	POST,
	PUT,
	ERROR
};

enum header_w
{
	ACCEPT_CHARSET,
	ACCEPT_LANGUAGE,
	ALLOW,
	AUTHORIZATION,
	CONNECTION,
	CONTENT_LANGUAGE,
	CONTENT_LENGTH,
	CONTENT_LOCATION,
	CONTENT_TYPE,
	DATE,
	HOST,
	LAST_MODIFIED,
	LOCATION,
	REFERER,
	REMOTE_USER,
	RETRY_AFTER,
	SERVER,
	TRANSFER_ENCODING,
	USER_AGENT,
	WWW_AUTHENTICATE
};	

namespace ft
{
	std::string headerToString(const header_w &header)
	{
		switch (header)
		{
			case ACCEPT_CHARSET : return ("ACCEPT_CHARSET");
			case ACCEPT_LANGUAGE : return ("ACCEPT_LANGUAGE");
			case ALLOW : return ("ALLOW");
			case AUTHORIZATION : return ("AUTHORIZATION");
			case CONNECTION : return ("CONNECTION");
			case CONTENT_LANGUAGE : return ("CONTENT_LANGUAGE");
			case CONTENT_LENGTH : return ("CONTENT_LENGTH");
			case CONTENT_LOCATION : return ("CONTENT_LOCATION");
			case CONTENT_TYPE : return ("CONTENT_TYPE");
			case DATE : return ("DATE");
			case HOST : return ("HOST");
			case LAST_MODIFIED : return ("LAST_MODIFIED");
			case LOCATION : return ("LOCATION");
			case REFERER : return ("REFERER");
			case REMOTE_USER : return ("REMOTE_USER");
			case RETRY_AFTER : return ("RETRY_AFTER");
			case SERVER : return ("SERVER");
			case TRANSFER_ENCODING : return ("TRANSFER_ENCODING");
			case USER_AGENT : return ("USER_AGENT");
			case WWW_AUTHENTICATE : return ("WWW_AUTHENTICATE");
			default: return ("INVALID HEADER");
		}
	}

	std::string methodToString(const method_w &method)
	{
		switch (method)
		{
			case GET : return ("GET");
			case HEAD : return ("HEAD");
			case POST : return ("POST");
			case PUT : return ("PUT");
			default : return ("INVALID METHOD");
		}
	}
}

#endif