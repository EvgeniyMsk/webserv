#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Server.hpp"
#include "Utils.hpp"

class Webserv
{
private:
	fd_set w_read_set;
	fd_set w_write_set;
	fd_set m_read_set;
	fd_set m_write_set;

	static std::string readFile(char *file);

	void getContent(std::string &buffer, std::string &context, std::string prec, size_t &nb_line, Config &config);

public:
	Webserv();

	virtual ~Webserv();

	static void exit(int sig);

	void parse(char *file, std::vector<Server> &servs);

	void init();

	void run();

	static int getMaxFd(std::vector<Server> &servs);

	static int getOpenFd(std::vector<Server> &servs);

	class InvalidConfigFileException : public std::exception
	{
	private:
		size_t line;
		std::string error;

		InvalidConfigFileException();

	public:
		InvalidConfigFileException(size_t d);

		virtual ~InvalidConfigFileException() throw();

		virtual const char *what() const throw();
	};

	std::vector<Server> servers;
};

#endif
