#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"
# include <sys/socket.h>
# include <string>
# include <set>
# include <sstream>
# include <string>
# include <set>
# include <map>

class Response
{
public:
	Response(Request* request, int socket);
	~Response();
	void sendResponse();
	void generateGetResponse();
	void generateHeadResponse();
	void generatePutResponse();
	void generateResponse();
	static const std::set<std::string> implemented_headers;
	static const std::map<int, std::string> status_codes;
private:
	static std::map<int,std::string> initStatusCodes();
	static std::set<std::string> initResponseHeaders();
	Request* newRequest;
	int newSocket;
	std::string reason_phrase;
	std::map<std::string, std::string> headers;
	std::string raw_response;
	std::string content;
};

#endif
