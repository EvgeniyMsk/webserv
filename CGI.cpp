#include "CGI.hpp"


CGI::CGI(const Config &newConfig, const Position &position, input &newInp) : inp(newInp), config(newConfig)
{
	change.clear();
    this->position = position;
	timeval tv;

	gettimeofday(&tv, nullptr);
	tmpFile = "/tmp/ws" + std::to_string(tv.tv_sec * 1000000 + tv.tv_usec);
	fileName = trimAfter(inp.root, '/');
	expand = '.' + trimAfter(fileName, '.');
	args[1] = nullptr;
	env = nullptr;
	buff = nullptr;
	initEnvironments();
	initARGS();
	mapEnv();
}

CGI::~CGI()
{
	if (env)
	{
		for (int i = 0; env[i]; ++i)
			free(env[i]);
		free(env);
	}
	delete buff;
}

int	CGI::initARGS()
{
    args[0] = const_cast<char *>(position.getCgiScrypt().c_str());
    args[1] = const_cast<char *>(inp.root.c_str());
    args[2] = nullptr;
	return (1);
}

void CGI::setUrl()
{
	std::string	&uri = (*inp.requestMap)["Location"];
	size_t		q_pos = uri.find('?');
	size_t		slash_pos = uri.rfind('/');

	envMap["QUERY_STRING"] = (*inp.requestMap)["query_string"];
	if (slash_pos != std::string::npos && q_pos < slash_pos)
	{
		envMap["PATH_INFO"] = uri.substr(slash_pos + 1, uri.size());
		envMap["REQUEST_URI"] = uri;
		envMap["PATH_TRANSLATED"] = position.getRoot() + envMap["PATH_INFO"];
	}
	else
		envMap["PATH_INFO"] = "/";
}

void CGI::setHttpHeaders()
{
	std::map<std::string, std::string>::iterator it = (*inp.requestMap).begin();
	static std::array<std::string, 5> extras = { "body", "method", "location", "protocol" , "query_string"};
	std::string	httpKey;
	while (it != (*inp.requestMap).end())
	{
		if (std::find(extras.begin(), extras.end(), it->first) == extras.end())
		{
			httpKey = it->first;
			std::transform(httpKey.begin(), httpKey.end(), httpKey.begin(), ::toupper);
			std::replace(httpKey.begin(), httpKey.end(), '-', '_');

			envMap["HTTP_" + httpKey] = it->second;
		}
		++it;
	}
}

void CGI::initEnvironments()
{

	setUrl();
	setHttpHeaders();
	//двойная проверка
	if (inp.requestMap->count("Authorization"))
	{
		std::string	&auth = (*inp.requestMap)["Authorization"];
		std::pair<std::string, std::string> credentials = splitPair(auth, ' ');

		envMap["AUTH_TYPE"] = credentials.first;
		envMap["REMOTE_USER"] = credentials.second;
		envMap["REMOTE_IDENT"] = envMap["REMOTE_USER"];
	}
	envMap["CONTENT_LENGTH"] = (*inp.requestMap)["Content-Length"];
	envMap["CONTENT_TYPE"] = (*inp.requestMap)["Content-Type"];
	envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	envMap["REMOTE_ADDR"] = inp.remote_addr;
	envMap["REQUEST_METHOD"] = (*inp.requestMap)["method"];
	envMap["SERVER_NAME"] = config.getServerName();
	envMap["SERVER_PORT"] = std::to_string(config.getPort());
	envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
	envMap["SERVER_SOFTWARE"] = "webserv";
}

int CGI::mapEnv()
{
	int			i = 0;
	std::string	envir;
	std::map<std::string, std::string>::iterator it;

	if (!(env = (char**)malloc(sizeof(char*) * (envMap.size() + 1))))
			throw(std::runtime_error(strerror(errno)));
	for(it = envMap.begin(); it != envMap.end(); it++)
	{
		envir = (it->first + "=" + it->second);
		if (!(env[i] = ft_strdup(envir.c_str())))
			throw(std::runtime_error(strerror(errno)));
		i++;
	}
	env[i] = (NULL);
	return (1);
}

int	CGI::run()
{
	int fd[2], status, tmp_fd, child;

	status = pipe(fd);
	tmp_fd = open(tmpFile.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	if (status < 0 || tmp_fd < 0 || (child = fork ()) < 0)
		throw (std::runtime_error(strerror(errno)));
	if (child == 0)
	{
		close(fd[1]);
		dup2(tmp_fd, 1);
		dup2(fd[0], 0);
		close(tmp_fd);
		close(fd[0]);
		if (execve(args[0], args, env) == -1)
            exit(2);
		exit(1);
	}
	else
	{
		close(fd[0]);
		write(fd[1], (*inp.requestMap)["body"].c_str(), (*inp.requestMap)["body"].size());
		close(fd[1]);
		waitpid(child, &status, 0);
		if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code == 2)
                return (0);
		}
		readCGI();
	}
	return (1);
}

std::pair<std::string, std::string> splitPair(std::string const &str, std::string const &c)
{
	size_t delim = str.find(c);

    if (delim == str.npos)
		return (std::pair<std::string, std::string>(str, str));
	return (std::pair<std::string, std::string>(str.substr(0, delim), str.substr(delim + c.size(), str.size())));
}

void	CGI::createResponseMap()
{
	int fd = open(tmpFile.c_str(), O_RDONLY);
	int rv = 0;
	size_t	headlen = 0;
	std::pair<std::string, std::string> pr;
	std::string str;

	get_next_line(fd, str);
	if (!std::strncmp(str.c_str(), "Status: ", 8) && str[str.size() - 1] == '\r')
	{
		// добавление заголовков

		lseek(fd, 0, SEEK_SET);
		while ((rv = get_next_line(fd, str)) > 0 && str[0] != '\r')
		{
			headlen += str.size() + 1;
			if (!str.empty())
				str.pop_back();
			responseMap.insert(splitPair(str, ": "));
		}
	}
	responseMap.insert(std::pair<std::string, std::string>("#file", tmpFile));
	responseMap.insert(std::pair<std::string, std::string>("#seek", std::to_string(headlen += 2)));
	close(fd);
}

void    CGI::readCGI()
{
	int fd = open(tmpFile.c_str(), O_RDONLY);
	long size = findFileSize(fd);

	if (size < 0 || fd < 0)
		throw (std::runtime_error(strerror(errno)));
	createResponseMap();
}

const std::string &CGI::getResponse() const
{
	return response;
}

std::map<std::string, std::string> CGI::getRespMap() const
{
	return responseMap;
}
