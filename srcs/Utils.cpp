#include "Utils.hpp"

namespace ft
{
	std::string headerToString(const header_w &header)
	{
		switch (header)
		{
			case ACCEPT_CHARSET :
				return ("ACCEPT_CHARSET");
			case ACCEPT_LANGUAGE :
				return ("ACCEPT_LANGUAGE");
			case ALLOW :
				return ("ALLOW");
			case AUTHORIZATION :
				return ("AUTHORIZATION");
			case CONNECTION :
				return ("CONNECTION");
			case CONTENT_LANGUAGE :
				return ("CONTENT_LANGUAGE");
			case CONTENT_LENGTH :
				return ("CONTENT_LENGTH");
			case CONTENT_LOCATION :
				return ("CONTENT_LOCATION");
			case CONTENT_TYPE :
				return ("CONTENT_TYPE");
			case DATE :
				return ("DATE");
			case HOST :
				return ("HOST");
			case LAST_MODIFIED :
				return ("LAST_MODIFIED");
			case LOCATION :
				return ("LOCATION");
			case REFERER :
				return ("REFERER");
			case REMOTE_USER :
				return ("REMOTE_USER");
			case RETRY_AFTER :
				return ("RETRY_AFTER");
			case SERVER :
				return ("SERVER");
			case TRANSFER_ENCODING :
				return ("TRANSFER_ENCODING");
			case USER_AGENT :
				return ("USER_AGENT");
			case WWW_AUTHENTICATE :
				return ("WWW_AUTHENTICATE");
			default:
				return ("INVALID HEADER");
		}
	}

	std::string methodToString(const method_w &method)
	{
		switch (method)
		{
			case GET :
				return ("GET");
			case HEAD :
				return ("HEAD");
			case POST :
				return ("POST");
			case PUT :
				return ("PUT");
			default :
				return ("INVALID METHOD");
		}
	}

	std::vector<method_w> stringToMethod(const std::vector<std::string> &strings)
	{
		std::vector<method_w> result;
		for (std::vector<std::string>::const_iterator it = strings.begin(); it != strings.end(); it++)
		{
			if (*it == "HEAD")
				result.push_back(HEAD);
			else if (*it == "GET")
				result.push_back(GET);
			else if (*it == "POST")
				result.push_back(POST);
			else if (*it == "PUT")
				result.push_back(PUT);
			else throw std::runtime_error("invalid method");
		}
		return (result);
	}

	std::vector<std::string> split(const std::string &s, const std::string &delim)
	{
		size_t start, end = 0;
		std::vector<std::string> vec;

		while (end != std::string::npos)
		{
			start = s.find_first_not_of(delim, end);
			end = s.find_first_of(delim, start);
			if (end != std::string::npos || start != std::string::npos)
				vec.push_back(s.substr(start, end - start));
		}
		return vec;
	}

	static const std::string base64_chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

	static inline bool is_base64(unsigned char c)
	{
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len)
	{
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while (in_len--)
		{
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3)
			{
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while ((i++ < 3))
				ret += '=';
		}
		return ret;
	}

