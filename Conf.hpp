#ifndef CONF_HPP
#define CONF_HPP

# include <string>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include "Utils.hpp"

class Config
{
public:
    Config() {}
    Config(const std::string &path_to_config);
};

#endif
