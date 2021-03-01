#include "Response.hpp"
#include "Server.hpp"
#include <ctime>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

std::string getCurrentDatetime() {
	time_t		time;
	char 		datetime[100];
	tm*			curr_time;

	std::time(&time);
	curr_time = std::localtime(&time);
	std::strftime(datetime, 100, "%a, %d %B %Y %H:%M:%S GMT", curr_time);
	return datetime;
}

Response::Response() : _cgi_status_code(0), _autoindex(), _status_code() {
	_header_vals[ACCEPT_CHARSET].clear();
	_header_vals[ACCEPT_LANGUAGE].clear();
	_header_vals[ALLOW].clear();
	_header_vals[AUTHORIZATION].clear();
	_header_vals[CONNECTION] = "keep-alive";
	_header_vals[CONTENT_LANGUAGE].clear();
	_header_vals[CONTENT_LENGTH].clear();
	_header_vals[CONTENT_LOCATION].clear();
	_header_vals[CONTENT_TYPE].clear();
	_header_vals[DATE].clear();
	_header_vals[HOST].clear();
	_header_vals[LAST_MODIFIED].clear();
	_header_vals[LOCATION].clear();
	_header_vals[RETRY_AFTER].clear();
	_header_vals[SERVER] = "Webserv/1.0";
	_header_vals[TRANSFER_ENCODING].clear();
	_header_vals[WWW_AUTHENTICATE].clear();

	_status_codes[100] = "100 Continue\r\n";
	_status_codes[200] = "200 OK\r\n";
	_status_codes[201] = "201 Created\r\n";
	_status_codes[202] = "202 Accepted\r\n";
	_status_codes[203] = "203 Non-Authoritative Information\r\n";
	_status_codes[204] = "204 No Content\r\n";
	_status_codes[300] = "300 Multiple Choices\r\n";
	_status_codes[301] = "301 Moved Permanently\r\n";
	_status_codes[302] = "302 Found\r\n";
	_status_codes[307] = "307 Temporary Redirect\r\n";
	_status_codes[400] = "400 Bad Request\r\n";
	_status_codes[404] = "404 Not Found\r\n";
	_status_codes[405] = "405 Method Not Allowed\r\n";
	_status_codes[406] = "406 Not Acceptable\r\n";
	_status_codes[407] = "407 Proxy Authentication Required\r\n";
	_status_codes[408] = "408 Request Timeout\r\n";
	_status_codes[409] = "409 Conflict\r\n";
	_status_codes[410] = "410 Gone\r\n";
	_status_codes[411] = "411 Length Required\r\n";
	_status_codes[413] = "413 Request Entity Too Large\r\n";
	_status_codes[414] = "414 Request-url Too Long\r\n";
	_status_codes[415] = "415 Unsupported Media Type\r\n";
	_status_codes[500] = "500 Internal Server Error\r\n";
	_status_codes[501] = "501 Not Implemented\r\n";
	_status_codes[502] = "502 Bad Gateway\r\n";
	_status_codes[503] = "503 Service Unavailable\r\n";
	_status_codes[504] = "504 Gateway Timeout\r\n";
	_status_codes[505] = "505 HTTP Version Not Supported\r\n";
}

Response::~Response() {
	this->_header_vals.clear();
	this->_cgi_headers.clear();
	this->_status_codes.clear();
	this->_response.clear();
	this->_body.clear();
}

Response::Response(const Response &src) : _cgi_status_code(0), _autoindex(), _status_code() {
	*this = src;
}

Response& Response::operator= (const Response &rhs) {
	if (this != &rhs) {
		this->_header_vals = rhs._header_vals;
		this->_cgi_headers = rhs._cgi_headers;
		this->_cgi_status_code = rhs._cgi_status_code;
		this->_status_codes = rhs._status_codes;
		this->_response	= rhs._response;
		this->_body = rhs._body;
		this->CGI = rhs.CGI;
	}
	return *this;
}

