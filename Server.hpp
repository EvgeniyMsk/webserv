#ifndef SERVER_HPP
# define SERVER_HPP

# include <list>
# include <map>
# include <algorithm>
# include <sstream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>
# include "Utils.hpp"
# include "Request.hpp"

class Server
{
private:
    int list;
    struct sockaddr_in addr;
    int port;
    std::list<int> readClient;
    std::map<int, Request *> clientRequest;
    int maxFd;
    Server() {};
public:
    Server(int newPort);
    ~Server() {};
    const int & getList() const;
    void updateMaxFd();
    const int & getMaxFd() const;
    void acceptConnection();
    void processConnections(fd_set* globalReadSetPtr);
    void handleRequests(fd_set* globalReadSetPtr);
    std::list<int>& getReadClients();
};

#endif
