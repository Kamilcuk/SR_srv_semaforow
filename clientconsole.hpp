#ifndef CLIENTCONSOLE_HPP
#define CLIENTCONSOLE_HPP

#include <string>
#include "servicable.hpp"

class ClientConsole : public Servicable
{
	std::string _serverurl;
public:
	ClientConsole(std::string serverurl);
	void run();
	void sendline(std::string msg);
};

#endif // CLIENTCONSOLE_HPP
