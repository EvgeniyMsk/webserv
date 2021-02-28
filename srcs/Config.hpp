#ifndef CONFIG_HPP
#define CONFIG_HPP
#include "Utils.hpp"
#include <string>
#include <vector>
#include <map>

class Config
{
private:
	std::string	root;
	std::string	autoindex;
	std::string	location_match;
	std::string	error_page;
	std::string	auth_basic_realm;
	std::string	htpasswd_path;
	std::vector<std::string> indexes;
	std::vector<std::string> cgi_allowed_extensions;
	std::vector<method_w> allow_method;
	long unsigned int maxBody;
	std::string	default_cgi_path;
	std::string	php_cgi;
	std::map<std::string, std::string>	loginfo;
public:
	friend class Server;
	//coplien form
	Config();
	explicit Config(std::string& );
	virtual ~Config();
	Config(const Config &config);
	Config&	operator=(const Config &config);
};

#endif