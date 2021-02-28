#include "Utils.hpp"
#include "Request.hpp"
#include <algorithm>
#include <stdexcept>
#include <cstring>

Request::Request() : method()
{
	statusCode = 0;
	methodMap["GET"] = GET;
	methodMap["HEAD"] = HEAD;
	methodMap["POST"] = POST;
	methodMap["PUT"] = PUT;
	headerMap["ACCEPT-CHARSET"] = ACCEPT_CHARSET;
	headerMap["ACCEPT-LANGUAGE"] = ACCEPT_LANGUAGE;
	headerMap["ALLOW"] = ALLOW;
	headerMap["AUTHORIZATION"] = AUTHORIZATION;
	headerMap["CONNECTION"] = CONNECTION;
	headerMap["CONTENT-LANGUAGE"] = CONTENT_LANGUAGE;
	headerMap["CONTENT-LENGTH"] = CONTENT_LENGTH;
	headerMap["CONTENT-LOCATION"] = CONTENT_LOCATION;
	headerMap["CONTENT-TYPE"] = CONTENT_TYPE;
	headerMap["DATE"] = DATE;
	headerMap["HOST"] = HOST;
	headerMap["LAST-MODIFIED"] = LAST_MODIFIED;
	headerMap["LOCATION"] = LOCATION;
	headerMap["REFERER"] = REFERER;
	headerMap["RETRY-AFTER"] = RETRY_AFTER;
	headerMap["SERVER"] = SERVER;
	headerMap["TRANSFER-ENCODING"] = TRANSFER_ENCODING;
	headerMap["USER-AGENT"] = USER_AGENT;
	headerMap["WWW-AUTHENTICATE"] = WWW_AUTHENTICATE;
}

Request::~Request()
{
	uri.clear();
	headers.clear();
	methodMap.clear();
	headerMap.clear();
	rawRequest.clear();
	env.clear();
}

Request::Request(const Request &obj) : statusCode(), method()
{
	*this = obj;
}

Request& Request::operator= (const Request &obj)
{
	if (this != &obj)
	{
		this->method = obj.method;
		this->uri = obj.uri;
		this->version = obj.version;
		this->headers = obj.headers;
		this->methodMap = obj.methodMap;
		this->headerMap = obj.headerMap;
		this->rawRequest = obj.rawRequest;
		this->env = obj.env;
	}
	return *this;
}


void Request::parseBody(request_s& req)
{
	size_t startpos = 0, endpos;
	size_t lastrn = rawRequest.rfind("\r\n");

	while (startpos < lastrn)
	{
		startpos = rawRequest.find("\r\n", startpos) + 2;
		endpos = rawRequest.find("\r\n", startpos);
		if (startpos == std::string::npos || endpos == std::string::npos || startpos > endpos)
		{
			break;
		}
		req.body.append(rawRequest, startpos, endpos - startpos);
		startpos = endpos + 1;
	}
}

request_s Request::parseHeadersOnly(const std::string &req)
{
	request_s request;
	rawRequest = req;
	headers.clear();
	statusCode = 200;

	try
	{
		parseRequestLine();
		parseHeaders();
	} catch (std::exception &e)
	{
		std::cerr << _RED _BOLD "Bad request!\n";
	}
	request.headers = headers;
	request.method = method;
	request.version = version;
	size_t n = uri.find_first_of('?');
	request.uri = uri.substr(0, n);
	if (n != std::string::npos)
		request.cgiparams = uri.substr(uri.find('?'));
	if (statusCode == 200 && request.uri.find("..") != std::string::npos)
		statusCode = 400;
	if (statusCode == 200 && uri.length() > 1000)
		statusCode = 414;
	request.env = env;
	return (request);
}

request_s Request::parseRequest(const std::string &req)
{
	this->env.clear();
	request_s request = parseHeadersOnly(req);

	if (request.headers.find(TRANSFER_ENCODING) != request.headers.end())
		parseBody(request);
	else
		request.body = rawRequest.substr(0, rawRequest.length() - 2);

	request.status_code = statusCode;
	return (request);
}

