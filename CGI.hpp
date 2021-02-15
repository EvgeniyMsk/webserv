#ifndef CGI_HPP
# define CGI_HPP
# include <iostream>
# include <fcntl.h>
# include <algorithm>
# include <map>
# include <sys/time.h>
# include <array>
# include "Utils.hpp"
# include "Conf.hpp"
# include "Position.hpp"

# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>

class CGI
{
private:
	std::string fileName;
	std::string expand;
	std::string tmpFile;
	char **env;
	char *args[4];
	char *buff;
	input &inp;
	Position position;
	std::string change;
	const Config &config;
	//ответ после обработки
	std::string response;
	std::map<std::string, std::string> envMap;
	std::map<std::string, std::string> responseMap;
    void readCGI();
    void initEnvironments();
    void setUrl();
	void setHttpHeaders();
	void createResponseMap();
    int initARGS();
    int mapEnv();
public:
	CGI();
    CGI(const Config &newConfig, const Position &position, input &newInp);
    virtual ~CGI();
	int run();
	const std::string &getResponse() const;
	std::map<std::string, std::string> getRespMap() const;
};

#endif
