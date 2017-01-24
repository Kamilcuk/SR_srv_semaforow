#ifndef SERVER_HPP
#define SERVER_HPP

#include "servicable.hpp"
#include <string>
#include "serverserver.hpp"

#include "semaphore.hpp"

class Server : public Servicable
{
	std::string _serverurl;
	int _port;
	ServerServer _serverserver;
	myhttpserver _serverserverhttpserver;
	std::vector<boost::thread> _serverserverthreads;

	/// uuids->Sempaphores reference wrapper -  i believe i can fly..
	std::mutex semsmutex;
	std::map< std::string, std::unique_ptr<Semaphore> > sems;

public:
	Server(std::string serverurl, int port, int threadsnum = 5);
	void run();
	void stop();
	static bool CheckClient(std::string c,std::string &errorstr);

	void operatorO_parseJson_in(httpserverresponse &res, Json::Value root);
	std::vector<std::string> getAllOwners();
	void HeartbeatScan();
	void DisconnectClient(std::string o);

	std::map<std::string, SemaphoreData> getSemsData();
};

#endif // SERVER_HPP
