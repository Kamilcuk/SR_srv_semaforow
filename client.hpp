#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <json/json.h>
#include <string>

class Client
{
	std::string _serverurl;
	std::string _owner;
public:
	Client(std::string serverurl);
	void run();
	Json::Value send(Json::Value msg);
	std::string Send_LocksCreateSemaphore(int CurrentVal, int MinVal, int MaxVal);
	std::string Send_MaintenanceHello(std::string name);
	std::string Send_LocksDeleteSemaphore(std::string uuid);
	std::string Send_LocksDecrementSemaphore(std::string uuid);
	std::string Send_LocksIncrementSemaphore(std::string uuid);
	std::string Send_LocksDump();
};

#endif // CLIENT_HPP
