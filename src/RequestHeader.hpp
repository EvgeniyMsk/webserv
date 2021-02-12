//
// Created by Qsymond on 12.02.2021.
//

#ifndef HEAD_REQ_HPP
#define HEAD_REQ_HPP

#include "Utils.hpp"

class RequestHeader
{
private:
	RequestHeader(const RequestHeader &copy) {(void)copy;}
	RequestHeader &operator=(const RequestHeader &copy) {(void)copy; return *this;}

public:
	~RequestHeader() {}
	RequestHeader() : HOST(LOCALHOST) {}
	// response
	//request
	//to check if ok ??? How to implement
	char        **ACCEPT_CHARSET;
	std::vector<std::string> ACCEPT_LANGUAGE;
	char **AUTHORIZATION;
	//char    **WWW_AUTHENTICATE;
	std::string HOST; //OK + Add PORT according to config_file -> déjà dans la requete
	std::string REFERER; // How to get that > Link ???
	std::string USER_AGENT; // OS, version, application
	std::string REMOTE_ADDR;
	std::string DATE; // HH:MM:SS GMT
	std::string TRANSFER_ENCODING;
	std::string ACCEPT_ENCODING;
	std::string CONTENT_LANGUAGE;
	std::string X_headers;

	//META
	std::string str;
	std::string AUTH_TYPE;
	std::string CONTENT_TYPE;
	std::string CONTENT_LENGTH;
	std::string REQUEST_METHOD;
	std::string QUERY_STRING;
	std::string SERVER_PROTOCOL;
	std::string SCRIPT_NAME;
	std::string PATH_INFO;
	std::string PATH_TRANSLATED;
	std::string REQUEST_URI;
	std::string SERVER_PORT;
	std::string SERVER_NAME;
	// std::string SERVER_SOFTWARE;
	//functions
	std::string getDate();
	std::string getReferer(std::string s);
	std::string getUserAgent(std::string s);
	std::string getTransferEncoding(std::string s);
	void        getRemAddr();
	std::string getStringtoParse(std::string s, std::string toParse);
	std::string getMetatoParse(std::string s, std::string toParse, std::string Sep);
	std::string get_meta();
	//void        getScriptName(std::string s);
	std::string contentNego(std::string root);
	void		parse(std::string s, std::string url);
	std::string getXtoparse(std::string s, std::string toParse);
	void 		getLanguageVector(std::string buffer);
	struct stat st;
};

#endif
