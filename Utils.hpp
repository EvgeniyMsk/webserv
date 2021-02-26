#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <cerrno>
# include <cstring>
# include <cstdlib>
# include <map>
# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>

# define TRUE 1
# define FALSE 1
# define BUFFER_SIZE 100000000

namespace utils
{
    void exitWithLog();
}

struct input
{
    std::string remote_addr;
    std::string root;
    std::map<std::string, std::string> *requestMap;
};

size_t ft_len(char const *s);
char *ft_strdup(char const *s);
std::string trimAfter(std::string const &str, char const &c);
std::pair<std::string, std::string> splitPair(std::string const &str, char const &c);
int get_next_line(int fd, std::string &line);
long findFileSize(int fd);

#endif