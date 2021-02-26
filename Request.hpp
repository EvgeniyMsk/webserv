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
		std::string requestTarget;
		std::string method;
		std::string absolute_root_path_for_request;

		int getStatusCode()
		{
			return (statusCode);
		}

		void setStatusCode(int newStatusCode)
		{
			statusCode = newStatusCode;
		}

		bool isStatusCodeOk()
		{
			if (statusCode != 200)
				return false;
			return true;
		}

		const std::string& getAbsoluteRootPathForRequest(void) const
		{
        return (absolute_root_path_for_request);
    	}
	private:
		std::string rawRequest;
};

#endif