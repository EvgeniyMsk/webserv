#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"
# include "Utils.hpp"
# include <sys/socket.h>
# include <string>
# include <set>
# include <sstream>
# include <string>
# include <set>
# include <map>
# include <list>
# include <time.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <climits>
# include <unistd.h>


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

	void generateResponseByStatusCode();
	void generateStatusLine();
	void generateHeaders();
	void readFileToContent(std::string & filename);
	void generateAutoindex();
	void setContentTypeByFilename(std::string & filename);
	std::string getDateHeader();
	std::string getLastModifiedHeader(time_t tv_sec);
	std::string getLocationHeader();

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

	struct tm _getCalendarTime(time_t tv_sec);
	struct tm _gmtime(time_t tv_sec);
	std::string content_type;
	std::string allow;
	std::string last_modified;
	std::string location;
};

#endif
