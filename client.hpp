#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <json/json.h>
#include <string>

class Client
{
	std::string _serverurl;
public:
	Client(std::string serverurl);
	void run();
	Json::Value send(Json::Value msg);
};

#endif // CLIENT_HPP
