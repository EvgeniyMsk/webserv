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
#include <sys/time.h>
#  define BUFFER_SIZE 8

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
	long atol(const char *str);
	int	is_first_char(std::string str, char find = '#');
	void get_key_value(std::string &str, std::string &key, std::string& value, const char* delim = " \t\n", const char* end = "\n\r#;");
	std::string inttostring(int a);
	void stringtoupper(std::string& str);
	void trimstring(std::string& str, const char* totrim = " ");
	int findNthOccur(std::string str, char ch, int N);
	time_t getTime();
	void bzero(void *s, size_t n);
	void *calloc(size_t count, size_t size);
	char *strdup(const char *s1);
}

#endif
