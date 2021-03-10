#include "HTTP.hpp"

HTTP::HTTP()
{

}

HTTP::~HTTP()
{

}

void			HTTP::parseRequest(Client &client, std::vector<config> &conf)
{
	Request				request;
	std::string			tmp;
	std::string			buffer;

	buffer = std::string(client.rBuf);
	if (buffer[0] == '\r')
		buffer.erase(buffer.begin());
	if (buffer[0] == '\n')
		buffer.erase(buffer.begin());
	ft::get_next_line(buffer, request.method, ' ');
	ft::get_next_line(buffer, request.uri, ' ');
	ft::get_next_line(buffer, request.version);
	if (parseHeaders(buffer, request))
		request.isValid = checkSyntax(request);
	if (request.uri != "*" || request.method != "OPTIONS")
		getConf(client, request, conf);
	if (request.isValid)
	{
		if (client.conf["root"][0] != '\0')
			chdir(client.conf["root"].c_str());
		if (request.method == "POST" || request.method == "PUT")
			client.status = BODYPARSING;
		else
			client.status = CODE;
	}
	else
	{
		request.method = "BAD";
		client.status = CODE;
	}
	client.req = request;
	tmp = client.rBuf;
	tmp = tmp.substr(tmp.find("\r\n\r\n") + 4);
	strcpy(client.rBuf, tmp.c_str());
}

bool			HTTP::parseHeaders(std::string &buf, Request &req)
{
	size_t		pos;
	std::string	line;
	std::string	key;
	std::string	value;

	while (!buf.empty())
	{
		ft::get_next_line(buf, line);
		if (line.size() < 1 || line[0] == '\n' || line[0] == '\r')
			break ;
		if (line.find(':') != std::string::npos)
		{
			pos = line.find(':');
			key = line.substr(0, pos);
			if (line[pos + 1] == ' ')
				value = line.substr(pos + 2);
			else
				value = line.substr(pos + 1);
			if (ft::isSpace(value[0]) || ft::isSpace(key[0]) || value.empty() || key.empty())
			{
				req.isValid = false;
				return (false);
			}
			req.headers[key] = value;
			req.headers[key].pop_back(); //remove '\r'
		}
		else
		{
			req.isValid = false;
			return (false);
		}
	}
	return (true);
}

void			HTTP::parseBody(Client &client)
{
	if (client.req.headers.find("Content-Length") != client.req.headers.end())
		getBody(client);
	else if (client.req.headers["Transfer-Encoding"] == "chunked")
		dechunkBody(client);
	else
	{
		client.req.method = "BAD";
		client.status = CODE;
	}
	if (client.status == CODE)
		g_logger.log("body size parsed from " + client.ip + ":" + std::to_string(client.port) + ": " + std::to_string(client.req.body.size()), MED);
}

void			HTTP::getBody(Client &client)
{
	unsigned int	bytes;

	if (client.chunk.len == 0)
		client.chunk.len = atoi(client.req.headers["Content-Length"].c_str());
	if (client.chunk.len < 0)
	{
		client.req.method = "BAD";
		client.status = CODE;
		return ;
	}
	bytes = strlen(client.rBuf);
	if (bytes >= client.chunk.len)
	{
		memset(client.rBuf + client.chunk.len, 0, BUFFER_SIZE - client.chunk.len);
		client.req.body += client.rBuf;
		client.chunk.len = 0;
		client.status = CODE;
	}
	else
	{
		client.chunk.len -= bytes;
		client.req.body += client.rBuf;
		memset(client.rBuf, 0, BUFFER_SIZE + 1);
	}
}

void			HTTP::dechunkBody(Client &client)
{
	if (strstr(client.rBuf, "\r\n") && client.chunk.found == false)
	{
		client.chunk.len = _helper.findLen(client);
		if (client.chunk.len == 0)
			client.chunk.done = true;
		else
			client.chunk.found = true;
	}
	else if (client.chunk.found == true)
		_helper.fillBody(client);
	if (client.chunk.done)
	{
		memset(client.rBuf, 0, BUFFER_SIZE + 1);
		client.status = CODE;
		client.chunk.found = false;
		client.chunk.done = false;
		return ;
	}
}

