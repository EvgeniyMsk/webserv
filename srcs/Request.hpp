#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>
#include "Utils.hpp"

class Server;

class Config;

struct request_s
{
	int status_code;
	method_w method;
	std::string uri;
	std::string cgiparams;
	std::pair<int, int> version;
	std::map<header_w, std::string> headers;
	Server *server;
	Config *location;
	std::string body;
	bool tempBuffer;
	bool cgi_ran;
	std::map<std::string, std::string> env;

	request_s();

	request_s(const request_s &x);

	request_s &operator=(const request_s &x);

	~request_s();

	std::string MethodToSTring() const;

	void clear();
};

std::ostream &operator<<(std::ostream &o, const request_s &r);

class Request
{
	int _status_code;
	method_w _method;
	std::string _uri;
	std::pair<int, int> _version;
	std::map<header_w, std::string> _headers;
	std::map<std::string, method_w> _methodMap;
	std::map<std::string, header_w> _headerMap;
	std::string _rawRequest;
	std::map<std::string, std::string> _env;

public:
	friend class Response;

	friend class Webserv;

	Request();

	virtual ~Request();

	Request(const Request &obj);

	Request &operator=(const Request &obj);

	request_s parseRequest(const std::string &req);

	void parseBody(request_s &req);

	void AddHeaderToEnv(const std::string &upperHeader, const std::string &value);

	request_s parseHeadersOnly(const std::string &req);

	void parseRequestLine();

	void parseHeaders();

	void setRawRequest(const std::string &rawRequest);

	void extractMethod(size_t eoRequestLine, size_t &pos);

	void extractUri(size_t eoRequestLine, size_t pos, size_t pos2);

	void extractVersion(size_t eoRequestLine, size_t &pos, size_t &pos2);

	method_w getMethod() const;

	const std::string &getUri() const;

	const std::pair<int, int> &getVersion() const;

	const std::map<header_w, std::string> &getHeaders() const;
};

#endif
