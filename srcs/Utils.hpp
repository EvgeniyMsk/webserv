#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>
#include <map>
#include <string>
#include <sys/time.h>
#include <iostream>
#include <iomanip>
#define OK 				"200 OK"
#define CREATED			"201 Created"
#define NOCONTENT		"204 No Content"
#define BADREQUEST		"400 Bad Request"
#define UNAUTHORIZED	"401 Unauthorized"
#define NOTFOUND 		"404 Not Found"
#define NOTALLOWED		"405 Method Not Allowed"
#define REQTOOLARGE		"413 Request Entity Too Large"
#define UNAVAILABLE		"503 Service Unavailable"
#define NOTIMPLEMENTED	"501 Not Implemented"
#define INTERNALERROR	"500 Internal Server Error"
#define BUFFER_SIZE 500

# define GREY			"\x1b[30m"
# define RED			"\x1b[41m"
# define GREEN			"\x1b[32m"
# define YELLOW		"\x1b[33m"
# define BLUE			"\x1b[34m"
# define PURPLE		"\x1b[35m"
# define CYAN			"\x1b[36m"
# define WHITE			"\x1b[37m"
# define END			"\x1b[0m"
# define UNDER			"\x1b[4m"

typedef std::map<std::string, std::string> element;
typedef std::map<std::string, element> Config;

struct Request
{
	bool isValid;
	std::string method;
	std::string uri;
	std::string version;
	std::map<std::string, std::string> headers;
	std::string body;

	void clear()
	{
		method.clear();
		uri.clear();
		version.clear();
		headers.clear();
		body.clear();
	}
};

struct Response
{
	std::string version;
	std::string statusCode;
	std::map<std::string, std::string> headers;
	std::string body;

	void clear()
	{
		version.clear();
		statusCode.clear();
		headers.clear();
		body.clear();
	}
};

struct w_chunk
{
	unsigned int length;
	bool isDone;
	bool isFound;
};

enum status
{
	BODYPARSING,
	CODE,
	HEADERS,
	CGI,
	BODY,
	RESPONSE,
	STANDBY,
	DONE
};

namespace ft
{
	bool isSpace(int c);

	void get_next_line(std::string &buffer, std::string &line);

	void get_next_line(std::string &buffer, std::string &line, char delChar);

	int pow(int n, int power);

	std::string getDate();

	void freeAll(char **args, char **env);
}

namespace utils
{
	void exitWithLog();

	void showMessage(std::string text);
}

#endif
