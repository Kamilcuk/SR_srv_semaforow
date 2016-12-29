#ifndef UUID_HPP
#define UUID_HPP

#include <string>

class Uuid
{
public:
	Uuid();
	static std::string getRandom();
};

#endif // UUID_HPP
