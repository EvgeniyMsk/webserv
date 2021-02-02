//
// Created by Qsymond on 02.02.2021.
//

#ifndef REQUESTHEADERS_HPP
#define REQUESTHEADERS_HPP
#include <string>
class RequestHeaders
{
private:
	RequestHeaders(RequestHeaders const &requestHeaders);
	RequestHeaders &operator=(RequestHeaders const &requestHeaders);
public:
	typedef std::string string;
	RequestHeaders();
	virtual ~RequestHeaders();
	string Accept_Charsets;
	string Accept_Language;
	string Authorization;
	string Content_Language;
	string Content_Length;
	string Content_Location;
	string Content_Type;
	string Date;
	string Host;
	string Last_Modified;
	string Referer;
	string Transfer_Encoding;
	string User_Agent;
};


#endif