void Request::parseRequestLine()
{
	size_t eoRequestLine = rawRequest.find("\r\n", 0);
	size_t pos = 0;
	size_t pos2 = 0;

	if (rawRequest[0] == ' ' && statusCode == 200)
	{
		std::cerr << "BAD REQ 1" << std::endl;
		statusCode = 400;
	}
	if (rawRequest.find("\r\n", 0) == std::string::npos && statusCode == 200)
	{
		std::cerr << "BAD REQ 2" << std::endl;
		statusCode = 400;
	}
	int numSpaces = 0;
	int doubleSpace = 0;
	for (int i = 0; rawRequest[i] != '\r'; i++)
	{
		if (rawRequest[i] == ' ')
			numSpaces++;
		if (rawRequest[i] == ' ' && rawRequest[i + 1] == ' ')
			doubleSpace = 1;
	}
	if ((numSpaces != 2 || doubleSpace == 1) && statusCode == 200)
	{
		std::cerr << "BAD REQ 3" << std::endl;
		statusCode = 400;
	}
	extractMethod(eoRequestLine, pos);
	pos++;
	extractUri(eoRequestLine, pos, pos2);
	pos++;
	if (uri.length() > 10000000 && statusCode == 200)
	{
		std::cerr << "BAD REQ 3.1" << std::endl;
		statusCode = 414;
	}
	extractVersion(eoRequestLine, pos, pos2);
	rawRequest.erase(0, pos + 5);
}

void Request::extractVersion(size_t eoRequestLine, size_t &pos, size_t &pos2)
{
	int mainVersion;
	int subVersion;
	std::string ret;

	pos = rawRequest.find("HTTP/", pos);
	if (pos > eoRequestLine && statusCode == 200)
	{
		std::cerr << "BAD REQ 3.2" << std::endl;
		statusCode = 400;
	}
	pos += 5;
	pos2 = rawRequest.find("\r\n", pos);
	if (pos2 > eoRequestLine && statusCode == 200)
	{
		std::cerr << "BAD REQ 4" << std::endl;
		statusCode = 400;
	}
	ret = rawRequest.substr(pos, pos2 - pos);
	mainVersion = ft::atoi(ret.c_str());
	subVersion = ft::atoi(ret.c_str() + 2);
	version = std::make_pair(mainVersion, subVersion);
}

void Request::extractUri(size_t eoRequestLine, size_t pos, size_t pos2)
{
	std::string ret;
	pos2 = rawRequest.find(' ', pos);
	if (pos2 > eoRequestLine && statusCode == 200)
	{
		std::cerr << "BAD REQ 5" << std::endl;
		statusCode = 400;
	}
	ret = rawRequest.substr(pos, pos2 - pos);
	uri = ret;
	if (uri[0] == ':' && statusCode == 200)
	{
		std::cerr << "BAD REQ 5.1" << std::endl;
		statusCode = 400;
	}
}

void Request::extractMethod(size_t eoRequestLine, size_t& pos)
{
	std::string ret;

	pos = rawRequest.find(' ', 0);
	if (pos > eoRequestLine && statusCode == 200)
	{
		std::cerr << "BAD REQ 6" << std::endl;
		statusCode = 400;
	}
	ret = rawRequest.substr(0, pos);
	std::map<std::string, method_w>::iterator it = methodMap.find(ret);
	if (it != methodMap.end())
	{
		method = it->second;
	} else if (statusCode == 200)
	{
		std::cerr << "BAD REQ 7: cant find method (" << ret << ") in methodMap" << std::endl;
		statusCode = 400;
	}
}

