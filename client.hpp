#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <json/json.h>
#include <string>
#include "clientserver.hpp"
#include <thread>
#include <boost/thread.hpp>
#include "clientconsole.hpp"
#include "servicable.hpp"

class Client : public Servicable
{
	std::string _serverurl;
	std::string _owner;
	unsigned int _port;

	ClientConsole _clientconsole;

	ClientServer _clientserver;
	myhttpserver _clientserverhttpserver;
	boost::thread _clientserverthread;
public:
	Client(std::string serverurl, std::string clientip, unsigned int port);

	Json::Value send(Json::Value msg);
	std::string Send_LocksCreateSemaphore(int CurrentVal, int MinVal, int MaxVal);
	std::string Send_MaintenanceHello(std::string name);
	std::string Send_LocksDeleteSemaphore(std::string uuid);
	std::string Send_LocksDecrementSemaphore(std::string uuid);
	std::string Send_LocksIncrementSemaphore(std::string uuid);
	std::string Send_LocksDump();

	static std::string sendStrToServerStr(std::string serverurl, std::string bodymsg);
	static Json::Value sendStrToServer(std::string serverurl, std::string msg);
	static Json::Value sendToServer(std::string serverurl, Json::Value msg);

	void run();
	void stop();
};

#endif // CLIENT_HPP