void			HTTP::getConf(Client &client, Request &req, std::vector<config> &conf)
{
	std::map<std::string, std::string> elmt;
	std::string		tmp;
	std::string 	file;
	struct stat		info;
	config			to_parse;

	if (!req.isValid)
	{
		client.conf["error"] = conf[0]["server|"]["error"];
		return ;
	}
	std::vector<config>::iterator it(conf.begin());
	while (it != conf.end())
	{
		if (req.headers["Host"] == (*it)["server|"]["server_name"])
		{
			to_parse = *it;
			break ;
		}
		++it;
	}
	if (it == conf.end())
		to_parse = conf[0];
	file = req.uri.substr(req.uri.find_last_of('/') + 1, req.uri.find('?'));
	tmp = req.uri;
	do
	{
		if (to_parse.find("server|location " + tmp + "|") != to_parse.end())
		{
			elmt = to_parse["server|location " + tmp + "|"];
			break ;
		}
		tmp = tmp.substr(0, tmp.find_last_of('/'));
	} while (tmp != "");
	if (elmt.size() == 0)
		if (to_parse.find("server|location /|") != to_parse.end())
			elmt = to_parse["server|location /|"];
	if (elmt.size() > 0)
	{
		client.conf = elmt;
		client.conf["path"] = req.uri.substr(0, req.uri.find("?"));
		if (elmt.find("root") != elmt.end())
			client.conf["path"].replace(0, tmp.size(), elmt["root"]);
	}
	for (std::map<std::string, std::string>::iterator it(to_parse["server|"].begin()); it != to_parse["server|"].end(); ++it)
	{
		if (client.conf.find(it->first) == client.conf.end())
			client.conf[it->first] = it->second;
	}
	lstat(client.conf["path"].c_str(), &info);
	if (S_ISDIR(info.st_mode))
		if (client.conf["index"][0] && client.conf["listing"] != "on")
			client.conf["path"] += "/" + elmt["index"];
	if (req.method == "GET")
		client.conf["savedpath"] = client.conf["path"];
	g_logger.log("path requested from " + client.ip + ":" + std::to_string(client.port) + ": " + client.conf["path"], MED);
}

void			HTTP::negotiate(Client &client)
{
	std::multimap<std::string, std::string> 	languageMap;
	std::multimap<std::string, std::string> 	charsetMap;
	int				fd = -1;
	std::string		path;
	std::string		ext;

	if (client.req.headers.find("Accept-Language") != client.req.headers.end())
		_helper.parseAcceptLanguage(client, languageMap);
	if (client.req.headers.find("Accept-Charset") != client.req.headers.end())
		_helper.parseAcceptCharset(client, charsetMap);
	if (!languageMap.empty())
	{
		for (std::multimap<std::string, std::string>::reverse_iterator it(languageMap.rbegin()); it != languageMap.rend(); ++it)
		{
			if (!charsetMap.empty())
			{
				for (std::multimap<std::string, std::string>::reverse_iterator it2(charsetMap.rbegin()); it2 != charsetMap.rend(); ++it2)
				{
					ext = it->second + "." + it2->second;
					path = client.conf["savedpath"] + "." + ext;
					fd = open(path.c_str(), O_RDONLY);
					if (fd != -1)
					{
						client.res.headers["Content-Language"] = it->second;
						break ;
					}
					ext = it2->second + "." + it->second;
					path = client.conf["savedpath"] + "." + ext;
					fd = open(path.c_str(), O_RDONLY);
					if (fd != -1)
					{
						client.res.headers["Content-Language"] = it->second;
						break ;
					}
				}
			}
			else
			{
				ext = it->second;
				path = client.conf["savedpath"] + "." + ext;
				fd = open(path.c_str(), O_RDONLY);
				if (fd != -1)
				{
					client.res.headers["Content-Language"] = it->second;
					break ;
				}
			}
		}
	}
	else if (languageMap.empty())
	{
		if (!charsetMap.empty())
		{
			for (std::multimap<std::string, std::string>::reverse_iterator it2(charsetMap.rbegin()); it2 != charsetMap.rend(); ++it2)
			{
				ext = it2->second;
				path = client.conf["savedpath"] + "." + it2->second;
				fd = open(path.c_str(), O_RDONLY);
				if (fd != -1)
					break ;
			}
		}
	}
	if (fd != -1)
	{
		client.conf["path"] = path;
		client.res.headers["Content-Location"] = client.req.uri + "." + ext;
		if (client.read_fd != -1)
			close(client.read_fd);
		client.read_fd = fd;
		client.res.statusCode = OK;
	}
}

