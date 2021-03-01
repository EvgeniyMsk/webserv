#include	"Server.hpp"
#include	<cerrno>
#include	<sys/stat.h>

Server::Server() : _port(80), _maxfilesize(1000000),
		_host("0.0.0.0"), _error_page("error.html"),
		_root("htmlfiles"),
		_fd(), _socketFd(), addr() {
}

Server::Server(int fd) : _port(80), _maxfilesize(1000000),
		_host("0.0.0.0"), _error_page("error.html"),
		_root("htmlfiles"),
		_socketFd(), addr() {
	this->_fd = fd;
}

Server::~Server() {
	for (std::vector<Client*>::iterator cli = _connections.begin(); cli != _connections.end(); cli++) {
		delete *cli;
	}
	for (std::vector<Config*>::iterator loc = _locations.begin(); loc != _locations.end(); loc++) {
		delete *loc;
	}
	_connections.clear();
	_locations.clear();
	_indexes.clear();
	close(_socketFd);
	_socketFd = -1;
}

Server::Server(const Server& x) : _port(), _maxfilesize(), _fd(), _socketFd(), addr() {
	*this = x;
}

Server&	Server::operator=(const Server& x) {
	if (this != &x) {
		this->_port = x._port;
		this->_host = x._host;
		this->_server_name = x._server_name;
		this->_maxfilesize = x._maxfilesize;
		this->_error_page = x._error_page;
		this->_indexes = x._indexes;
		this->_root = x._root;
		this->_locations = x._locations;
		this->_socketFd = x._socketFd;
		this->addr = x.addr;

		this->_connections = x._connections;
	}
	return *this;
}

size_t	Server::getport() const {
	return this->_port;
}

void	Server::setport(const std::string& port) {
	this->_port = ft::atoi(port.c_str());
}

std::string	Server::gethost() const {
	return this->_host;
}

void	Server::sethost(const std::string& host) {
	this->_host = host;
}

std::string	Server::getindex() const {
	struct stat statstruct = {};
	std::string filepath;
	for (std::vector<std::string>::const_iterator it = _indexes.begin(); it != _indexes.end(); ++it) {
		filepath = this->_root + *it;
		if (stat(filepath.c_str(), &statstruct) != -1)
			return *it;
	}
	return this->_error_page;
}

void	Server::setindexes(const std::string& index) {
	this->_indexes = ft::split(index, " \t\r\n\v\f");
}

std::string	Server::getroot() const {
	return this->_root;
}

void	Server::setroot(const std::string& root) {
	this->_root = root;
}

std::string	Server::getservername() const {
	return this->_server_name;
}

void	Server::setservername(const std::string& servername) {
	this->_server_name = servername.substr(servername.find_first_not_of(" \t\r\n\f\v"), servername.find_last_not_of(" \t\r\n\f\v") - servername.find_first_not_of(" \t\r\n\f\v") + 1);
}

long int	Server::getmaxfilesize() const {
	return this->_maxfilesize;
}

void	Server::setmaxfilesize(const std::string& clientbodysize) {
	this->_maxfilesize = ft::atol(clientbodysize.c_str());
	if (clientbodysize[clientbodysize.find_first_not_of("1234567890")] == 'M')
		this->_maxfilesize *= 1000000;
}

std::string	Server::geterrorpage() const {
	return this->getroot() + '/' + this->_error_page;
}

void	Server::seterrorpage(const std::string& errorpage) {
	this->_error_page = errorpage;
}


void	Server::configurelocation(const std::string& in) {
	std::vector<std::string> v = ft::split(in, " \t\r\n\v\f");
	Config	*loc = new Config(v[0]);
	loc->setup(this->_fd);
	this->_locations.push_back(loc);
}

std::vector<Config*>	Server::getlocations() const {
	return this->_locations;
}

void	Server::setup(int fd) {
	this->_fd = fd;
	std::map<std::string, void (Server::*)(const std::string&)> m;
	m["port"] = &Server::setport;
	m["host"] = &Server::sethost;
	m["autoindex"] = &Server::setautoindex;
	m["index"] = &Server::setindexes;
	m["root"] = &Server::setroot;
	m["server_name"] = &Server::setservername;
	m["max_filesize"] = &Server::setmaxfilesize;
	m["error_page"] = &Server::seterrorpage;
	m["location"] = &Server::configurelocation;
	std::string str;

	while (ft::get_next_line(fd, str) > 0) {
		std::string key, value;
		if (str.empty() || ft::is_first_char(str) || ft::is_first_char(str, ';'))
			continue ;
		if (ft::is_first_char(str, '}'))
			break ;
		ft::get_key_value(str, key, value);
		if (!m.count(key))
			std::cerr << "Unable to parse key '" << key << "' in Server block " << this->getservername() << std::endl;
		(this->*(m.at(key)))(value);
	}
	if (_port <= 0 || _host.empty() || _maxfilesize <= 0 || _error_page.empty() || _server_name.empty())
		throw std::runtime_error("invalid setting in server block");
}

