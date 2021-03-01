#include "Request.hpp"
#include <algorithm>
#include <stdexcept>
#include <cstring>

Request::Request() : _method()
{
	_status_code = 0;
	_methodMap["GET"] = GET;
	_methodMap["HEAD"] = HEAD;
	_methodMap["POST"] = POST;
	_methodMap["PUT"] = PUT;
	_headerMap["ACCEPT-CHARSET"] = ACCEPT_CHARSET;
	_headerMap["ACCEPT-LANGUAGE"] = ACCEPT_LANGUAGE;
	_headerMap["ALLOW"] = ALLOW;
	_headerMap["AUTHORIZATION"] = AUTHORIZATION;
	_headerMap["CONNECTION"] = CONNECTION;
	_headerMap["CONTENT-LANGUAGE"] = CONTENT_LANGUAGE;
	_headerMap["CONTENT-LENGTH"] = CONTENT_LENGTH;
	_headerMap["CONTENT-LOCATION"] = CONTENT_LOCATION;
	_headerMap["CONTENT-TYPE"] = CONTENT_TYPE;
	_headerMap["DATE"] = DATE;
	_headerMap["HOST"] = HOST;
	_headerMap["LAST-MODIFIED"] = LAST_MODIFIED;
	_headerMap["LOCATION"] = LOCATION;
	_headerMap["REFERER"] = REFERER;
	_headerMap["RETRY-AFTER"] = RETRY_AFTER;
	_headerMap["SERVER"] = SERVER;
	_headerMap["TRANSFER-ENCODING"] = TRANSFER_ENCODING;
	_headerMap["USER-AGENT"] = USER_AGENT;
	_headerMap["WWW-AUTHENTICATE"] = WWW_AUTHENTICATE;
}

Request::~Request()
{
	_uri.clear();
	_headers.clear();
	_methodMap.clear();
	_headerMap.clear();
	_rawRequest.clear();
	_env.clear();
}

Request::Request(const Request &obj) : _status_code(), _method()
{
	*this = obj;
}

Request &Request::operator=(const Request &obj)
{
	if (this != &obj)
	{
		this->_method = obj._method;
		this->_uri = obj._uri;
		this->_version = obj._version;
		this->_headers = obj._headers;
		this->_methodMap = obj._methodMap;
		this->_headerMap = obj._headerMap;
		this->_rawRequest = obj._rawRequest;
		this->_env = obj._env;
	}
	return *this;
}


void Request::parseBody(request_s &req)
{
	size_t startpos = 0, endpos;
	size_t lastrn = _rawRequest.rfind("\r\n");

	while (startpos < lastrn)
	{
		startpos = _rawRequest.find("\r\n", startpos) + 2;
		endpos = _rawRequest.find("\r\n", startpos);
		if (startpos == std::string::npos || endpos == std::string::npos || startpos > endpos)
		{
			break;
		}
		req.body.append(_rawRequest, startpos, endpos - startpos);
		startpos = endpos + 1;
	}
}

request_s Request::parseHeadersOnly(const std::string &req)
{
	request_s request;
	_rawRequest = req;
	_headers.clear();
	_status_code = 200;

	try
	{
		parseRequestLine();
		parseHeaders();
	} catch (std::exception &e)
	{
		std::cerr << "Bad req!\n";
	}
	request.headers = _headers;
	request.method = _method;
	request.version = _version;
	size_t n = _uri.find_first_of('?');
	request.uri = _uri.substr(0, n);
	if (n != std::string::npos)
		request.cgiparams = _uri.substr(_uri.find('?'));
	if (_status_code == 200 && request.uri.find("..") != std::string::npos)
		_status_code = 400;
	if (_status_code == 200 && _uri.length() > 1000)
		_status_code = 414;
	request.env = _env;
	return (request);
}

request_s Request::parseRequest(const std::string &req)
{
	this->_env.clear();
	request_s request = parseHeadersOnly(req);

	if (request.headers.find(TRANSFER_ENCODING) != request.headers.end())
		parseBody(request);
	else
		request.body = _rawRequest.substr(0, _rawRequest.length() - 2);

	request.status_code = _status_code;
	return (request);
}