void			HTTP::createListing(Client &client)
{
	DIR				*dir;
	struct dirent	*cur;

	close(client.read_fd);
	client.read_fd = -1;
	dir = opendir(client.conf["path"].c_str());
	client.res.body = "<html>\n<body>\n";
	client.res.body += "<h1>Directory listing</h1>\n";
	while ((cur = readdir(dir)) != NULL)
	{
		if (cur->d_name[0] != '.')
		{
			client.res.body += "<a href=\"" + client.req.uri;
			if (client.req.uri != "/")
				client.res.body += "/";
			client.res.body += cur->d_name;
			client.res.body += "\">";
			client.res.body += cur->d_name;
			client.res.body += "</a><br>\n";
		}
	}
	closedir(dir);
	client.res.body += "</body>\n</html>\n";
}

//TO COMPLETE
bool			HTTP::checkSyntax(const Request &req)
{
	if (req.method.size() == 0 || req.uri.size() == 0
		|| req.version.size() == 0)
		return (false);
	if (req.method != "GET" && req.method != "POST"
		&& req.method != "HEAD" && req.method != "PUT"
		&& req.method != "CONNECT" && req.method != "TRACE"
		&& req.method != "OPTIONS" && req.method != "DELETE")
		return (false);
	if (req.method != "OPTIONS" && req.uri[0] != '/') //OPTIONS can have * as uri
		return (false);
	if (req.version != "HTTP/1.1\r" && req.version != "HTTP/1.1")
		return (false);
	if (req.headers.find("Host") == req.headers.end())
		return (false);
	return (true);
}

void			HTTP::execCGI(Client &client)
{
	char			**args = NULL;
	char			**env = NULL;
	std::string		path;
	int				ret;
	int				tubes[2];

	if (client.conf["php"][0] && client.conf["path"].find(".php") != std::string::npos)
		path = client.conf["php"];
	else if (client.conf["exec"][0])
		path = client.conf["exec"];
	else
		path = client.conf["path"];
	close(client.read_fd);
	client.read_fd = -1;
	args = (char **)(malloc(sizeof(char *) * 3));
	args[0] = strdup(path.c_str());
	args[1] = strdup(client.conf["path"].c_str());
	args[2] = NULL;
	env = _helper.setEnv(client);
	client.tmp_fd = open(TMP_PATH, O_WRONLY | O_CREAT, 0666);
	pipe(tubes);
	g_logger.log("executing CGI for " + client.ip + ":" + std::to_string(client.port), MED);
	if ((client.cgi_pid = fork()) == 0)
	{
		close(tubes[1]);
		dup2(tubes[0], 0);
		dup2(client.tmp_fd, 1);
		errno = 0;
		ret = execve(path.c_str(), args, env);
		if (ret == -1)
		{
			std::cerr << "Error with CGI: " << strerror(errno) << std::endl;
			exit(1);
		}
	}
	else
	{
		close(tubes[0]);
		client.write_fd = tubes[1];
		client.read_fd = open(TMP_PATH, O_RDONLY);
		client.setFileToWrite(true);
	}
	ft::freeAll(args, env);
}

