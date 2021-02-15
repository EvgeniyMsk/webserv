# ifndef POSITION_HPP
# define POSITION_HPP
# include <string>
# include <vector>
# include <iostream>

class Position
{
private:
	std::string pos;
	std::string root;
	std::string cgiScrypt;

public:
	Position() {};
	~Position() {};
	Position &operator=(const Position &target);
	Position(const Position &copy);
	const std::string &getRoot() const;
	const std::string &getCgiScrypt() const;
};

#endif