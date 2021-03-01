#ifndef CGI_HPP
#define CGI_HPP
#include <map>
#include <string>
#include "Request.hpp"

class Cgi
{
public:
	Cgi();

	Cgi(const Cgi &);

	Cgi &operator=(const Cgi &);

	virtual ~Cgi();

	int run_cgi(request_s &, std::string &scriptpath, const std::string &Uri);

private:
	void initEnvMap(request_s &, const std::string &, bool redirect_status, std::string &scriptpath);

	void mapToEnv(request_s &request);

	void clearEnv();

	std::map<std::string, std::string> environment_map;
	char **environment;
};

#endif
