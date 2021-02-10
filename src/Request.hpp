//
// Created by Qsymond on 10.02.2021.
//

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Utils.hpp"

class Request
{
public:
	Request(const std::string& request ) : rawRequest(request) {};
	virtual ~Request() {};
	const std::string & getRawRequest() { return this->rawRequest; }
	void setRawRequest(const std::string & request) { this->rawRequest = request ;}

private:
	std::string rawRequest;
};

#endif
