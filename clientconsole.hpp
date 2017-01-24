#ifndef CLIENTCONSOLE_HPP
#define CLIENTCONSOLE_HPP

#include <string>
#include "servicable.hpp"
#include "json/json.h"
class Client;
class ClientConsole : public Servicable
{
	std::string _serverurl;
	Client *_client;
public:
	ClientConsole(Client *client, std::string serverurl);
	void run();
	void sendline(std::string msg);
	Json::Value sendlineJsonJson(Json::Value msg);
	void one_run();
};

#endif // CLIENTCONSOLE_HPP
