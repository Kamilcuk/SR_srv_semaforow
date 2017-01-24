#ifndef SERVER_HPP
#define SERVER_HPP

#include "servicable.hpp"
#include <string>
#include "serverserver.hpp"

class Server : public Servicable
{
	std::string _serverurl;
	int _port;
	ServerServer _serverserver;
	myhttpserver _serverserverhttpserver;
	boost::thread _serverserverthread;


	/// uuids->Sempaphores
	std::map<std::string,Semaphore> sems;

public:
	Server(std::string serverurl, int port);
	void run();
	void stop();
	static bool CheckClient(std::string c,std::string &errorstr);

	void operatorO_parseJson_in(httpserverresponse &res, Json::Value root);
	std::vector<std::string> getAllOwners();
	void HeartbeatScan();
	void DisconnectClient(std::string o);
};

#endif // SERVER_HPP
