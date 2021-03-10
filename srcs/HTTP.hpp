#ifndef HTTP_HPP
#define HTTP_HPP

#include <vector>
#include <dirent.h>
#include <sys/stat.h>

#include "Utils.hpp"
#include "Client.hpp"

class Client;

class HTTP
{
	std::map<std::string, std::string> MIMETypes;
public:
	HTTP();

	virtual ~HTTP();

	void parseRequest(Client &client, std::vector<Config> &config);

	void parseBody(Client &client);

	void dispatcher(Client &client);

	void createResponse(Client &client);

private:
	void handleGet(Client &client);

	void handleHead(Client &client);

	void handlePost(Client &client);

	void handlePut(Client &client);

	void handleConnect(Client &client);

	void handleTrace(Client &client);

	void handleOptions(Client &client);

	void handleDelete(Client &client);

	void handleBadRequest(Client &client);

	void getConf(Client &client, Request &req, std::vector<Config> &conf);

	void negotiate(Client &client);

	void createListing(Client &client);

	bool checkSyntax(const Request &request);

	bool parseHeaders(std::string &buf, Request &req);

	void getBody(Client &client);

	void dechunkBody(Client &client);

	void execCGI(Client &client);

	void parseCGIResult(Client &client);

	std::string getLastModified(std::string path);

	std::string findType(Client &client);

	void getErrorPage(Client &client);

	int findLen(Client &client);

	void fillBody(Client &client);

	int fromHexa(const char *nb);

	char **setEnv(Client &client);

	std::string decode64(const char *data);

	void parseAcceptLanguage(Client &client, std::multimap<std::string, std::string> &map);

	void parseAcceptCharset(Client &client, std::multimap<std::string, std::string> &map);

	void assignMIME();

	int getStatusCode(Client &client);

	int GETStatus(Client &client);

	int POSTStatus(Client &client);

	int PUTStatus(Client &client);

	int CONNECTStatus(Client &client);

	int TRACEStatus(Client &client);

	int OPTIONSStatus(Client &client);

	int DELETEStatus(Client &client);
};

#endif
