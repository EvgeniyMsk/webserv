#include "Response.hpp"

const std::set<std::string> Response::implemented_headers = Response::initResponseHeaders();

const std::map<int, std::string> Response::status_codes = Response::initStatusCodes();

std::set<std::string> Response::initResponseHeaders()
{
	std::set<std::string> implemented_headers;
	implemented_headers.insert("allow"); // Allow: OPTIONS, GET, HEAD
	implemented_headers.insert("content-language"); // Content-Language: en, ase, ru
	implemented_headers.insert("content-length"); // Content-Length: 1348
	implemented_headers.insert("content-location");
	implemented_headers.insert("content-type"); // Content-Type: text/html;charset=utf-8
	implemented_headers.insert("date"); // Date: Tue, 15 Nov 1994 08:12:31 GMT
	implemented_headers.insert("last-modified");
	implemented_headers.insert("location"); // Location: http://example.com/about.html#contacts
	implemented_headers.insert("retry-after");
	implemented_headers.insert("server"); // Server: Apache/2.2.17 (Win32) PHP/5.3.5
	implemented_headers.insert("transfer-encoding"); // Transfer-Encoding: gzip, chunked
	implemented_headers.insert("www-authenticate");
	return implemented_headers;
}

std::map<int,std::string> Response::initStatusCodes()
{
	std::map<int,std::string> status_codes;
	status_codes[100] = "CONTINUE";
	status_codes[101] = "SWITCHING PROTOCOLS";
	status_codes[200] = "OK";
	status_codes[201] = "CREATED";
	status_codes[202] = "ACCEPTED";
	status_codes[203] = "NON-AUTHORITATIVE INFORMATION";
	status_codes[204] = "NO CONTENT";
	status_codes[205] = "RESET CONTENT";
	status_codes[206] = "PARTIAL CONTENT";
	status_codes[300] = "MULTIPLE CHOICES";
	status_codes[301] = "MOVED PERMANENTLY";
	status_codes[302] = "FOUND";
	status_codes[303] = "SEE OTHER";
	status_codes[304] = "NOT MODIFIED";
	status_codes[305] = "USE PROXY";
	status_codes[307] = "TEMPORARY REDIRECT";
	status_codes[400] = "BAD REQUEST";
	status_codes[401] = "UNAUTHORIZED";
	status_codes[402] = "PAYMENT REQUIRED";
	status_codes[403] = "FORBIDDEN";
	status_codes[404] = "NOT FOUND";
	status_codes[405] = "METHOD NOT ALLOWED";
	status_codes[406] = "NOT ACCEPTABLE";
	status_codes[407] = "PROXY AUTHENTICATION REQUIRED";
	status_codes[408] = "REQUEST TIMEOUT";
	status_codes[409] = "CONFLICT";
	status_codes[410] = "GONE";
	status_codes[411] = "LENGTH REQUIRED";
	status_codes[412] = "PRECONDITION FAILED";
	status_codes[413] = "PAYLOAD TOO LARGE";
	status_codes[414] = "URI TOO LONG";
	status_codes[415] = "UNSUPPORTED MEDIA TYPE";
	status_codes[416] = "RANGE NOT SATISFIABLE";
	status_codes[417] = "EXPECTATION FAILED";
	status_codes[426] = "UPGRADE REQUIRED";
	status_codes[500] = "UPGRADE REQUIRED";
	status_codes[501] = "NOT IMPLEMENTED";
	status_codes[502] = "BAD GATEWAY";
	status_codes[503] = "SERVICE UNAVAILABLE";
	status_codes[504] = "GATEWAY TIMEOUT";
	status_codes[505] = "HTTP VERSION NOT SUPPORTED";
	return status_codes;
}

Response::Response(Request* request, int socket) : newRequest(request), newSocket(socket), reason_phrase("OK"), raw_response(""), content("") { };

Response::~Response(void) { };

void Response::sendResponse()
{
	// ответ клиенту
	send(newSocket, raw_response.c_str(), raw_response.length(), 0);
}

void Response::generateStatusLine()
{
	raw_response += "HTTP/1.1 ";
	raw_response += " ";
	raw_response += Response::status_codes[newRequest->getStatusCode()];
	raw_response += "\r\n";
}

struct tm Response::_getCalendarTime(time_t tv_sec)
{
	struct tm calendar_time;
	int days = tv_sec / 86400;
	days += 719468;
	int era = (days >= 0 ? days : days - 146096) / 146097;
	int doe = days - era * 146097;
	int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
	int y = yoe + era * 400;
	int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
	int mp = (5 * doy + 2) / 153;
	int d = doy - (153 * mp + 2) / 5 + 1;
	int m = mp + (mp < 10 ? 3 : -9);