void		HTTP::parseCGIResult(Client &client)
{
	size_t			pos;
	std::string		headers;
	std::string		key;
	std::string		value;

	if (client.res.body.find("\r\n\r\n") == std::string::npos)
		return ;
	headers = client.res.body.substr(0, client.res.body.find("\r\n\r\n") + 1);
	pos = headers.find("Status");
	if (pos != std::string::npos)
	{
		client.res.statusCode.clear();
		pos += 8;
		while (headers[pos] != '\r')
		{
			client.res.statusCode += headers[pos];
			pos++;
		}
	}
	pos = 0;
	while (headers[pos])
	{
		while (headers[pos] && headers[pos] != ':')
		{
			key += headers[pos];
			++pos;
		}
		++pos;
		while (headers[pos] && headers[pos] != '\r')
		{
			value += headers[pos];
			++pos;
		}
		client.res.headers[key] = value;
		key.clear();
		value.clear();
		if (headers[pos] == '\r')
			pos++;
		if (headers[pos] == '\n')
			pos++;
	}
	pos = client.res.body.find("\r\n\r\n") + 4;
	client.res.body = client.res.body.substr(pos);
	client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
}

void		HTTP::createResponse(Client &client)
{
	std::map<std::string, std::string>::const_iterator b;

	client.response = client.res.version + " " + client.res.statusCode + "\r\n";
	b = client.res.headers.begin();
	while (b != client.res.headers.end())
	{
		if (b->second != "")
			client.response += b->first + ": " + b->second + "\r\n";
		++b;
	}
	client.response += "\r\n";
	client.response += client.res.body;
	client.res.clear();
}

void	HTTP::dispatcher(Client &client)
{
	typedef void	(HTTP::*ptr)(Client &client);
	std::map<std::string, ptr> map;

	map["GET"] = &HTTP::handleGet;
	map["HEAD"] = &HTTP::handleHead;
	map["PUT"] = &HTTP::handlePut;
	map["POST"] = &HTTP::handlePost;
	map["CONNECT"] = &HTTP::handleConnect;
	map["TRACE"] = &HTTP::handleTrace;
	map["OPTIONS"] = &HTTP::handleOptions;
	map["DELETE"] = &HTTP::handleDelete;
	map["BAD"] = &HTTP::handleBadRequest;

	(this->*map[client.req.method])(client);
}

void	HTTP::handleGet(Client &client)
{
	struct stat	file_info;
	std::string	credential;

	switch (client.status)
	{
		case CODE:
			_helper.getStatusCode(client);
			fstat(client.read_fd, &file_info);
			if (S_ISDIR(file_info.st_mode) && client.conf["listing"] == "on")
				createListing(client);
			if (client.res.statusCode == NOTFOUND)
				negotiate(client);
			if (((client.conf.find("CGI") != client.conf.end() && client.req.uri.find(client.conf["CGI"]) != std::string::npos)
				 || (client.conf.find("php") != client.conf.end() && client.req.uri.find(".php") != std::string::npos))
				&& client.res.statusCode == OK)
			{
				execCGI(client);
				client.status =CGI;
			}
			else
				client.status = HEADERS;
			client.setFileToRead(true);
			break ;
		case CGI:
			if (client.read_fd == -1)
			{
				parseCGIResult(client);
				client.status = HEADERS;
			}
			break ;
		case HEADERS:
			lstat(client.conf["path"].c_str(), &file_info);
			if (!S_ISDIR(file_info.st_mode))
				client.res.headers["Last-Modified"] = _helper.getLastModified(client.conf["path"]);
			if (client.res.headers["Content-Type"][0] == '\0')
				client.res.headers["Content-Type"] = _helper.findType(client);
			if (client.res.statusCode == UNAUTHORIZED)
				client.res.headers["WWW-Authenticate"] = "Basic";
			else if (client.res.statusCode == NOTALLOWED)
				client.res.headers["Allow"] = client.conf["methods"];
			client.res.headers["Date"] = ft::getDate();
			client.res.headers["Server"] = "webserv";
			client.status = BODY;
			break ;
		case BODY:
			if (client.read_fd == -1)
			{
				client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
				createResponse(client);
				client.status = RESPONSE;
			}
			break;
	}
}

