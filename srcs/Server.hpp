#ifndef SERVER_HPP
# define SERVER_HPP

# include	<vector>
# include    "Config.hpp"
# include	"Client.hpp"

#define BACKLOG 128

class Server {
	public:
		Server();
		explicit Server(int fd);
		~Server();
		Server(const Server& x);
		Server& 	operator=(const Server& x);
		friend class Connection;

private:
		void		setport(const std::string& );
		void		sethost(const std::string& );
		void		setindexes(const std::string& );
		void		setroot(const std::string& );
		void		setservername(const std::string& );
		void		setmaxfilesize(const std::string& );
		void		seterrorpage(const std::string& );
		void		setautoindex(const std::string& );
		void		configurelocation(const std::string& );
public:
		void		startListening();
		int			addConnection();
		std::vector<Client*> _connections;
		size_t					getport() const;
		std::string				gethost() const;
		std::string				getindex() const;
		std::string				getroot() const;
		std::string				getservername() const;
		long int				getmaxfilesize() const;
		std::string				geterrorpage() const;
		int 					getpage(const std::string& uri, std::map<header_w, std::string>&, bool autoindex) const;
		std::vector<Config*> 	getlocations() const;
		int						getSocketFd() const;
		std::string 			getautoindex() const;

		Config*	matchlocation(const std::string& uri) const;
		std::string	getfilepath(const std::string& uri) const;
		void		setup(int fd);
		void		addServerInfoToLocation(Config* loc) const;

private:
		size_t		_port;
		long int	_maxfilesize;
		std::string _host,
					_server_name,
					_error_page,
					_root,
					_autoindex;
		int			_fd,
					_socketFd;
		struct sockaddr_in	addr;
		std::vector<std::string> _indexes;
		std::vector<Config*> _locations;
};

std::ostream&	operator<<(std::ostream& o, const Server& x);

#endif
