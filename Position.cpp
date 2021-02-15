#include "Position.hpp"

const std::string &Position::getRoot() const
{
    return root;
}

const std::string &Position::getCgiScrypt() const
{
    return cgiScrypt;
}

Position &Position::operator=(const Position &target)
{
	pos = target.pos;
	root = target.root;
	cgiScrypt = target.cgiScrypt;
	return (*this);
}

Position::Position(const Position &copy)
{
	this->operator=(copy);
}