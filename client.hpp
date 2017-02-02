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
	struct SemaphoreInfo
	{
		int value = 0;
		std::string uuid;
		std::string status;
		SemaphoreInfo(std::string _uuid) : uuid(_uuid) {}
		void increment() { value++; }
		void decrement() { value--; }
	};

	const std::string _serverurl;
	const std::string _owner;
	const unsigned int _port;

	ClientConsole _clientconsole;

	ClientServer _clientserver;
	myhttpserver _clientserverhttpserver;
	std::vector<boost::thread> _clientserverthreads;

	/// uuid -> SemaphoreInfo
	std::mutex semsmutex;
	std::map<std::string, SemaphoreInfo> sems;

	bool deadlock = false;
public:
	Client(std::string serverurl, std::string clientip, unsigned int port, unsigned int threads = 2);

	std::string Send_LocksCreateSemaphore(int CurrentVal, int MinVal, int MaxVal);
	std::string Send_MaintenanceHello(std::string name);
	std::string Send_LocksDeleteSemaphore(std::string uuid);
	std::string Send_LocksDecrementSemaphore(std::string uuid);
	std::string Send_LocksIncrementSemaphore(std::string uuid);
	std::string Send_LocksDump();
	std::string Send_LocksProbe(std::string InitiatorAddr, std::__cxx11::string uuid);

	void SendLocksToAllServersWhenClientProbe(std::string InitiatorAddr);

	static std::string sendStrToServerStr(std::string serverurl, std::string bodymsg, unsigned int Timeout = 5);
	static Json::Value sendStrToServer(std::string serverurl, std::string msg, unsigned int Timeout = 5);
	static Json::Value sendToServer(std::string serverurl, Json::Value msg, unsigned int Timeout = 5);
	static std::string Send_ClientProbe_Static(std::string clienturl, std::string InitiatorAddr);
	Json::Value send(Json::Value msg, unsigned int Timeout = 5);

	void run();
	void stop();

	std::map<std::string, SemaphoreInfo> getSems() const;
	std::string getOwner() const;
	bool getDeadlock() const;
	void setDeadlock(bool value);
	std::string Send_LocksProbe_all(std::string ReturnAddr, std::string InitiatorAddr, std::string uuid);
	void SafeExit();
};

#endif // CLIENT_HPP
