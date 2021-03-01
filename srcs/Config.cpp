#include "Config.hpp"
#include <sys/stat.h>
#include <climits>

Config::Config() : maxBody(0), cgi_path()
{
	this->locationMatch = "";
}

Config::Config(std::string &location_match) : maxBody(0), cgi_path()
{
	this->locationMatch = location_match;
	this->autoIndex = "off";
}

Config::~Config()
{
	loginfo.clear();
}

Config::Config(const Config &x) : maxBody(0)
{
	*this = x;
}

Config &Config::operator=(const Config &x)
{
	if (this != &x)
	{
		this->root = x.root;
		this->locationMatch = x.locationMatch;
		this->autoIndex = x.autoIndex;
		this->allowedMethod = x.allowedMethod;
		this->indexes = x.indexes;
		this->errorPage = x.errorPage;
		this->maxBody = x.maxBody;
		this->cgiAllowedExtensions = x.cgiAllowedExtensions;
		this->cgi_path = x.cgi_path;
		this->php_cgi = x.php_cgi;
		this->auth_basic_realm = x.auth_basic_realm;
		this->htPasswdPath = x.htPasswdPath;
		this->loginfo = x.loginfo;
	}
	return *this;
}

std::vector<method_w> stringToMethod(const std::vector<std::string> &in)
{
	std::vector<method_w> out;
	for (std::vector<std::string>::const_iterator it = in.begin(); it != in.end(); it++)
	{
		if (*it == "HEAD")
			out.push_back(HEAD);
		else if (*it == "GET")
			out.push_back(GET);
		else if (*it == "POST")
			out.push_back(POST);
		else if (*it == "PUT")
			out.push_back(PUT);
		else throw std::runtime_error("invalid method");
	}
	return (out);
}

void Config::setAutoIndex(const std::string &autoIndex)
{ this->autoIndex = autoIndex; }

void Config::setAllowMethod(const std::string &method)
{ this->allowedMethod = stringToMethod(ft::split(method, " \t\r\n\v\f")); }

void Config::setIndex(const std::string &index)
{ this->indexes = ft::split(index, " \t\r\n\v\f"); }

void Config::setCgiAllowedExtensions(const std::string &extentions)
{ this->cgiAllowedExtensions = ft::split(extentions, " \t\r\n\v\f"); }

void Config::setErrorPage(const std::string &errorPage)
{ this->errorPage = errorPage; }

void Config::setMaxBody(const std::string &maxBody)
{ this->maxBody = ft::atoi(maxBody.c_str()); }

void Config::setDefaultCgiPath(const std::string &path)
{
	struct stat statstruct = {};
	if (stat(path.c_str(), &statstruct) != -1)
		this->cgi_path = path;
}

void Config::setRoot(const std::string &root)
{
	struct stat statstruct = {};
	this->root = root;
	if (stat(root.c_str(), &statstruct) == -1)
		throw std::runtime_error("Корневая папка root не существует или путь не корректен.");
}


void Config::setPhpCgiPath(const std::string &path)
{
	struct stat statstruct = {};
	this->php_cgi = path;
	if (stat(root.c_str(), &statstruct) == -1)
		throw std::runtime_error("Путь к исполняемому файлу php-cgi не существует или не корректен.");
}

std::string Config::getRoot() const
{ return this->root; }

std::string Config::getAutoIndex() const
{ return this->autoIndex; }

std::string Config::getlocationmatch() const
{ return this->locationMatch; }

std::vector<std::string> Config::getIndexes() const
{ return this->indexes; }

std::vector<std::string> Config::getCgiAllowedExtensions() const
{ return this->cgiAllowedExtensions; }

std::string Config::getErrorPage() const
{ return this->getRoot() + '/' + this->errorPage; }

long unsigned int Config::getMaxBody() const
{ return this->maxBody; }

std::string Config::getIndex() const
{
	struct stat statstruct = {};
	for (size_t i = 0; i < this->indexes.size(); i++)
	{
		std::string check = this->root + '/' + this->indexes[i];
		if (stat(check.c_str(), &statstruct) != -1)
		{
			return this->indexes[i];
		}
	}
	return "";
}

std::string Config::getDefaultCgiPath() const
{ return this->cgi_path; }

std::string Config::getPhpCgiPath() const
{ return this->php_cgi; }

