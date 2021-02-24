#include "Utils.hpp"

void utils::exitWithLog(void)
{
    std::cout << std::strerror(errno) << std::endl;
    exit(errno);
}
 
size_t ft_len(char const *s)
{
	char const *str = s;

	while (*str)
		str++;
	return (str - s);
}

char *ft_strdup(char const *s)
{
	char *res = (char *)malloc(ft_len(s) + 1);
	int i = 0;

	if (res)
	{
		for (i = 0; s[i]; ++i)
			res[i] = s[i];
		res[i] = 0;
	}
	return (res);
}

std::string     trimAfter(std::string const &str, char const &c)
{
    size_t delim = str.find(c);

    if (delim != str.npos)
        return str.substr(delim + 1, str.npos);
    return str;
}

std::pair<std::string, std::string> splitPair(std::string const &str, char const &c)
{
	size_t delim = str.find(c);
	std::pair<std::string, std::string> pair;

	if (delim != std::string::npos)
	{
		pair.first = str.substr(0, delim);
		pair.second = str.substr(delim + 1, str.size() - delim - 1);
	} else
		pair.first = str;
	return (pair);
}

int                 get_next_line(int fd, std::string &line)
{
	static std::string  reaminder;
	size_t              endl = reaminder.find('\n');
	ssize_t             len;
	char                *buf;
	int                 rv;

	buf = new char[BUFFER_SIZE + 1];
	while ((rv = read(fd, buf, BUFFER_SIZE)) >= 0)
	{
		buf[rv] = '\0';
		reaminder += buf;
		if (endl != reaminder.npos || rv == 0)
		{
			delete buf;
			if (endl == line.npos)
			{
				if (reaminder.empty())
					return (0);
				endl = line.size();
			}
			line = reaminder.substr(0, reaminder.find('\n'));
			len = line.size() - reaminder.size() + 1;
			reaminder.clear();
			lseek(fd, len, SEEK_CUR);
			return (1);
		}
	}
	delete buf;
	return (-1);
}

long  findFileSize(int fd)
{
    struct stat file_stat;
    int res = fstat(fd, &file_stat);
    if (res < 0)
        return (-1);
    return file_stat.st_size;
}