	std::string base64_decode(std::string const &encoded_string)
	{
		int in_len = encoded_string.size();
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;

		while (in_len-- && encoded_string[in_] != '=' && is_base64(encoded_string[in_]))
		{
			char_array_4[i++] = encoded_string[in_];
			in_++;
			if (i == 4)
			{
				for (i = 0; i < 4; i++)
					char_array_4[i] = base64_chars.find(char_array_4[i]);

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 4; j++)
				char_array_4[j] = 0;

			for (j = 0; j < 4; j++)
				char_array_4[j] = base64_chars.find(char_array_4[j]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (j = 0; (j < i - 1); j++)
				ret += char_array_3[j];
		}
		return ret;
	}

	std::string getextension(const std::string &uri)
	{
		size_t dotindex = uri.rfind('.');
		if (dotindex == std::string::npos)
			return "";
		return uri.substr(dotindex, uri.find_first_of('/', dotindex) - dotindex);
	}

	static int finish(std::string &line, int ret, std::string &buf)
	{
		buf = line.substr(line.find_first_of('\n') + 1, line.length() - line.find_first_of('\n') - 1);
		line = line.substr(0, line.find_first_of("\r\n"));
		return (ret > 0);
	}

	int get_next_line(int fd, std::string &line)
	{
		int ret = 1;
		static std::map<int, std::string> m;

		line.clear();
		while (ret > 0)
		{
			line += m[fd];
			if (line.find('\n') != std::string::npos)
			{
				return (finish(line, ret, m[fd]));
			}
			m[fd].clear();
			char *tmp = (char *) malloc(BUFFER_SIZE + 1);
			if (!tmp)
				return (0);
			for (int i = 0; i < BUFFER_SIZE + 1; i++)
				tmp[i] = 0;
			ret = read(fd, tmp, BUFFER_SIZE);
			m[fd].assign(tmp);
			free(tmp);
		}
		return 0;
	}

	int	iswhitespace(char c)
	{
		if (c == ' ' || c == '\f' || c == '\n'
			|| c == '\r' || c == '\t' || c == '\v')
		{
			return (1);
		}
		return (0);
	}

	int	atoi(const char *str)
	{
		int					sign;
		unsigned long int	result;

		result = 0;
		sign = 1;
		while (iswhitespace(*str))
			str++;
		if (*str == '-')
		{
			sign = -1;
			str++;
		}
		else if (*str == '+')
			str++;
		while (*str >= '0' && *str <= '9')
		{
			result = (10 * result) + (*str - '0');
			str++;
		}
		if (result > 9223372036854775807 && sign == -1)
			return (0);
		else if (result >= 9223372036854775807)
			return (-1);
		return (sign * result);
	}

	long	atol(const char *str)
	{
		int		sign;
		long	result;

		result = 0;
		sign = 1;
		while (iswhitespace(*str))
			str++;
		if (*str == '-')
		{
			sign = -1;
			str++;
		}
		else if (*str == '+')
			str++;
		while (*str >= '0' && *str <= '9')
		{
			result = (10 * result) + (*str - '0');
			str++;
		}
		if (result > LONG_MAX && sign == -1)
			return (0);
		else if (result >= LONG_MAX)
			return (-1);
		return (sign * result);
	}

	int		is_first_char(std::string str, char find)
	{
		int i = 0;
		while (str[i] && iswhitespace(str[i]))
			++i;
		if (str[i] == find)
			return (1);
		return (0);
	}

	void	get_key_value(std::string &str, std::string &key, std::string& value, const char* delim, const char* end) {
		size_t kbegin = str.find_first_not_of(delim);
		size_t kend = str.find_first_of(delim, kbegin);
		key = str.substr(kbegin, kend - kbegin);
		size_t vbegin = str.find_first_not_of(delim, kend);
		size_t vend = str.find_first_of(end, vbegin);
		value = str.substr(vbegin, vend - vbegin);
	}

	std::string inttostring(int n) {
		std::string ss;

		if (n == 0)
			ss = "0";
		while (n) {
			char i = '0' + (n % 10);
			n /= 10;
			ss = i + ss;
		}
		return ss;
	}
	void	stringtoupper(std::string& str) {
		for (size_t i = 0; i < str.length(); ++i) {
			str[i] = toupper(str[i]);
		}
	}

	void	trimstring(std::string& str, const char* totrim) {
		size_t	begin = str.find_first_not_of(totrim),
				end = str.find_last_not_of(totrim);
		if (begin == std::string::npos || end == 0)
			return;
		str = str.substr(begin, end - begin + 1);
	}

	int findNthOccur(std::string str, char ch, int N)
	{
		int occur = 0;

		for (int i = (str.length()); i >= 0; i--) {
			if (str[i] == ch) {
				occur += 1;
			}
			if (occur == N)
				return i;
		}
		return -1;
	}

	time_t	getTime() {
		struct timeval tv = {};
		struct timezone tz = {};

		gettimeofday(&tv, &tz);
		return (tv.tv_usec);
	}

	void	bzero(void *s, size_t n)
	{
		size_t			i;
		unsigned char	*ptr;

		ptr = (unsigned char *)s;
		i = 0;
		while (i < n)
		{
			ptr[i] = 0;
			i++;
		}
	}

	void	*calloc(size_t count, size_t size)
	{
		void *ptr;

		if (count == 0 || size == 0)
		{
			count = 1;
			size = 1;
		}
		ptr = malloc(count * size);
		if (ptr)
			bzero(ptr, count * size);
		return (ptr);
	}

	char	*strdup(const char *s1)
	{
		char	*dup;
		size_t	i;

		i = strlen(s1) + 1;
		dup = (char*)malloc(i);
		if (dup == NULL)
			return (NULL);
		memcpy(dup, s1, i);
		return (dup);
	}
}