int Server::getSocketFd() const {
	return _socketFd;
}

void Server::setautoindex(const std::string& ai) {
	this->_autoindex = ai;
}

std::string Server::getautoindex() const {
	return this->_autoindex;
}

Config* Server::matchlocation(const std::string &uri) const {
	size_t		n;
	Config*	out = _locations.front();

	for (std::vector<Config*>::const_iterator it = this->_locations.begin() + 1; it != this->_locations.end(); ++it) {
		n = (*it)->getlocationmatch().length();
		if ((*it)->getlocationmatch()[n - 1] == '/' && n > 1)
			n -= 1;
		if (n >= out->getlocationmatch().length() && (*it)->getlocationmatch().compare(0, n, uri, 0, n) == 0)
			out = *it;
	}
	this->addServerInfoToLocation(out);
	return (out);
}

std::string	Server::getfilepath(const std::string& uri) const {
	Config* loca = this->matchlocation(uri);

	std::string	TrimmedUri(uri),
				filepath(loca->getRoot());
	TrimmedUri.erase(0, loca->locationMatch.length());
	if (filepath[filepath.length() - 1] != '/' && TrimmedUri[0] != '/')
		filepath += '/';
	filepath += TrimmedUri;
	return (filepath);
}


int Server::getpage(const std::string &uri, std::map<header_w, std::string>& headervals, bool autoindex) const {
	struct stat statstruct = {};
	int fd = -1;
	Config*	loca = this->matchlocation(uri);
	std::string filepath = this->getfilepath(uri);

	if (stat(filepath.c_str(), &statstruct) != -1) {
		if (S_ISDIR(statstruct.st_mode)) {
			if (filepath[filepath.length() - 1] != '/')
				filepath += '/';
			filepath += loca->getIndex();
			if (!filepath.empty())
				fd = open(filepath.c_str(), O_RDONLY);
			if (autoindex == true && fd < 0)
				return (-3);
		} else if (!filepath.empty())
			fd = open(filepath.c_str(), O_RDONLY);
	} else if (autoindex == true && uri[uri.length() - 1] == '/')
		return (-3);
 	headervals[CONTENT_LOCATION] = filepath;
	return (fd);
}

void Server::startListening() {
	bzero(&addr, sizeof(addr));

	if ((_socketFd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		std::cerr << "Error setting server socket\n";
		throw std::runtime_error(strerror(errno));
	}
	int x = 1;
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEPORT, &x, sizeof(x)) == -1) {
		std::cerr << "Error setting server socket options\n";
		throw std::runtime_error(strerror(errno));
	}
	
	addr.sin_family = AF_INET;
	if (gethost() == "localhost") {
		addr.sin_addr.s_addr = INADDR_ANY;
	}
	else {
		addr.sin_addr.s_addr = inet_addr(gethost().c_str());
	}
	addr.sin_port = htons(getport());

	for (int i = 0; i < 6; ++i) {
		if ((bind(this->_socketFd, (struct sockaddr *) &addr, sizeof(addr))) == -1) {
			std::cerr << "Cannot bind (" << errno << " " << strerror(errno) << ")" << std::endl;
			if (i == 5)
				throw std::runtime_error(strerror(errno));
		} else
			break;
	}
	if (listen(this->_socketFd, BACKLOG) == -1) {
		std::cerr << "Error listening to server socket\n";
		throw std::runtime_error(strerror(errno));
	}

	if (fcntl(this->_socketFd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error setting server socket to be nonblocking\n";
		throw std::runtime_error(strerror(errno));
	}
}

int Server::addConnection() {
	Client* newClient = new Client(this);
	this->_connections.push_back(newClient);
	return newClient->clientSocket;
}

void Server::addServerInfoToLocation(Config* loc) const {
	if (loc->root.empty())
		loc->root = this->_root;
	if (loc->autoIndex.empty())
		loc->autoIndex = this->_autoindex;
	if (loc->indexes.empty())
		loc->indexes = this->_indexes;
	if (loc->maxBody == 0)
		loc->maxBody = this->_maxfilesize;
	if (loc->errorPage.empty())
		loc->errorPage = this->_error_page;
}

std::ostream& operator<<( std::ostream& o, const Server& x) {
	o << x.getservername() <<  " is listening on: " << x.gethost() << ":" << x.getport() << std::endl;
	o << "Default root folder: " << x.getroot() << std::endl;
	o << "Default index page: " << x.getindex() << std::endl;
	o << "error page: " << x.geterrorpage() << std::endl;
	o << "client body limit: " << x.getmaxfilesize() << std::endl << std::endl;
	std::vector<Config*> v = x.getlocations();
	for (size_t i = 0; i < v.size(); i++) {
		o << *v[i];
	}
	o << std::endl;
	return (o);
}