	calendar_time.tm_sec = tv_sec % 60;
	calendar_time.tm_min = tv_sec % 3600 / 60;
	calendar_time.tm_hour = tv_sec % 86400 / 3600;

	calendar_time.tm_mday = d;
	calendar_time.tm_mon = m - 1;
	calendar_time.tm_year = y + (m <= 2) - 1900;

	days = tv_sec / 86400;
	calendar_time.tm_wday = (days >= -4 ? (days + 4) % 7 : (days + 5) % 7 + 6);
	return calendar_time;
}

struct tm Response::_gmtime(time_t tv_sec)
{
	struct tm calendar_time;
	calendar_time = _getCalendarTime(tv_sec);
	return calendar_time;
};

std::string Response::getDateHeader()
{
	char s[30];
	struct tm calendar_time;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	calendar_time = _gmtime(tv.tv_sec);
	strftime(s, sizeof(s), "%a, %d %b %Y %H:%M:%S GMT", &calendar_time);
	std::string date_header;
	date_header.append("date: ").append(s).append("\r\n");
	return (date_header);
}

std::string Response::getLastModifiedHeader(time_t tv_sec)
{
	char s[30];
	struct tm calendar_time;

	calendar_time = _gmtime(tv_sec);
	strftime(s, sizeof(s), "%a, %d %b %Y %H:%M:%S GMT", &calendar_time);

	std::string date_header;
	date_header.append("last-modified: ").append(s).append("\r\n");
	return (date_header);
}

std::string Response::getLocationHeader()
{
	std::string location;

	location += "location: ";
	location += "http://";
	location += newRequest->headers["host"];
	location += newRequest->requestTarget;
	location += '/';
	location += "\r\n";
	return (location);
}

void Response::generateHeaders()
{
	raw_response += "server: webserv\r\n";
	raw_response += getDateHeader();
	raw_response += content_type;
	raw_response += allow;
	raw_response += "Content-Length: ";
	raw_response += "\r\n";
	raw_response += last_modified;
	raw_response += location;
	raw_response += "\r\n";
}

void Response::generateResponseByStatusCode()
{
	content_type = "Content-Type: text/html\r\n";

	generateStatusLine();
	generateHeaders();
	raw_response.append(content);
}

void Response::readFileToContent(std::string & filename)
{
	char buf[1024 + 1];
	int ret;
	int fd;

	fd = open(filename.c_str(), O_RDONLY);
	while ((ret = read(fd, buf, 1024)))
	{
		if (ret < 0)
			utils::exitWithLog();
		content.append(buf, ret);
	}
}

void Response::generateAutoindex()
{
	content = "generated autoindex";
}

void Response::setContentTypeByFilename(std::string & filename)
{
	std::string ext;
	size_t dot_pos;

	dot_pos = filename.rfind('.');
	if (dot_pos == std::string::npos)
		content_type = "Content-Type: application/octet-stream\r\n";
	else
	{
		ext = filename.substr(dot_pos);
		if (ext == ".txt")
			content_type = "Content-Type: text/plain\r\n";
		else if (ext == ".html")
			content_type = "Content-Type: text/html\r\n";
		else if (ext == ".jpg")
			content_type = "Content-Type: image/jpeg;\r\n";
		else
			content_type = "Content-Type: application/octet-stream\r\n";
	}
}

void Response::generateGetResponse()
{
	generateHeadResponse();

	if (!newRequest->isStatusCodeOk())
		return ;

	raw_response += content;
}

void Response::generateHeadResponse()
{
}

void Response::generatePutResponse()
{
    std::cout << "HELLO TARGET: " <<newRequest->requestTarget << std::endl;
    for (int i = 0; i < 100; i++)
        std::cout << std::endl;
    std::cout << "ABS PATH: " << newRequest->getAbsoluteRootPathForRequest() << std::endl;
    generateStatusLine();
    generateHeaders();
    raw_response += content;
}

void Response::generateResponse()
{
	if (newRequest->isStatusCodeOk())
	{
		if (newRequest->method == "GET")
			generateGetResponse();
		else if (newRequest->method == "HEAD")
			generateHeadResponse();
		else if (newRequest->method == "PUT")
			generatePutResponse();
		else
			newRequest->setStatusCode(501); // 501 Not Implemented
	}
	if (!newRequest->isStatusCodeOk())
		generateResponseByStatusCode();
}
