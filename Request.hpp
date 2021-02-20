#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>

class Request
{
	public:
		Request(const std::string& request ) : rawRequest(request) {};
		~Request(void) {};
		const std::string & getRawRequest(void) { return this->rawRequest; }
		void setRawRequest(const std::string & request) { this->rawRequest = request ;}
		std::map<std::string, std::string> headers;
		int statusCode;
		std::string method;
		void setStatusCode(int newStatusCode)
		{
			statusCode = newStatusCode;
		}
	private:
		std::string rawRequest;
};

#endif