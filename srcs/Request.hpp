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
	Config *config;
	std::string body;
	bool transfer_buffer;
	bool cgi_ran;

	std::map<std::string, std::string> env;

	request_s();

	request_s(const request_s &x);

	request_s &operator=(const request_s &x);

	~request_s();

	std::string MethodToSTring() const;

	void clear();
};

std::ostream&	operator<<(std::ostream& o, const request_s& r);

class Request
{
	int statusCode;
	method_w method;
	std::string uri;
	std::pair<int, int> version;
	std::map<header_w, std::string> headers;
	std::map<std::string, method_w> methodMap;
	std::map<std::string, header_w> headerMap;
	std::string rawRequest;
	std::map<std::string, std::string> env;

public:
	friend class ResponseHandler;

	friend class Connection;

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