#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>

class Request
{
	public:
		Request(const std::string& request ) : rawRequest(request) {};
		~Request(void) {};
		const std::string & getRawRequest(void) { return this->rawRequest; }
		void setRawRequest(const std::string & request) { this->rawRequest = request ;}

	private:
		std::string rawRequest;
};

#endif