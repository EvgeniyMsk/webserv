#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>
#include <vector>
# include <string>
# include <iostream>
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <map>
#  define BUFFER_SIZE 8


# define _GREY			"\x1b[30m"
# define _RED			"\x1b[31m"
# define _GREEN			"\x1b[32m"
# define _YELLOW		"\x1b[33m"
# define _BLUE			"\x1b[34m"
# define _PURPLE		"\x1b[35m"
# define _CYAN			"\x1b[36m"
# define _WHITE			"\x1b[37m"

/*
** This is a minimal set of ANSI/VT100 color codes
*/
# define _END			"\x1b[0m"
# define _BOLD			"\x1b[1m"
# define _UNDER			"\x1b[4m"
# define _REV			"\x1b[7m"

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
	std::string headerToString(const header_w &header);
	std::string methodToString(const method_w &method);
	std::vector<method_w> stringToMethod(const std::vector<std::string> &strings);
	std::vector<std::string> split(const std::string &s, const std::string &delim);
	std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len);
	std::string base64_decode(std::string const &encoded_string);
	std::string getextension(const std::string &uri);
	int get_next_line(int fd, std::string &line);
	int	atoi(const char *str);
	int	is_first_char(std::string str, char find = '#');
	void get_key_value(std::string &str, std::string &key, std::string& value, const char* delim = " \t\n", const char* end = "\n\r#;");
}

#endif