void	HTTP::handleHead(Client &client)
{
	struct stat	file_info;

	switch (client.status)
	{
		case CODE:
			_helper.getStatusCode(client);
			fstat(client.read_fd, &file_info);
			if (S_ISDIR(file_info.st_mode) && client.conf["listing"] == "on")
				createListing(client);
			else if (client.res.statusCode == NOTFOUND)
				negotiate(client);
			fstat(client.read_fd, &file_info);
			if (client.res.statusCode == OK)
			{
				client.res.headers["Last-Modified"] = _helper.getLastModified(client.conf["path"]);
				client.res.headers["Content-Type"] = _helper.findType(client);
			}
			else if (client.res.statusCode == UNAUTHORIZED)
				client.res.headers["WWW-Authenticate"] = "Basic";
			else if (client.res.statusCode == NOTALLOWED)
				client.res.headers["Allow"] = client.conf["methods"];
			client.res.headers["Date"] = ft::getDate();
			client.res.headers["Server"] = "webserv";
			client.res.headers["Content-Length"] = std::to_string(file_info.st_size);
			createResponse(client);
			client.status = RESPONSE;
			break ;
	}
}

void	HTTP::handlePost(Client &client)
{
	switch (client.status)
	{
		case BODYPARSING:
			parseBody(client);
			break ;
		case CODE:
			_helper.getStatusCode(client);
			if (((client.conf.find("CGI") != client.conf.end() && client.req.uri.find(client.conf["CGI"]) != std::string::npos)
				 || (client.conf.find("php") != client.conf.end() && client.req.uri.find(".php") != std::string::npos))
				&& client.res.statusCode == OK)
			{
				execCGI(client);
				client.status = CGI;
				client.setFileToRead(true);
			}
			else
			{
				if (client.res.statusCode == OK || client.res.statusCode == CREATED)
					client.setFileToWrite(true);
				else
					client.setFileToRead(true);
				client.status = HEADERS;
			}
			break ;
		case CGI:
			if (client.read_fd == -1)
			{
				parseCGIResult(client);
				client.status = HEADERS;
			}
			break ;
		case HEADERS:
			if (client.res.statusCode == UNAUTHORIZED)
				client.res.headers["WWW-Authenticate"] = "Basic";
			else if (client.res.statusCode == NOTALLOWED)
				client.res.headers["Allow"] = client.conf["methods"];
			client.res.headers["Date"] = ft::getDate();
			client.res.headers["Server"] = "webserv";
			if (client.res.statusCode == CREATED)
				client.res.body = "File created\n";
			else if (client.res.statusCode == OK && !((client.conf.find("CGI") != client.conf.end() && client.req.uri.find(client.conf["CGI"]) != std::string::npos)
													  || (client.conf.find("php") != client.conf.end() && client.req.uri.find(".php") != std::string::npos)))
				client.res.body = "File modified\n";
			client.status = BODY;
			break ;
		case BODY:
			if (client.read_fd == -1 && client.write_fd == -1)
			{
				if (client.res.headers["Content-Length"][0] == '\0')
					client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
				createResponse(client);
				client.status = RESPONSE;
			}
			break ;
	}
}

