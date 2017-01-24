#include "server.hpp"
#include "helpers.hpp"
#include "unistd.h"
#include <json/json.h>
#include <boost/network/protocol/http/response.hpp>
#include "abstractserver.hpp"
#include <map>
#include <string>
#include <iostream>
#include "helpers.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "client.hpp"

Server::Server(std::string serverurl, int port) :
	_serverurl(serverurl),
	_port(port),
	_serverserver(this),
	_serverserverhttpserver(
		myhttpserver::options(_serverserver).address(_serverurl).port(std::to_string(_port))
	)
{
	_serverserverhttpserver.run();
	_serverserverthread = boost::thread(
			boost::bind(&decltype(_serverserverhttpserver)::run, &_serverserverhttpserver)
	);
}

bool Server::CheckClient(std::string c,std::string &errorstr)
{
	try {
		std::string realc = "http://"+c+"/v1";
		//DEBUGMSG("%s \n", realc.c_str());

		Json::Value root;   // will contains the root value after parsing.
		root["method"] = "Maintenance.Heartbeat";
		Json::Value ret = Client::sendToServer(realc, root);
		if ( ret["method"] != "Maintenance.Heartbeat" ) {
			errorstr = "ret[\"method\"] != \"Maintenance.Heartbeat\"";
			return false;
		}
		return true;
	} catch(const std::exception &e) {
		std::cerr << e.what();
		errorstr = "exception while checking client:"+std::string(e.what());
	}
	return false;
}

std::vector<std::string> Server::getAllOwners()
{
	std::vector<std::string> ret;
	for(auto const& isems : sems) {
		std::vector<std::string> owners = isems.second.owners;
		for(auto const& o : owners) {
			ret.push_back(o);
		}
	}
	return ret;
}

void Server::DisconnectClient(std::string o)
{
	for(auto& isems : sems) {
		std::vector<std::string> owners = isems.second.owners;
		for(auto& realo : owners) {
			if ( !realo.compare(o) ) {
				Semaphore &s = isems.second;
				DEBUGMSG("Incrementing semaphore %s with owner %s \n",s.uuid.c_str(), o.c_str());
				s.increment(o);
			}
		}
	}
}

void Server::HeartbeatScan()
{
	DEBUGMSG("initialiting heartbeat detection\n");
	std::vector<std::string> v = getAllOwners();
	std::vector<std::string> uniquev = v;
	std::sort(uniquev.begin(),uniquev.end());
	std::unique(uniquev.begin(), uniquev.end());
	for(auto const &o : uniquev) {
		std::string errorstr;
		if ( !Server::CheckClient(o, errorstr) ) {
			DEBUGMSG("No connection with client: %s errostr=%s \n", o.c_str(), errorstr.c_str());
			DisconnectClient(o);
		}
	}
}

void Server::run()
{
	int i=0;
	while(!_stop) {
		sleep(1);
		i=i+1;
		if ( i == 5 ) {
			//HeartbeatScan();
			i=0;
		}
	}
	_serverserverhttpserver.stop();
	_serverserverthread.join();
}

void Server::stop()
{
	DEBUGMSG(" ");
	Servicable::stop();
}


