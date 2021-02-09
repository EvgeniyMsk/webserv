#include "Conf.hpp"
#include "get_next_line/get_next_line.hpp"

Config::Config(const std::string &path_to_config)
{
    int file = open(path_to_config.c_str(), O_RDONLY);
    if (file < 0)
        utils::exitWithLog();
    std::cout << "Фaйл успешно открылся" << std::endl;

    char *str;
    get_next_line(file, &str);
    std::cout << str << std::endl;
}