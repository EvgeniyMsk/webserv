#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "Cgi.hpp"
#include <dirent.h>
#include <vector>

class Response {
	std::map<header_w, std::string>	_header_vals;
	std::map<header_w, std::string>	_cgi_headers;
	int									_cgi_status_code;
	std::map<int, std::string>			_status_codes;
	std::string							_response;
	std::string							_body;
	bool								_autoindex;
	std::vector<std::string>			_autoindex_root;
	int									_status_code;
	Cgi									CGI;
public:
	friend class Webserv;
	Response();
	virtual ~Response();
	Response(const Response &src);
	Response& operator= (const Response &rhs);

	std::string&	handleRequest(request_s& request);
	void		handleBody(request_s& request);
	void 		handle404(request_s& request);
	void		handleAutoIndex(request_s& request);
	void		handleStatusCode(request_s& request);
	void		handleALLOW(request_s& request);
	void		handleCONTENT_LANGUAGE( );
	void		handleCONTENT_LENGTH( );
	void		handleCONTENT_LOCATION( );
	void		handleCONTENT_TYPE(request_s& request);
	void		handleDATE( );
	void		handleLOCATION( std::string& url );
	void		handleSERVER( );
	void		handleCONNECTION_HEADER(const request_s& request);

	void		extractCGIheaders(const std::string&);
	int			authenticate(request_s& request);
	void 		generateResponse(request_s& request);
	int			generatePage(request_s& request);
	void		handlePut(request_s& request);
	int 		handlePost(request_s& request, std::string& filepath, bool validfile);
	void		negotiateLanguage(request_s& request);
};


#endif