void Server::operatorO_parseJson_in(httpserverresponse &res, Json::Value root)
{
	// main switch, yay
	if ( !root["method"].compare("Maintenance.Hello") ) {
		return responseWithJsonTo(res, root,
						 "Message", "Hello, "+root["params"][0]["Name"].asString());
	} else
	if ( !root["method"].compare("Maintenance.Heartbeat") ) {
		return responseWithJsonTo(res, root);
	} else
	if ( !root["method"].compare("Locks.CreateSemaphore") ) {
		int CurrentVal;
		int MinVal;
		int MaxVal;
		try {
			CurrentVal	 = std::stoi(root["params"][0]["CurrentVal"].asString());
			MinVal		 = std::stoi(root["params"][0]["MinVal"].asString());
			MaxVal		 = std::stoi(root["params"][0]["MaxVal"].asString());
		} catch(std::invalid_argument) {
			return responseWithJsonTo(res, root, "ERROR", "conversion could be performed");
		} catch(std::out_of_range) {
			return responseWithJsonTo(res, root, "ERROR", "the converted value would fall out of the range of the result type or if the underlying function (std::strtol or std::strtoll) sets errno to ERANGE.");
		}

		DEBUGMSG("cur=%d min=%d max=%d \n", CurrentVal, MinVal, MaxVal);

		if ( CurrentVal == MinVal ) {
			return responseWithJsonTo(res, root, "ERROR", "CurrentVal == MinVal");
		} else
		if ( MinVal > MaxVal ) {
			return responseWithJsonTo(res, root, "ERROR", "MinVal > MaxVal");
		}

		std::string u;
		do {
			u = getUuidStr();
		} while( sems.find(u) != sems.end() );
		sems.insert(std::make_pair(u, Semaphore(CurrentVal, MinVal, MaxVal, u)));
		return responseWithJsonTo(res, root, "UUID", u);
	} else
	if ( !root["method"].compare("Locks.DeleteSemaphore") ) {
		std::string u = root["params"][0]["UUID"].asString();
		auto isem = sems.find(u);
		if ( isem == sems.end() ) {
			return responseWithJsonTo(res, root, "ERROR", "no such semaphores with guven uuid - "+u);
		}
		sems.erase(u);
		return responseWithJsonTo(res, root, "UUID", u);
	} else
	if ( !root["method"].compare("Locks.DecrementSemaphore") ) {
		std::string u = root["params"][0]["UUID"].asString();
		std::string o = root["params"][0]["Owner"].asString();
		if ( o.empty() ) {
			return responseWithJsonTo(res, root, "ERROR", "no owner given in the json");
		}
		auto isem = sems.find(u);
		if ( isem == sems.end() ) {
			return responseWithJsonTo(res, root, "ERROR", "no such semaphore with guven uuid - "+u);
		}

		std::string errorstr;
		if ( !Server::CheckClient(o, errorstr) ) {
			return responseWithJsonTo(res, root, "ERROR", "CheckClient: "+errorstr);
		}

		isem->second.decrement(o);
		return responseWithJsonTo(res, root, "UUID", u);
	}else
	if ( !root["method"].compare("Locks.IncrementSemaphore") ) {
		std::string u = root["params"][0]["UUID"].asString();
		std::string o = root["params"][0]["Owner"].asString();
		if ( o.empty() ) {
			return responseWithJsonTo(res, root, "ERROR", "no owner given in the json");
		}
		auto isem = sems.find(u);
		if ( isem == sems.end() ) {
			return responseWithJsonTo(res, root, "ERROR", "no such semaphores with guven uuid - "+u);
		}
		std::string errorstr;
		if ( !Server::CheckClient(o, errorstr) ) {
			return responseWithJsonTo(res, root, "ERROR", "CheckClient: "+errorstr);
		}

		isem->second.increment(o);
		return responseWithJsonTo(res, root, "UUID", u);
	}else
	if ( !root["method"].compare("Locks.Dump") ) {
		Json::Value root;
		root["method"] = "Locks.Dump";
		unsigned int i=0;
		for(auto const isems : sems) {
			root["params"][i]["UUID"] = isems.first;
			Semaphore s = isems.second;
			root["params"][i]["owners_size"] = s.owners.size();
			for(unsigned int j=0; j<s.owners.size(); ++j) {
				root["params"][i]["owners"][j] = s.owners[j];
			}
			root["params"][i]["CurrentVal"] = s.cur;
			root["params"][i]["MaxVal"] = s.max;
			root["params"][i]["MinVal"] = s.min;
			i++;
		}
		return fillrespOKJson(res, root);
	}else
	if ( !root["method"].compare("Locks.Probe") ) {

	}else {
		return responseWithJsonTo(res, root, "ERROR", "No method field or unkown method field value in json body!");
	}

	return;
}

