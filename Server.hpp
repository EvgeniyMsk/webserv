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
# include <arpa/inet.h>
# include <sys/time.h>
# include "Utils.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Server
{
private:
    int list;
    struct sockaddr_in addr;
    int port;
    std::list<int> readClient;
    std::list<int> writeClient;
    std::map<int, Request *> clientRequest;
    int maxFd;
public:
    Server() {};
    Server(int newPort);
    ~Server() {};
    const int & getList() const;
    void updateMaxFd();
    const int & getMaxFd() const;
    void acceptConnection();
    void processConnections(fd_set* globalReadSetPtr, fd_set* globalWriteSetPtr);
    void handleRequests(fd_set* globalReadSetPtr);
    void handleResponses(fd_set* globalWriteSetPtr);
    std::list<int>& getReadClients();
    void readError(std::list<int>::iterator & it);
};

#endif
