#ifndef CONFIG_HPP
#define CONFIG_HPP
#include "Utils.hpp"
#include <string>
#include <vector>
#include <map>
#include <sys/stat.h>

class Config
{
private:
	std::string root;
	std::string autoindex;
	std::string location_match;
	std::string error_page;
	std::string auth_basic_realm;
	std::string htpasswd_path;
	std::vector<std::string> indexes;
	std::vector<std::string> cgi_allowed_extensions;
	std::vector<method_w> allow_method;
	long unsigned int maxBody;
	std::string default_cgi_path;
	std::string php_cgi;
	std::map<std::string, std::string> loginfo;
public:
	friend class Server;

	Config();

	explicit Config(std::string &);

	virtual ~Config();

	Config(const Config &config);

	Config &operator=(const Config &config);

	void setRoot(const std::string &);

	void setAutoIndex(const std::string &);

	void setAllowMethod(const std::string &);

	void setIndex(const std::string &);

	void setCgiAllowedExtensions(const std::string &);

	void setErrorPage(const std::string &);

	void setMaxBody(const std::string &);

	void setDefaultCgiPath(const std::string &);

	void setPhpCgiPath(const std::string &);

	void setAuthBasicRealm(const std::string &);

	void setHtPasswdPath(const std::string &);

	std::string getRoot() const;

	std::string getAutoIndex() const;

	std::string getLocationMatch() const;

	std::string getAllowedMethods() const;

	std::string getIndex() const;

	std::vector<std::string> getIndexes() const;

	std::vector<std::string> getCgiAllowedExtensions() const;

	std::string getErrorPage() const;

	std::string getDefaultCgiPath() const;

	long unsigned int getMaxBody() const;

	std::string getPhpCgiPath() const;

	std::string getAuthBasicRealm() const;

	std::string getHtPasswdPath() const;

	void setup(int);

	bool checkIfMethodAllowed(const method_w &meth) const;

	bool getmatch(const std::string &username, const std::string &passwd);

	bool isExtensionAllowed(const std::string &uri) const;
};

std::ostream&	operator<<(std::ostream &os, const Config &config);

#endif