void	HTTP::handlePut(Client &client)
{
	std::string		path;
	std::string		body;

	switch (client.status)
	{
		case BODYPARSING:
			parseBody(client);
			break ;
		case CODE:
			if (_helper.getStatusCode(client))
				client.setFileToWrite(true);
			else
				client.setFileToRead(true);
			client.res.headers["Date"] = ft::getDate();
			client.res.headers["Server"] = "webserv";
			if (client.res.statusCode == CREATED || client.res.statusCode == NOCONTENT)
			{
				client.res.headers["Location"] = client.req.uri;
				if (client.res.statusCode == CREATED)
					client.res.body = "Ressource created\n";
			}
			if (client.res.statusCode == NOTALLOWED)
				client.res.headers["Allow"] = client.conf["methods"];
			else if (client.res.statusCode == UNAUTHORIZED)
				client.res.headers["WWW-Authenticate"] = "Basic";
			client.status = BODY;
			break ;
		case BODY:
			if (client.write_fd == -1 && client.read_fd == -1)
			{
				client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
				createResponse(client);
				client.status = RESPONSE;
			}
			break ;
	}
}

void	HTTP::handleConnect(Client &client)
{
	switch (client.status)
	{
		case CODE:
			_helper.getStatusCode(client);
			client.setFileToRead(true);
			client.res.headers["Date"] = ft::getDate();
			client.res.headers["Server"] = "webserv";
			client.status = BODY;
			break ;
		case BODY:
			if (client.read_fd == -1)
			{
				client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
				createResponse(client);
				client.status = RESPONSE;
			}
			break ;
	}
}

void	HTTP::handleTrace(Client &client)
{
	switch (client.status)
	{
		case CODE:
			_helper.getStatusCode(client);
			client.res.headers["Date"] = ft::getDate();
			client.res.headers["Server"] = "webserv";
			if (client.res.statusCode == OK)
			{
				client.res.headers["Content-Type"] = "message/http";
				client.res.body = client.req.method + " " + client.req.uri + " " + client.req.version + "\r\n";
				for (std::map<std::string, std::string>::iterator it(client.req.headers.begin());
					 it != client.req.headers.end(); ++it)
				{
					client.res.body += it->first + ": " + it->second + "\r\n";
				}
			}
			else
				client.setFileToRead(true);
			client.status = BODY;
			break ;
		case BODY:
			if (client.read_fd == -1)
			{
				client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
				createResponse(client);
				client.status = RESPONSE;
			}
			break ;
	}
}

void	HTTP::handleOptions(Client &client)
{
	switch (client.status)
	{
		case CODE:
			_helper.getStatusCode(client);
			client.res.headers["Date"] = ft::getDate();
			client.res.headers["Server"] = "webserv";
			if (client.req.uri != "*")
				client.res.headers["Allow"] = client.conf["methods"];
			createResponse(client);
			client.status = RESPONSE;
			break ;
	}
}

void	HTTP::handleDelete(Client &client)
{
	switch (client.status)
	{
		case CODE:
			std::cout << "here\n";
			if (!_helper.getStatusCode(client))
				client.setFileToRead(true);
			client.res.headers["Date"] = ft::getDate();
			client.res.headers["Server"] = "webserv";
			if (client.res.statusCode == OK)
			{
				unlink(client.conf["path"].c_str());
				client.res.body = "File deleted\n";
			}
			else if (client.res.statusCode == NOTALLOWED)
				client.res.headers["Allow"] = client.conf["methods"];
			else if (client.res.statusCode == UNAUTHORIZED)
				client.res.headers["WWW-Authenticate"] = "Basic";
			client.status = BODY;
			break ;
		case BODY:
			if (client.read_fd == -1)
			{
				client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
				createResponse(client);
				client.status = RESPONSE;
			}
			break ;
	}
}

void	HTTP::handleBadRequest(Client &client)
{
	struct stat		file_info;

	switch (client.status)
	{
		case CODE:
			client.res.version = "HTTP/1.1";
			client.res.statusCode = BADREQUEST;
			_helper.getErrorPage(client);
			fstat(client.read_fd, &file_info);
			client.setFileToRead(true);
			client.res.headers["Date"] = ft::getDate();
			client.res.headers["Server"] = "webserv";
			client.status = BODY;
			break ;
		case BODY:
			if (client.read_fd == -1)
			{
				client.res.headers["Content-Length"] = std::to_string(client.res.body.size());
				createResponse(client);
				client.status = RESPONSE;
			}
			break ;
	}
}