void Request::parseHeaders()
{
	std::string upperHeader;
	size_t eoRequestLine;
	int owsOffset;
	while (!rawRequest.empty())
	{
		upperHeader.clear();
		eoRequestLine = rawRequest.find("\r\n", 0);
		if (eoRequestLine != 0 && eoRequestLine != std::string::npos)
		{
			if (rawRequest[0] == ' ' && statusCode == 200)
			{
				std::cerr << "BAD REQ 8" << std::endl;
				statusCode = 400;
			}
			size_t pos = rawRequest.find(':', 0);
			if (pos > eoRequestLine && statusCode == 200)
			{
				std::cerr << "BAD REQ 9" << std::endl;
				statusCode = 400;
			}
			std::string header = rawRequest.substr(0, pos);
			if (header.empty() && statusCode == 200)
			{
				std::cerr << "BAD REQ 10.1" << std::endl;
				statusCode = 400;
			}
			if (header.find(' ') != std::string::npos && statusCode == 200)
			{
				std::cerr << "BAD REQ 10" << std::endl;
				statusCode = 400;
			}
			pos++;
			// 'Skip' over OWS at beginning of value string
			pos = rawRequest.find_first_not_of(' ', pos);
			owsOffset = 0;
			// Create offset for OWS at end of value string
			for (size_t i = eoRequestLine - 1; i != std::string::npos && rawRequest[i] == ' '; --i)
			{
				owsOffset++;
			}
			// Extract value string and check if not empty or beginning with newline
			std::string value = rawRequest.substr(pos, eoRequestLine - pos - owsOffset);
			if ((value.empty() || rawRequest[pos] == '\r') && statusCode == 200)
			{
				std::cerr << "BAD REQ 11" << std::endl;
				statusCode = 400;
			}
			for (int i = 0; header[i]; i++)
			{
				upperHeader += std::toupper(header[i]);
			}
			// Match found header to correct headerType using map
			std::map<std::string, header_w>::iterator it = headerMap.find(upperHeader);
			if (it != headerMap.end())
			{
				if (headers.find(it->second) != headers.end() && statusCode == 200)
				{
					std::cerr << "BAD REQ 12" << std::endl;
					statusCode = 400;
				}
				headers.insert(std::make_pair(it->second, value));
			}
			this->AddHeaderToEnv(upperHeader, value);
		} else
		{
			eoRequestLine = rawRequest.find("\r\n", 0);
			if (eoRequestLine == std::string::npos && statusCode == 200)
			{
				std::cerr << "2. BAD REQ 7: cant find '\\r\\n' in rawRequest" << std::endl;
				statusCode = 400;
			}
			rawRequest.erase(0, eoRequestLine + 2);
			return;
		}
		rawRequest.erase(0, eoRequestLine + 2);
	}
}

const std::map<header_w, std::string>& Request::getHeaders() const
{
	return headers;
}

method_w Request::getMethod() const
{
	return method;
}

const std::string& Request::getUri() const
{
	return uri;
}

void Request::setRawRequest(const std::string& rawRequest)
{
	this->rawRequest = rawRequest;
}

const std::pair<int, int>& Request::getVersion() const
{
	return version;
}

void Request::AddHeaderToEnv(const std::string &upperHeader, const std::string &value)
{
	std::string insert("HTTP_");
	insert.append(upperHeader);
	std::replace(insert.begin(), insert.end(), '-', '_');
	if (this->env.count(insert) == 0)
		this->env[insert] = value;
}

request_s::request_s() : status_code(200), uri(), cgiparams(), server(), config(), transfer_buffer(), cgi_ran(false)
{

}

request_s::request_s(const request_s &x) : status_code(), server(), config(), transfer_buffer(), cgi_ran(false)
{
	*this = x;
}

request_s &request_s::operator=(const request_s &x)
{
	if (this != &x)
	{
		status_code = x.status_code;
		method = x.method;
		uri = x.uri;
		cgiparams = x.cgiparams;
		version = x.version;
		headers = x.headers;
		server = x.server;
		config = x.config;
		body = x.body;
		transfer_buffer = x.transfer_buffer;
		cgi_ran = x.cgi_ran;
		env = x.env;
	}
	return *this;
}

request_s::~request_s()
{
	this->headers.clear();
	this->body.clear();
	this->env.clear();
	this->server = nullptr;
	this->config = nullptr;
}

std::string request_s::MethodToSTring() const
{
	switch (this->method)
	{
		case GET:
			return "GET";
		case HEAD:
			return "HEAD";
		case POST:
			return "POST";
		case PUT:
			return "PUT";
		default:
			return "NOMETHOD";
	}
}

void request_s::clear()
{
	this->status_code = 200;
	this->method = ERROR;
	this->uri.clear();
	this->cgiparams.clear();
	this->headers.clear();
	this->server = NULL;
	this->config = NULL;
	this->body.clear();
	this->transfer_buffer = false;
	this->cgi_ran = false;
	this->env.clear();
}

std::ostream&	operator<<(std::ostream& o, const request_s& r)
{
	o << _CYAN
	  << "uri: " << r.uri << std::endl
	  << "method: " << r.MethodToSTring() << std::endl
	  << "status_code: " << r.status_code << std::endl
	  << "HEADERS:" << std::endl;
	for (std::map<header_w, std::string>::const_iterator it = r.headers.begin(); it != r.headers.end(); ++it)
		o << '\t' << ft::headerToString(it->first) << ": " << it->second << std::endl;
	o << _END << std::endl;
	return o;
}