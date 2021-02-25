#ifndef Server_HPP
#define Server_HPP

#include <queue>
#include "Handler.hpp"

#define TIMEOUT 10
#define RETRY	"25"

class Webserv
{
	friend class Config;
	typedef std::map<std::string, std::string> 	elmt;
	typedef std::map<std::string, elmt>			config;

	private:
		int						serverSocket;
		int						maxFD;
		int						port;
		struct sockaddr_in		address;
		fd_set					*w_read_set;
		fd_set					*w_write_set;
		fd_set					*m_read_set;
		fd_set					*m_write_set;
		Handler					_handler;
		std::vector<config>		s_config;

	public:
		std::vector<Client*>	clients;
		std::queue<int>			_tmp_clients;
		
		Webserv();
		~Webserv();

		int		getMaxFd();
		int		getServerSocket() const;
		int		getOpenFd();
		void	init(fd_set *readSet, fd_set *writeSet, fd_set *rSet, fd_set *wSet);
		void	refuseConnection();
		void	acceptConnection();
		int		readRequest(std::vector<Client*>::iterator it);
		int		writeResponse(std::vector<Client*>::iterator it);
		void	send503(int fd);

	class		ServerException: public std::exception
	{
		private:
			std::string function;
			std::string error;

		public:
			ServerException(void);
			ServerException(std::string function, std::string error);
			virtual	~ServerException(void) throw();	
			virtual const char		*what(void) const throw();
	};

	private:
		int		getTimeDiff(std::string start);

};

#endif
