//
// Created by Qsymond on 02.02.2021.
//

#ifndef RESPONSEHEADERS_HPP
#define RESPONSEHEADERS_HPP

#include <string>
class ResponseHeaders
{
private:
	ResponseHeaders(ResponseHeaders const &responseHeaders);
	ResponseHeaders &operator=(ResponseHeaders const &responseHeaders);
public:
	typedef std::string string;
	ResponseHeaders();
	virtual ~ResponseHeaders();
	string Allow;
	string Content_Language;
	string Content_Length;
	string Content_Location;
	string Content_Type;
	string Date;
	string Last_Modified;
	string Location;
	string Retry_After;
	string Server;
	string Transfer_Encoding;
	string WWW_Authenticate;
};


#endif //WEBSERV_RESPONSEHEADERS_HPP
