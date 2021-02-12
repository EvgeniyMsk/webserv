//
// Created by Qsymond on 12.02.2021.
//

#include "RequestHeader.hpp"

static void		*ft_free(char **str, int size)
{
	int	i;

	i = 0;
	while (size > i)
		free(str[i++]);
	free(str);
	return (0);
}

static size_t	ft_countw(const char *s, char c)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] != c)
			count++;
		while (s[i] != c && s[i + 1])
			i++;
		i++;
	}
	return (count);
}

static size_t	ft_lenw(const char *s, char c)
{
	size_t	i;
	size_t	len;

	i = 0;
	len = 0;
	while (s[i] == c)
		i++;
	while (s[i] != c && s[i])
	{
		len++;
		i++;
	}
	return (len);
}

char			**ft_split(char const *s, char c)
{
	size_t	i;
	size_t	j;
	size_t	k;
	char	**str;

	i = 0;
	k = 0;
	if (!s || !(str = (char **)malloc(sizeof(char *) * (ft_countw(s, c) + 1))))
		return (NULL);
	while (i < ft_countw(s, c))
	{
		if (!(str[i] = (char *)malloc(sizeof(char) * (ft_lenw(&s[k], c) + 1))))
		{
			ft_free(str, k);
			return (NULL);
		}
		j = 0;
		while (s[k] == c)
			k++;
		while (s[k] != c && s[k])
			str[i][j++] = s[k++];
		str[i++][j] = '\0';
	}
	str[i] = NULL;
	return (str);
}

char	*ft_strstr(const char *haystack, const char *needle)
{
	size_t i;
	size_t j;

	i = 0;
	if (needle[0] == '\0')
		return ((char*)haystack);
	while (haystack[i] != '\0')
	{
		j = 0;
		while (haystack[i + j] == needle[j] && needle[j])
			j++;
		if (needle[j] == '\0')
			return ((char *)haystack + i);
		i++;
	}
	return (NULL);
}

void	ft_tabdel(void **tab)
{
	int i;

	i = 0;
	if (tab)
	{
		while (tab[i])
			free(tab[i++]);
		free(tab);
	}
}

std::string RequestHeader::get_meta()

{
	memset((char *) &str, 0, sizeof(str));
	str.append("&HTTP_HOST=");
	str.append(HOST);
	str.append("&HTTP_REFERER=");
	str.append("");
	str.append("&HTTP_USER_AGENT=");
	str.append(USER_AGENT);
	str.append("&HTTP_ACCEPT_ENCODING=");
	str.append(ACCEPT_ENCODING);
	str.append("&HTTP_TRANSFER_ENCODING=");
	str.append(TRANSFER_ENCODING);
	str.append("&AUTH_TYPE=");
	str.append(AUTH_TYPE);
	str.append("&CONTENT_LENGTH=");
	str.append(CONTENT_LENGTH);
	str.append("&CONTENT_TYPE=");
	str.append(CONTENT_TYPE);
	str.append("&GATEWAY_INTERFACE=");
	str.append("CGI/1.1");
	str.append("&PATH_INFO=");
	str.append(PATH_INFO);
	str.append("&PATH_TRANSLATED=");
	str.append(PATH_TRANSLATED);
	str.append("&QUERY_STRING="); // if nothing ?
	str.append(QUERY_STRING);
	str.append("&REMOTE_ADDR="); //AJOUTER avec l'adressse IP
	str.append(REMOTE_ADDR);
	str.append("&REMOTE_USER="); //AJOUTER en dur
	str.append("user");
	str.append("&REMOTE_IDENT=");
	str.append("login_user");
	str.append("&REQUEST_METHOD=");
	str.append(REQUEST_METHOD);
	str.append("&REQUEST_URI=");
	str.append(REQUEST_URI);
	str.append("&SCRIPT_NAME=");
	str.append(SCRIPT_NAME);
	str.append("&SERVER_NAME="); // to take
	str.append(SERVER_NAME);
	str.append("&SERVER_PORT="); // to take
	str.append(SERVER_PORT);
	str.append("&SERVER_PROTOCOL=");
	str.append(SERVER_PROTOCOL);
	str.append("&SERVER_SOFTWARE=");
	str.append("webserv"); // A modif ?
	std::string tmp = "";
	for (size_t i = 0; i != X_headers.size(); i++)
	{
		if (X_headers.at(i) == ':')
			tmp+= '=';
		else if (X_headers.at(i) == 'X' && X_headers.at(i+1) == '-')
		{
			tmp += '&';
			i++;
		}
		else if (X_headers.at(i) == '-')
			tmp += '_';
		else if (X_headers.at(i) != ' ')
			tmp += std::toupper(X_headers.at(i));
	}
	char **head_split = ft_split(tmp.c_str(), '&');
	int j = 0;
	while (head_split && head_split[j])
	{
		str.append("&HTTP_X_");
		str.append(head_split[j]);
		j++;
	}
	j = 0;
	while (head_split[j])
	{
		free(head_split[j]);
		j++;
	}
	free(head_split);
	return str;
}

std::string RequestHeader::getMetatoParse(std::string s, std::string toParse, std::string Sep)
{
	int n;
	std::string referer;
	const char *c_sep = Sep.c_str();
	n = s.find(toParse);
	if (n != (int)std::string::npos)
	{
		n = n + std::string(toParse).size();
		int i = n;
		while (s[i] != '\0')
		{
			int j = 0;
			while(s[i] && s[i] != c_sep[j] && c_sep[j] != '\0')
			{
				j++;
			}
			if (j != (int)strlen(c_sep))
				break;
			i++;
		}
		referer = s.substr(n, i - n);
		return referer;
	}
	return "";
}

