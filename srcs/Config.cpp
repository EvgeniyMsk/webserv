#include "Config.hpp"

Config::Config() : maxBody(0), default_cgi_path()
{
	this->location_match = "";
}

Config::Config(std::string& location_match) : maxBody(0), default_cgi_path()
{
	this->location_match = location_match;
	this->autoindex = "off";
}

Config::~Config()
{
	loginfo.clear();
}

Config::Config(const Config& config) : maxBody(0)
{
	*this = config;
}

Config &Config::operator=(const Config& config)
{
	if (this != &config)
	{
		this->root = config.root;
		this->location_match = config.location_match;
		this->autoindex = config.autoindex;
		this->allow_method = config.allow_method;
		this->indexes = config.indexes;
		this->error_page = config.error_page;
		this->maxBody = config.maxBody;
		this->cgi_allowed_extensions = config.cgi_allowed_extensions;
		this->default_cgi_path = config.default_cgi_path;
		this->php_cgi = config.php_cgi;
		this->auth_basic_realm = config.auth_basic_realm;
		this->htpasswd_path = config.htpasswd_path;
		this->loginfo = config.loginfo;
	}
	return *this;
}

std::vector<method_w> stringToMethod(const std::vector<std::string>& strings)
{
	std::vector<method_w> result;
	for (std::vector<std::string>::const_iterator it = strings.begin(); it != strings.end(); it++)
	{
		if (*it == "HEAD")
			result.push_back(HEAD);
		else if (*it == "GET")
			result.push_back(GET);
		else if (*it == "POST")
			result.push_back(POST);
		else if (*it == "PUT")
			result.push_back(PUT);
		else throw std::runtime_error("invalid method");
	}
	return (result);
}

void Config::setAutoIndex(const std::string& in) { this->autoindex = in; }

void Config::setAllowMethod(const std::string& in) { this->allow_method = ft::stringToMethod(ft::split(in, " \t\r\n\v\f")); }

void Config::setIndex(const std::string& in) { this->indexes = ft::split(in, " \t\r\n\v\f"); }

void Config::setCgiAllowedExtensions(const std::string& in) { this->cgi_allowed_extensions = ft::split(in, " \t\r\n\v\f"); }

void Config::setErrorPage(const std::string& in) { this->error_page = in; }

void Config::setMaxBody(const std::string& in) { this->maxBody = ft::atoi(in.c_str()); }

void Config::setDefaultCgiPath(const std::string& in)
{
	struct stat statstruct = {};
	if (stat(in.c_str(), &statstruct) != -1)
		this->default_cgi_path = in;
}

void Config::setRoot(const std::string& in)
{
	struct stat statstruct = {};
	this->root = in;
	if (stat(root.c_str(), &statstruct) == -1)
		throw std::runtime_error("location root folder does not exist.");
}

void Config::setPhpCgiPath(const std::string& in)
{
	struct stat statstruct = {};
	this->php_cgi = in;
	if (stat(root.c_str(), &statstruct) == -1)
		throw std::runtime_error("php-cgi executable path does not exist.");
}

std::string	Config::getRoot() const { return this->root; }

std::string	Config::getAutoIndex() const { return this->autoindex; }

std::string	Config::getLocationMatch() const { return this->location_match; }

std::vector<std::string> Config::getIndexes() const { return this->indexes; }

std::vector<std::string> Config::getCgiAllowedExtensions() const { return this->cgi_allowed_extensions; }

std::string	Config::getErrorPage() const { return this->getRoot() + '/' + this->error_page; }

long unsigned int Config::getMaxBody() const { return this->maxBody; }

std::string	Config::getIndex() const
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

std::string	Config::getDefaultCgiPath() const { return this->default_cgi_path; }

std::string	Config::getPhpCgiPath() const { return this->php_cgi; }

std::string	Config::getAllowedMethods() const
{
	std::string ret("Allow:");
	for (size_t i = 0; i < this->allow_method.size(); ++i)
	{
		if (i > 0)
			ret += ',';
		ret += " " + ft::methodToString(this->allow_method[i]);
	}
	return (ret);
}

bool Config::checkIfMethodAllowed(const method_w& meth) const
{
	for (std::vector<method_w>::const_iterator it = this->allow_method.begin(); it != this->allow_method.end(); ++it)
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
	m["auth_basic"] = &Config::setAllowMethod;
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
			std::cerr << _RED _BOLD << "Unable to parse key '" << key << "' in Location block " << this->getLocationMatch() << _END << std::endl;
		(this->*(m.at(key)))(value);
	}
}

void Config::setAuthBasicRealm(const std::string &realm)
{
	this->auth_basic_realm = realm;
}

std::string	Config::getAuthBasicRealm() const
{
	return this->auth_basic_realm;
}

void Config::setHtPasswdPath(const std::string &path)
{
	struct stat statstruct = {};
	if (stat(path.c_str(), &statstruct) == -1)
		return;

	this->htpasswd_path = path;
	int htpasswd_fd = open(this->htpasswd_path.c_str(), O_RDONLY);
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

std::string	Config::getHtPasswdPath() const
{
	return this->htpasswd_path;
}

bool Config::getmatch(const std::string& username, const std::string& passwd)
{
	std::map<std::string, std::string>::const_iterator it = this->loginfo.find(username);

	return (it != loginfo.end() && passwd == ft::base64_decode(it->second));
}

bool Config::isExtensionAllowed(const std::string& uri) const
{
	std::string extension = ft::getextension(uri);

	for (std::vector<std::string>::const_iterator it = cgi_allowed_extensions.begin(); it != cgi_allowed_extensions.end(); ++it)
	{
		if (extension == *it)
			return (true);
	}
	return (false);
}

std::ostream&	operator<<(std::ostream& os, const Config& config)
{
	std::vector<std::string> v;
	std::vector<method_w> meths;
	os << "Location block \"" << config.getLocationMatch() << "\":" << std::endl
	  << "root folder: \"" << config.getRoot() << "\"" << std::endl
	  << "autoindex is: \"" << config.getAutoIndex() << "\"" << std::endl;
	os << config.getAllowedMethods() << std::endl;
	os << "indexes:";
	v = config.getIndexes();
	for (size_t i = 0; i < v.size(); i++)
		os << " \"" << v[i] << "\"";
	os << std::endl;
	os << "cgi:";
	v = config.getCgiAllowedExtensions();
	for (size_t i = 0; i < v.size(); i++)
		os << " \"" << v[i] << "\"";
	os << std::endl;
	return os;
}