std::string Config::getMethods() const
{
	std::string ret("Allow:");
	for (size_t i = 0; i < this->allowedMethod.size(); ++i)
	{
		if (i > 0)
			ret += ',';
		ret += " " + ft::methodToString(this->allowedMethod[i]);
	}
	return (ret);
}

bool Config::isMethodAllowed(const method_w &meth) const
{
	for (std::vector<method_w>::const_iterator it = this->allowedMethod.begin(); it != this->allowedMethod.end(); ++it)
	{
		if (*it == meth)
			return true;
	}
	return false;
}

void Config::setup(int fd)
{
	std::map<std::string, void (Config::*)(const std::string &)> m;
	m["root"] = &Config::setRoot;
	m["autoindex"] = &Config::setAutoIndex;
	m["allow_method"] = &Config::setAllowMethod;
	m["index"] = &Config::setIndex;
	m["ext"] = &Config::setCgiAllowedExtensions;
	m["error_page"] = &Config::setErrorPage;
	m["maxBody"] = &Config::setMaxBody;
	m["default_cgi"] = &Config::setDefaultCgiPath;
	m["php-cgi"] = &Config::setPhpCgiPath;
	m["auth_basic"] = &Config::setauth_basic;
	m["auth_basic_user_file"] = &Config::setHtPasswdPath;
	std::string str;

	while (ft::get_next_line(fd, str) > 0)
	{
		std::string key, value;
		if (str.empty() || ft::is_first_char(str) || ft::is_first_char(str, ';')) // checks for comments or empty lines
			continue;
		if (ft::is_first_char(str, '}')) // End of location block
			break;
		ft::get_key_value(str, key, value);
		if (!m.count(key))
			std::cerr << "Unable to parse key '" << key << "' in Config block " << this->getlocationmatch() << std::endl;
		(this->*(m.at(key)))(value);
	}
}

void Config::setauth_basic(const std::string &realm)
{
	this->auth_basic_realm = realm;
}

std::string Config::getAuthBasicRealm() const
{
	return this->auth_basic_realm;
}

void Config::setHtPasswdPath(const std::string &path)
{
	struct stat statstruct = {};
	if (stat(path.c_str(), &statstruct) == -1)
		return;

	this->htPasswdPath = path;
	int htpasswd_fd = open(this->htPasswdPath.c_str(), O_RDONLY);
	if (htpasswd_fd < 0)
		return;
	std::string line;
	while (ft::get_next_line(htpasswd_fd, line))
	{
		std::string user, pass;
		ft::get_key_value(line, user, pass, ":");
		this->loginfo[user] = pass;
	}
	if (close(htpasswd_fd) == -1)
		throw std::runtime_error("Failed to close .htpasswd file");
}

std::string Config::getHtPasswdPath() const
{
	return this->htPasswdPath;
}

bool Config::getMatch(const std::string &username, const std::string &passwd)
{
	std::map<std::string, std::string>::const_iterator it = this->loginfo.find(username);

	return (it != loginfo.end() && passwd == ft::base64_decode(it->second));
}

bool Config::isExtensionAllowed(const std::string &uri) const
{
	std::string extension = ft::getextension(uri);

	for (std::vector<std::string>::const_iterator it = cgiAllowedExtensions.begin(); it != cgiAllowedExtensions.end(); ++it)
	{
		if (extension == *it)
			return (true);
	}
	return (false);
}

std::ostream &operator<<(std::ostream &o, const Config &x)
{
	std::vector<std::string> v;
	std::vector<method_w> meths;
	o << "Config block \"" << x.getlocationmatch() << "\":" << std::endl
	  << "\troot folder: \"" << x.getRoot() << "\"" << std::endl
	  << "\tautoindex is: \"" << x.getAutoIndex() << "\"" << std::endl;
	o << '\t' << x.getMethods() << std::endl;
	o << "\tindexes:";
	v = x.getIndexes();
	for (size_t i = 0; i < v.size(); i++)
		o << " \"" << v[i] << "\"";
	o << std::endl;
	o << "\tcgi:";
	v = x.getCgiAllowedExtensions();
	for (size_t i = 0; i < v.size(); i++)
		o << " \"" << v[i] << "\"";
	o << std::endl;
	return o;
}
