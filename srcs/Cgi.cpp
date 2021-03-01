#include "Cgi.hpp"
#include <sys/wait.h>
#include <cerrno>
#include "Server.hpp"

void	exit_fatal()
{
	std::cerr << strerror(errno) << std::endl;
	exit(EXIT_FAILURE);
}

Cgi::Cgi() : environment()
{

}

Cgi::Cgi(const Cgi &x) : environment()
{
	*this = x;
}

Cgi &Cgi::operator=(const Cgi &x)
{
	if (this != &x)
	{
		this->environment_map = x.environment_map;
		this->environment = x.environment;
	}
	return *this;
}

Cgi::~Cgi()
{
	this->environment_map.clear();
}

void Cgi::initEnvMap(request_s &req, const std::string& OriginalUri, bool redirect_status, std::string& scriptpath)
{
	char buf[500];
	std::string realpath = getcwd(buf, 500);
	this->environment_map["AUTH_TYPE"] = req.headers[AUTHORIZATION];
	this->environment_map["CONTENT_LENGTH"] = ft::inttostring(req.body.size());
	this->environment_map["CONTENT_TYPE"] = req.headers[CONTENT_TYPE];
	this->environment_map["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->environment_map["PATH_INFO"] = OriginalUri + req.cgiparams;
	this->environment_map["PATH_TRANSLATED"] = realpath + this->environment_map["PATH_INFO"];
	this->environment_map["QUERY_STRING"] = req.cgiparams;
	this->environment_map["REMOTE_ADDR"] = req.server->gethost();
	this->environment_map["REMOTE_IDENT"] = "";
	this->environment_map["REMOTE_USER"] = req.headers[REMOTE_USER];
	this->environment_map["REQUEST_METHOD"] = req.MethodToSTring();
	this->environment_map["REQUEST_URI"] = OriginalUri;
	this->environment_map["REQUEST_FILENAME"] = OriginalUri;
	this->environment_map["SCRIPT_FILENAME"] = scriptpath;
	this->environment_map["SCRIPT_NAME"] = scriptpath;
	this->environment_map["SERVER_NAME"] = req.server->getservername();
	this->environment_map["SERVER_PORT"] = std::string(ft::inttostring(req.server->getport()));
	this->environment_map["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->environment_map["SERVER_SOFTWARE"] = "HTTP 1.1";
	if (redirect_status)
		this->environment_map["REDIRECT_STATUS"] = "true";
}

void Cgi::mapToEnv(request_s& request)
{
	int i = 0;
	this->environment_map.insert(request.env.begin(), request.env.end());
	this->environment = (char **) ft::calloc(this->environment_map.size() + 1, sizeof(char *));
	if (!environment)
		exit_fatal();

	for (std::map<std::string, std::string>::const_iterator it = environment_map.begin(); it != environment_map.end(); it++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->environment[i] = ft::strdup(tmp.c_str());
		if (!this->environment[i])
			exit_fatal();
		++i;
	}
}

void	Cgi::clearEnv()
{
	for (int i = 0; environment[i]; i++)
	{
		free(environment[i]);
		environment[i] = nullptr;
	}
	free(environment);
	environment = nullptr;
}

int Cgi::run_cgi(request_s &request, std::string& scriptpath, const std::string& Uri)
{
	int file_to_load;
	int file_to_send;
	int status;
	pid_t pid;
	bool redirect_status = false;
	char *args[3] = {&scriptpath[0], nullptr, nullptr};

	std::string phpcgipath = request.server->matchlocation(Uri)->getPhpCgiPath();
	if (!phpcgipath.empty() && ft::getextension(scriptpath) == ".php")
	{
		args[0] = ft::strdup(phpcgipath.c_str());
		args[1] = ft::strdup(scriptpath.c_str());
		redirect_status = true;
	}
	this->initEnvMap(request, Uri, redirect_status, scriptpath);
	this->mapToEnv(request);

	if ((file_to_load = open("/tmp/webservin.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) == -1)
		exit_fatal();
	write(file_to_load, request.body.c_str(), request.body.length());
	if (close(file_to_load) == -1)
		exit_fatal();
	if ((pid = fork()) == -1)
		exit_fatal();

	if (pid == 0)
	{
		if ((file_to_send = open("/tmp/webservout.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) == -1)
			exit_fatal();
		if (dup2(file_to_send, STDOUT_FILENO) == -1 || close(file_to_send) == -1)
			exit_fatal();
		if ((file_to_load = open("/tmp/webservin.txt", O_RDONLY, S_IRWXU)) == -1)
			exit_fatal();
		if (dup2(file_to_load, STDIN_FILENO) == -1 || close(file_to_load) == -1)
			exit_fatal();
		if (execve(args[0], args, environment) == -1)
			std::cerr << "execve: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	this->clearEnv();
	waitpid(0, &status, 0);
	if (WIFEXITED(status))
		status = WEXITSTATUS(status);
	request.cgi_ran = true;
	if ((file_to_send = open("/tmp/webservout.txt", O_RDONLY, S_IRWXU)) == -1)
		exit_fatal();
	if (!phpcgipath.empty())
	{
		free(args[0]);
		free(args[1]);
	}
	return (file_to_send);
}
