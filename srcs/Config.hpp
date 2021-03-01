#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <map>
# include <vector>
# include "Utils.hpp"

class Config
{
public:
	friend class Server;

	Config();

	explicit Config(std::string &config);

	~Config();

	Config(const Config &);

	Config &operator=(const Config &config);

private:
	void setRoot(const std::string &);

	void setAutoIndex(const std::string &);

	void setAllowMethod(const std::string &);

	void setIndex(const std::string &);

	void setCgiAllowedExtensions(const std::string &);

	void setErrorPage(const std::string &);

	void setMaxBody(const std::string &);

	void setDefaultCgiPath(const std::string &);

	void setPhpCgiPath(const std::string &);

	void setauth_basic(const std::string &);

	void setHtPasswdPath(const std::string &);


public:
	std::string getRoot() const;

	std::string getAutoIndex() const;

	std::string getlocationmatch() const;

	std::string getMethods() const;

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

	bool isMethodAllowed(const method_w &meth) const;

	bool getMatch(const std::string &username, const std::string &passwd);

	bool isExtensionAllowed(const std::string &uri) const;

private:
	std::string root;
	std::string autoIndex;
	std::string locationMatch;
	std::string errorPage;
	std::string auth_basic_realm;
	std::string htPasswdPath;
	std::vector<std::string> indexes;
	std::vector<std::string> cgiAllowedExtensions;
	std::vector<method_w> allowedMethod;
	long unsigned int maxBody;
	std::string cgi_path;
	std::string php_cgi;
	std::map<std::string, std::string> loginfo;
};

std::ostream &operator<<(std::ostream &o, const Config &x);

#endif