void Request::parseRequestLine()
{
	size_t eoRequestLine = _rawRequest.find("\r\n", 0);
	size_t pos = 0;
	size_t pos2 = 0;

	// Check if first char is space
	if (_rawRequest[0] == ' ' && _status_code == 200)
	{
		std::cerr << "BAD REQ 1" << std::endl; //TODO Decide if we want to keep this in 'production'
		_status_code = 400;
	}
	if (_rawRequest.find("\r\n", 0) == std::string::npos && _status_code == 200)
	{
		std::cerr << "BAD REQ 2" << std::endl;
		_status_code = 400;
	}
	// Check if there is 2 spaces in pline
	int numSpaces = 0;
	int doubleSpace = 0;
	for (int i = 0; _rawRequest[i] != '\r'; i++)
	{
		if (_rawRequest[i] == ' ')
			numSpaces++;
		if (_rawRequest[i] == ' ' && _rawRequest[i + 1] == ' ')
			doubleSpace = 1;
	}
	if ((numSpaces != 2 || doubleSpace == 1) && _status_code == 200)
	{
		std::cerr << "BAD REQ 3" << std::endl;
		_status_code = 400;
	}
	extractMethod(eoRequestLine, pos);
	pos++;
	extractUri(eoRequestLine, pos, pos2);
	pos++;
	if (_uri.length() > 10000000 && _status_code == 200)
	{
		std::cerr << "BAD REQ 3.1" << std::endl;
		_status_code = 414;
	}
	extractVersion(eoRequestLine, pos, pos2);

	_rawRequest.erase(0, pos + 5);
}

void Request::extractVersion(size_t eoRequestLine, size_t &pos, size_t &pos2)
{
	int mainVersion;
	int subVersion;
	std::string ret;

	pos = _rawRequest.find("HTTP/", pos);
	if (pos > eoRequestLine && _status_code == 200)
	{
		std::cerr << "BAD REQ 3.2" << std::endl;
		_status_code = 400;
	}
	pos += 5;
	pos2 = _rawRequest.find("\r\n", pos);
	if (pos2 > eoRequestLine && _status_code == 200)
	{
		std::cerr << "BAD REQ 4" << std::endl;
		_status_code = 400;
	}
	ret = _rawRequest.substr(pos, pos2 - pos);
	mainVersion = ft::atoi(ret.c_str());
	subVersion = ft::atoi(ret.c_str() + 2);
	_version = std::make_pair(mainVersion, subVersion);
}

void Request::extractUri(size_t eoRequestLine, size_t pos, size_t pos2)
{
	std::string ret;

	pos2 = _rawRequest.find(' ', pos);
	if (pos2 > eoRequestLine && _status_code == 200)
	{
		std::cerr << "BAD REQ 5" << std::endl;
		_status_code = 400;
	}
	ret = _rawRequest.substr(pos, pos2 - pos);
	_uri = ret;
	if (_uri[0] == ':' && _status_code == 200)
	{
		std::cerr << "BAD REQ 5.1" << std::endl;
		_status_code = 400;
	}
}

void Request::extractMethod(size_t eoRequestLine, size_t &pos)
{
	std::string ret;

	pos = _rawRequest.find(' ', 0);
	if (pos > eoRequestLine && _status_code == 200)
	{
		std::cerr << "BAD REQ 6" << std::endl;
		_status_code = 400;
	}
	ret = _rawRequest.substr(0, pos);
	std::map<std::string, method_w>::iterator it = _methodMap.find(ret);
	if (it != _methodMap.end())
	{
		_method = it->second;
	} else if (_status_code == 200)
	{
		std::cerr << "BAD REQ 7: cant find method (" << ret << ") in _methodMap" << std::endl;
		_status_code = 400;
	}
}