std::string RequestHeader::getStringtoParse(std::string s, std::string toParse)
{
	int n;
	std::string referer;
	n = s.find(toParse);
	if (n != (int)std::string::npos)
	{
		n = n + std::string(toParse).size();
		int i = n;
		while (s[i] && s[i] != '\n' && s[i] != '\r') { i++;}
		referer = s.substr(n, i - n);
		return referer;
	}
	return "";
}

std::string RequestHeader::getReferer(std::string s)
{
	int n;
	std::string referer;
	n = s.find(REFERER_STR);
	if (n != (int)std::string::npos)
	{
		n = n + std::string(REFERER_STR).size();
		int i = n;
		while (s[i] && s[i] != '\n') { i++;}
		referer = s.substr(n, i - n);
	}
	return referer;
}

std::string RequestHeader::getUserAgent(std::string s)
{
	int n;
	std::string referer;
	n = s.find(USER_AGENT_STR);
	if (n != (int)std::string::npos)
	{
		n = n + std::string(USER_AGENT_STR).size();
		int i = n;
		while (s[i] && s[i] != '\r' && s[i] != '\n') { i++;}
		referer = s.substr(n, i - n);
	}
	return referer;
}

std::string RequestHeader::getTransferEncoding(std:: string s)
{
	int n;
	std::string referer;
	n = s.find(TRANSFER_EN_STR);
	if (n != (int)std::string::npos)
	{
		n = n + std::string(TRANSFER_EN_STR).size();
		int i = n;
		while (s[i] && s[i] != '\r') { i++;}
		referer = s.substr(n, i - n);
	}
	return referer;
}

std::string RequestHeader::contentNego(std::string root) {
	size_t i = 0;
	CONTENT_LANGUAGE = "fr";
	if (ACCEPT_LANGUAGE.empty())
		return root + "fr/";
	std::string res = root + "fr/";
	for (std::vector<std::string>::iterator it = ACCEPT_LANGUAGE.begin(); it!=ACCEPT_LANGUAGE.end(); ++it) {
		std::string tmp = *it;
		if ((i = tmp.find(';')) != std::string::npos)
		{
			tmp = tmp.substr(0, i);
		}
		std::ifstream f(root + tmp + "/");

		if (f.good()) {
			CONTENT_LANGUAGE = tmp;
			res = root + tmp + "/";
			f.close();
			break;
		}
		f.close();
	}
	return res;
}

std::string RequestHeader::getXtoparse(std::string s, std::string toParse)
{
	int n;
	std::string tmp(s);
	std::string referer = "";
	n = tmp.find(toParse);
	while (n != (int)std::string::npos)
	{
		int i = n;
		while (tmp[i] != '\n' && tmp[i] != '\r') { i++;}
		referer += tmp.substr(n, i - n);
		tmp = tmp.substr(i, tmp.size() - i);
		n = tmp.find(toParse);
	}
	return referer;
}

void		RequestHeader::parse(std::string s, std::string url) {
	if (ft_strstr(s.c_str(), "X-") != NULL)
	{
		X_headers = getXtoparse(s, "X-");
	}
	char **tab = ft_split(getStringtoParse(s, AUTH_STR).c_str(), ' ');
	if (tab != NULL && tab[0] != NULL)
		AUTH_TYPE = tab[0];
	ACCEPT_ENCODING = getStringtoParse(s, ACCEPT_EN_STR);
	CONTENT_TYPE = getStringtoParse(s, CONTENT_T_STR);
	if (ft_strstr(s.c_str(), CONTENT_L_STR) != NULL)
		CONTENT_LENGTH = getStringtoParse(s, CONTENT_L_STR);
	QUERY_STRING = getMetatoParse((char *)url.c_str(), "?", (char *)" #");
	SERVER_NAME = getMetatoParse((char *)url.c_str(), "://", ":/?#");
	if (getMetatoParse((char*)url.c_str(), SERVER_NAME + ":", "?/#") != "")
		SERVER_PORT = getMetatoParse((char*)url.c_str(), SERVER_NAME + ":", "?/#") != "";
	REFERER = getReferer(s);
	USER_AGENT = getUserAgent(s);
	if (getStringtoParse(s, ACCEPT_CHAR_STR) != "")
		ACCEPT_CHARSET = ft_split(getStringtoParse(s, ACCEPT_CHAR_STR).c_str(), ',');
	if (ft_strstr(s.c_str(), TRANSFER_EN_STR) != NULL)
		TRANSFER_ENCODING = getTransferEncoding(s);
	DATE = getStringtoParse(s, DATE_STR);
	getLanguageVector(s);
	ft_tabdel((void **)tab);
}

void RequestHeader::getLanguageVector(std::string buffer)
{
	std::string lg = getStringtoParse(buffer, ACCEPT_LAN_STR);
	if (lg == "")
		return;
	std::string copy = lg;
	size_t pos = 0;
	while ((pos = copy.find(" ")) != std::string::npos) {
		ACCEPT_LANGUAGE.push_back(copy.substr(0, pos));
		copy.erase(0, pos + 1);
	}
	ACCEPT_LANGUAGE.push_back(copy);
}