int Response::generatePage(request_s& request) {
	int			fd = -1;
	struct stat statstruct = {};
	std::string filepath(request.location->getRoot());
	if (filepath[filepath.length() - 1] != '/')
		filepath.append("/");
	if (request.location->getlocationmatch() != request.uri)
		filepath.append(request.uri, request.uri.rfind('/') + 1);

	bool	validfile = stat(filepath.c_str(), &statstruct) == 0,
			allowed_extension = request.location->isExtensionAllowed(filepath);

	if ((request.uri.length() > 9 && request.uri.substr(0, 9) == "/cgi-bin/") || allowed_extension || (request.method == POST && !request.location->getDefaultCgiPath().empty())) {
		if (validfile && !S_ISDIR(statstruct.st_mode)) {
			fd = this->CGI.run_cgi(request, filepath, request.uri);
		}
		else if (!request.location->getDefaultCgiPath().empty() && request.method == POST) {
			std::string defaultcgipath = request.location->getDefaultCgiPath();
			fd = this->CGI.run_cgi(request, defaultcgipath, request.uri);
		}
	}
	else if (request.method == POST) {
		fd = handlePost(request, filepath, validfile);
	} else {
		fd = request.server->getpage(request.uri, _header_vals, this->_autoindex);
		if (fd == -1)
			request.status_code = 404;
	}
	return (fd);
}

void Response::extractCGIheaders(const std::string& incoming) {
	Request TMP;
	std::vector<std::string> vec = ft::split(incoming, "\n");
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it) {
		if ((*it).find(':') == std::string::npos)
			continue;
		std::string key, value;
		ft::get_key_value(*it, key, value, ":");
		ft::stringtoupper(key);
		ft::trimstring(value);
		std::map<std::string, header_w>::iterator header = TMP._headerMap.find(key);
		if (header != TMP._headerMap.end()) {
			_cgi_headers[header->second] = value;
		}
		else if (key == "STATUS")
			_cgi_status_code = ft::atoi(value.c_str());
	}
}

void Response::handle404(request_s& request) {
	int 	fd = open(request.server->geterrorpage().c_str(), O_RDONLY);
	int		ret = 1024;
	char	buf[1025];
	_status_code = 404;
	if (fd == -1) {
		_body += "Sorry, something went wrong in handling the error. whoops.\n";
		return;
	}
	while (ret == 1024) {
		ret = read(fd, buf, 1024);
		if (ret <= 0)
			break ;
		_body.append(buf, ret);
		memset(buf, 0, 1025);
	}
	close(fd);
}

void Response::handleAutoIndex(request_s& request) {
	DIR							*dir;
	char						cwd[2048];
	struct dirent				*entry;
	struct stat 				stats = {};
	struct tm					dt = {};
	std::string 				ss;
	std::string 				path;
	std::string					months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	std::string 				url = "http://";
	std::string					s;
	bool						valid_root = false;

	if (request.uri[request.uri.length() - 1] != '/')
		request.uri += "/";
	s = request.uri;
	if (getcwd(cwd, sizeof(cwd)) == NULL)
    	throw std::runtime_error("getcwd() error");
	else {
		path += cwd;
		path += "/";
	}
	path += request.server->getfilepath(request.uri);
	if (stat(path.c_str(), &stats) == 0 && S_ISDIR(stats.st_mode)) {
		valid_root = true;
	}
	if (!valid_root || (dir = opendir(path.c_str())) == NULL) {
		handle404(request);
		return ;
	}
	url += request.server->gethost() + ":" + ft::inttostring(request.server->getport());
	s = s.substr(0, ft::findNthOccur(s, '/', 2) + 1);

	_body += "<h1>Index of " + request.uri + "</h1><hr><pre><a href=\"" + url + s + "\">../</a><br>";

	while ((entry = readdir(dir)) != NULL) {
		ss = "";
		if (strncmp(entry->d_name, ".", 1) != 0 && strncmp(entry->d_name, "..", 2) != 0) {
			if (url[url.length()-1] == '/')
				url = url.substr(0, url.length()-1);
			_body += "<a href=\"" + url + request.uri + entry->d_name + "\">" + entry->d_name + "</a>";
			for (int i = std::string(entry->d_name).length(); i < 51; i++) {
				_body += " ";
			}
			if (stat((path + entry->d_name).c_str(), &stats) == 0) {
				dt = *(gmtime(&stats.st_ctime));
				if (dt.tm_mday < 10)
					ss += "0" + ft::inttostring(dt.tm_mday) + "-";
				else
					ss += ft::inttostring(dt.tm_mday) + "-";
				
				ss	+= months[dt.tm_mon] + "-"
					+ ft::inttostring(dt.tm_year + 1900) + " "
					+ ft::inttostring(dt.tm_hour) + ":"
					+ ft::inttostring(dt.tm_min) + ":"
					+ ft::inttostring(dt.tm_sec) + "\t\t\t";
				
				if (S_ISDIR(stats.st_mode))
					ss += std::string("-") + "<br>";
				else
					ss += ft::inttostring(stats.st_size) + "<br>";
				_body += ss;

			}
		}
	}
	_body += "</pre><hr>";
	closedir(dir);
}

