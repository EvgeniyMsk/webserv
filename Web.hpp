#ifndef WEB_HPP
# define WEB_HPP

# include <vector>
# include "Server.hpp"
# include "Conf.hpp"

# define CONFIG_FILE_DEFAULT_PATH "/web.conf"

class Web
{
private:
    std::vector<Server*> servers;
    // Для множества сокетов
    fd_set readSet;
    fd_set writeSet;
    int maxFd;
    Config conf;
public:
    Web() {}
    Web(const std::string& config_file_path);
    void addServer(Server* server);
    int getServersCount() const;
    Server* getServerByPosition(int i);

    fd_set* getReadSetPtr();
    fd_set* getWriteSetPtr();

    void setToReadFDSet(std::list<int>& clientsFD);
    void updateMaxFD();
    const int &getMaxFD() const;
	void serveConnections();
	void stop();
};

#endif