void Request::parseHeaders()
{
	std::string upperHeader;
	size_t eoRequestLine;
	int owsOffset;
	while (!_rawRequest.empty())
	{
		upperHeader.clear();
		eoRequestLine = _rawRequest.find("\r\n", 0);
		if (eoRequestLine != 0 && eoRequestLine != std::string::npos)
		{
			if (_rawRequest[0] == ' ' && _status_code == 200)
			{
				std::cerr << "BAD REQ 8" << std::endl;
				_status_code = 400;
			}
			size_t pos = _rawRequest.find(':', 0);
			if (pos > eoRequestLine && _status_code == 200)
			{
				std::cerr << "BAD REQ 9" << std::endl;
				_status_code = 400;
			}
			std::string header = _rawRequest.substr(0, pos);
			if (header.empty() && _status_code == 200)
			{
				std::cerr << "BAD REQ 10.1" << std::endl;
				_status_code = 400;
			}
			if (header.find(' ') != std::string::npos && _status_code == 200)
			{
				std::cerr << "BAD REQ 10" << std::endl;
				_status_code = 400;
			}
			pos++;
			pos = _rawRequest.find_first_not_of(' ', pos);
			owsOffset = 0;
			for (size_t i = eoRequestLine - 1; i != std::string::npos && _rawRequest[i] == ' '; --i)
			{
				owsOffset++;
			}
			std::string value = _rawRequest.substr(pos, eoRequestLine - pos - owsOffset);
			if ((value.empty() || _rawRequest[pos] == '\r') && _status_code == 200)
			{
				std::cerr << "BAD REQ 11" << std::endl;
				_status_code = 400;
			}
			for (int i = 0; header[i]; i++)
			{
				upperHeader += std::toupper(header[i]);
			}
			std::map<std::string, header_w>::iterator it = _headerMap.find(upperHeader);
			if (it != _headerMap.end())
			{
				if (_headers.find(it->second) != _headers.end() && _status_code == 200)
				{
					std::cerr << "BAD REQ 12" << std::endl;
					_status_code = 400;
				}
				_headers.insert(std::make_pair(it->second, value));
			}
			this->AddHeaderToEnv(upperHeader, value);
		} else
		{
			eoRequestLine = _rawRequest.find("\r\n", 0);
			if (eoRequestLine == std::string::npos && _status_code == 200)
			{
				std::cerr << "2. BAD REQ 7: cant find '\\r\\n' in _rawRequest" << std::endl;
				_status_code = 400;
			}
			_rawRequest.erase(0, eoRequestLine + 2);
			return;
		}
		_rawRequest.erase(0, eoRequestLine + 2);
	}
}

const std::map<header_w, std::string> &Request::getHeaders() const
{
	return _headers;
}

method_w Request::getMethod() const
{
	return _method;
}

const std::string &Request::getUri() const
{
	return _uri;
}

void Request::setRawRequest(const std::string &rawRequest)
{
	_rawRequest = rawRequest;
}

const std::pair<int, int> &Request::getVersion() const
{
	return _version;
}

void Request::AddHeaderToEnv(const std::string &upperHeader, const std::string &value)
{
	std::string insert("HTTP_");
	insert.append(upperHeader);
	std::replace(insert.begin(), insert.end(), '-', '_');
	if (this->_env.count(insert) == 0)
		this->_env[insert] = value;
}

request_s::request_s() : status_code(200), uri(), cgiparams(), server(), location(), tempBuffer(), cgi_ran(false)
{

}

request_s::request_s(const request_s &x) : status_code(), server(), location(), tempBuffer(), cgi_ran(false)
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
		location = x.location;
		body = x.body;
		tempBuffer = x.tempBuffer;
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
	this->server = NULL;
	this->location = NULL;
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
	this->server = nullptr;
	this->location = nullptr;
	this->body.clear();
	this->tempBuffer = false;
	this->cgi_ran = false;
	this->env.clear();
}

std::ostream &operator<<(std::ostream &o, const request_s &r)
{
	o << "uri: " << r.uri << std::endl
	  << "method: " << r.MethodToSTring() << std::endl
	  << "status_code: " << r.status_code << std::endl
	  << "HEADERS:" << std::endl;
	for (std::map<header_w, std::string>::const_iterator it = r.headers.begin(); it != r.headers.end(); ++it)
		o << '\t' << ft::headerToString(it->first) << ": " << it->second << std::endl;
	o << std::endl;
	return o;
}