void Response::handleBody(request_s& request) {
	char	buf[1024];
	int		ret = 1024,
			fd,
			totalreadsize = 0;

	_body.clear();
	if (request.status_code >= 400 && request.status_code < 500) {
		fd = open(request.location->getErrorPage().c_str(), O_RDONLY);
	} else {
		fd = generatePage(request);
		if (fd == -1) {
			fd = open(request.location->getErrorPage().c_str(), O_RDONLY);
			if (request.status_code == 200)
				request.status_code = 404;
		}
	}
	if (fd == -3) {
		this->handleAutoIndex(request);
	}
	else if (fd < 0) {
		_body = "Why are you here? Something obviously went wrong.\n";
		return;
	}
	else
	{
		while (ret == 1024) {
			ret = read(fd, buf, 1024);
			if (ret <= 0)
				break;
			totalreadsize += ret;
			_body.append(buf, ret);
			memset(buf, 0, 1024);
		}
		if (close(fd) == -1) {
			exit(EXIT_FAILURE);
		}
	}
	if (request.cgi_ran) {
		size_t pos = _body.find("\r\n\r\n");
		this->extractCGIheaders(_body.substr(0, pos + 4));
		if (pos != std::string::npos)
			_body.erase(0, pos + 4);
	}

}
std::string& Response::handleRequest(request_s& request) {
	this->_response.clear();
	_response = "HTTP/" + ft::inttostring(request.version.first) + '.' + ft::inttostring(request.version.second) + ' ';
	_body.clear();
	if (request.method == PUT)
		handlePut(request);
	else
		generateResponse(request);
	_body.clear();
	return _response;
}

void Response::handlePut(request_s& request) {
	struct stat statstruct = {};
	std::string filePath = request.server->getfilepath(request.uri);

	if (!request.location->isMethodAllowed(request.method)) {
		this->_response += _status_codes[405];
		handleBody(request);
		handleALLOW(request);
	}
	else if (request.body.length() > request.location->getMaxBody()) { // If body length is higher than location::maxBody
		this->_response += _status_codes[413];
		this->_header_vals[CONNECTION] = "close";
		handleBody(request);
	}
	else if (filePath[filePath.length() - 1] == '/' || (stat(filePath.c_str(), &statstruct) == 0 && S_ISDIR(statstruct.st_mode))) {
		this->_response += _status_codes[409];
		request.status_code = 409;
		handleLOCATION(filePath);
		handleBody(request);
	}
	else {
		if (stat(filePath.c_str(), &statstruct) == 0 && !S_ISDIR(statstruct.st_mode))
			request.status_code = 204;
		else
			request.status_code = 201;
		int fd = open(filePath.c_str(), O_TRUNC | O_CREAT | O_WRONLY, S_IRWXU);
		if (fd != -1) {
			this->_response += _status_codes[request.status_code];
			size_t WriteRet = write(fd, request.body.c_str(), request.body.length());
			if (close(fd) == -1)
				throw std::runtime_error("error closing putfile");
			if (WriteRet != request.body.length())
				throw std::runtime_error("Write return in Response::handlePut is not equal to req.body.length()");
			handleLOCATION(filePath);
		}
		else {
			this->_response += _status_codes[500];
		}
	}
	handleCONTENT_LENGTH();
	handleDATE();
	handleCONTENT_TYPE(request);
	handleCONNECTION_HEADER(request);

	this->_response += "\r\n";
	if (!_body.empty())
		this->_response += _body + "\r\n";
}

int Response::handlePost(request_s &request, std::string& filepath, bool validfile) {
	request.status_code = 200;
	if (!validfile)
		request.status_code = 201;
	int fd = open(filepath.c_str(), O_TRUNC | O_CREAT | O_WRONLY, S_IRWXU);
	if (fd == -1)
		return (fd);
	size_t writeret = write(fd, request.body.c_str(), request.body.length());
	if (close(fd) == -1 || writeret < request.body.length())
		return (-1);
	fd = open(filepath.c_str(), O_RDONLY);
	return (fd);
}

void Response::generateResponse(request_s& request) {
	try {
		if (request.server->getautoindex() == "on") {
			this->_autoindex = true;
			this->_autoindex_root.push_back("global");
		}
		else if (request.location->getAutoIndex() == "on") {
			this->_autoindex = true;
			this->_autoindex_root.push_back(request.location->getRoot());
		}
		if (!request.location->isMethodAllowed(request.method)) {
			request.status_code = 405;
			throw std::runtime_error("Method not allowed.");
		}
		if (this->authenticate(request))
			throw std::runtime_error("Authorization failed!");
		if (request.body.length() > request.location->getMaxBody()) 
		{
			request.status_code = 413;
			this->_header_vals[CONNECTION] = "close";
			throw std::runtime_error("Payload too large.");
		}
		negotiateLanguage(request);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	handleBody(request);
	handleStatusCode(request);
	handleCONTENT_TYPE(request);
	handleALLOW(request);
	handleDATE();
	handleCONTENT_LENGTH();
	handleCONTENT_LOCATION();
	handleCONTENT_LANGUAGE();
	handleSERVER();
	handleCONNECTION_HEADER(request);
	_response += "\r\n";
	if (request.method != HEAD && !_body.empty()) {
		_response += _body;
	}
	_body.clear();
}

int Response::authenticate(request_s& request) {
	if (request.location->getHtPasswdPath().empty()) {
		request.headers[AUTHORIZATION].clear();
		return 0;
	}
	std::string username, passwd, str;
	try {
		std::string auth = request.headers.at(AUTHORIZATION);
		std::string type, credentials;
		ft::get_key_value(auth, type, credentials);
		credentials = ft::base64_decode(credentials);
		ft::get_key_value(credentials, username, passwd, ":");
	}
	catch (std::exception& e) {
		std::cerr << "No credentials provided by client" << std::endl;
	}
	request.headers[AUTHORIZATION] = request.headers[AUTHORIZATION].substr(0, request.headers[AUTHORIZATION].find_first_of(' '));
	request.headers[REMOTE_USER] = username;
	if (request.location->getMatch(username, passwd)) {
		std::cout << "Authorization successful!" << std::endl;
		return 0;
	}

	request.status_code = 401;
	_response += "401 Unauthorized\r\n";
	this->_response +=	"Server: Webserv/1.1\r\n"
					  	"Content-Type: text/html\r\n"
	   					"WWW-Authenticate: Basic realm=";
	this->_response += request.location->getAuthBasicRealm();
	this->_response += ", charset=\"UTF-8\"\r\n";
	return 1;
}

void	Response::handleStatusCode(request_s& request) {
	if (request.status_code == 200 && _cgi_status_code)
		request.status_code = _cgi_status_code;
	if (request.version.first != 1 && request.status_code == 200)
		request.status_code = 505;
	_response += _status_codes[request.status_code];
}

void Response::handleALLOW(request_s& request) {
	_header_vals[ALLOW] = request.location->getMethods();
	_response += _header_vals[ALLOW];
	_response += "\r\n";
}

void Response::handleCONTENT_LANGUAGE() {
	std::string lang;
	size_t	found = _body.find("<html");
	size_t	lang_idx = _body.find("lang", found + 1);

	if (lang_idx != std::string::npos) {
		for (size_t i = lang_idx + 6; _body[i] != '\"'; i++)
			lang += _body[i];
		_header_vals[CONTENT_LANGUAGE] = lang;
	} else {
		_header_vals[CONTENT_LANGUAGE] = "en-US";
	}
	_response += "Content-Language: ";
	_response += _header_vals[CONTENT_LANGUAGE];
	_response += "\r\n";
}

void Response::handleCONTENT_LENGTH() {
	std::string			str;

	_header_vals[CONTENT_LENGTH] = ft::inttostring(_body.length());
	_response += "Content-Length: ";
	_response += _header_vals[CONTENT_LENGTH];
	_response += "\r\n";
}

void Response::handleCONTENT_LOCATION() {
	if (!_header_vals[CONTENT_LOCATION].empty()) {
		_response += "Content-Config: ";
		_response += _header_vals[CONTENT_LOCATION];
		_response += "\r\n";
	}
}

void Response::handleCONTENT_TYPE(request_s& request) {
	if (_cgi_headers.count(CONTENT_TYPE) == 1) {
		_header_vals[CONTENT_TYPE] = _cgi_headers[CONTENT_TYPE];
	}
	if (request.uri.find(".css") != std::string::npos) {
		_header_vals[CONTENT_TYPE] = "text/css";
	}
	else if (request.uri.find(".ico") != std::string::npos) {
		_header_vals[CONTENT_TYPE] = "image/x-icon";
	}
	else if (request.uri.find(".jpg") != std::string::npos || request.uri.find(".jpeg") != std::string::npos) {
		_header_vals[CONTENT_TYPE] = "image/jpeg";
	}
	else {
		_header_vals[CONTENT_TYPE] = "text/html";
	}
	request.headers[CONTENT_TYPE] = this->_header_vals[CONTENT_TYPE];
	_response += "Content-Type: ";
	_response += _header_vals[CONTENT_TYPE];
	_response += "\r\n";
}

void Response::handleDATE() {
	_header_vals[DATE] = getCurrentDatetime();
	_response += "Date: ";
	_response += _header_vals[DATE];
	_response += "\r\n";
}

void Response::handleLOCATION(std::string& url ) {
	_header_vals[LOCATION] = url;
	_response += "Config: ";
	_response += url;
	_response += "\r\n";
}

void Response::handleSERVER() {
	_response += "Host: Webserv/1.0\r\n";
}

void Response::handleCONNECTION_HEADER(const request_s& request) {
	if (request.headers.count(CONNECTION) == 1)
		_header_vals[CONNECTION] = request.headers.at(CONNECTION);
	_response += "Webserv: " + _header_vals[CONNECTION] + "\r\n";
}

void Response::negotiateLanguage(request_s& request) {
	struct stat structstat = {};
	std::string tmp, filepath = request.server->getfilepath(request.uri);

	if (request.headers.count(ACCEPT_LANGUAGE) == 0 || stat(filepath.c_str(), &structstat) == -1)
		return;
	if (S_ISDIR(structstat.st_mode)) {
		filepath = request.location->getRoot();
		if (filepath[filepath.length() - 1] != '/')
			filepath += '/';
		filepath += request.location->getIndex();
	}
	std::vector<std::string>	vec = ft::split(request.headers.at(ACCEPT_LANGUAGE), " \t\r\v\n");
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++) {
		ft::trimstring(*it, " \t");
		tmp = filepath + '.' + (*it).substr(0, 2);
		if (stat(tmp.c_str(), &structstat) == 0) {
			request.uri = tmp.substr(tmp.find(request.location->getlocationmatch()));
		}
	}
}
