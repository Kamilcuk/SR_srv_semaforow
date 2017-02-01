#include <vector>
#include <json/json.h>
#include <iostream>
#include <boost/network/protocol/http/client.hpp>
#include "client.hpp"
#include "abstractserver.hpp"
#include <cstdio>
#include "unistd.h"
#include <boost/bind.hpp>
#include "helpers.hpp"

#define DBG_CONN std::cout
//#define DBG_CONN if (1) {} else std::cout

std::map<std::string, Client::SemaphoreInfo> Client::getSems() const
{
	return sems;
}

std::string Client::getOwner() const
{
	return _owner;
}

bool Client::getDeadlock() const
{
	return deadlock;
}

void Client::setDeadlock(bool value)
{
	deadlock = value;
}

Client::Client(std::string serverurl, std::string clientip, unsigned int port, unsigned int threads) :
	_serverurl(serverurl),
	_owner(clientip+":"+std::to_string(port)),
	_port(port),
	_clientconsole(this, serverurl),
	_clientserver(this),
	_clientserverhttpserver(
		myhttpserver::options(_clientserver).
			address(clientip).port(std::to_string(_port))
	)
{
	for(unsigned int i=0;i<threads;++i) {
		_clientserverthreads.push_back( boost::thread(
				boost::bind(&decltype(_clientserverhttpserver)::run,
							&_clientserverhttpserver)
		));
	}
}

std::string Client::sendStrToServerStr(std::string serverurl, std::string bodymsg, unsigned int Timeout)
{
	using namespace boost::network;
	using namespace boost::network::http;
	http::client::options options;
	if ( Timeout != 0 ) {
		options.timeout(Timeout);
	}
	http::client client(options);
	if ( serverurl.substr(0,7).compare("http://") ) {
		serverurl="http://"+serverurl+"/v1";
	}
	DBG_CONN << "sendStrToServerStr: " << serverurl << " " << bodymsg;
	client::request request(serverurl);
	request << header("Content-Type", "application/json");
	request << header("Content-Length", std::to_string( bodymsg.length() ));
	request << header("Connection", "close");
	request << body(bodymsg);
	//DBG_CONN << "Reqe: " << bodymsg;
	client::response response = client.post(request);
	std::string respmsg = body(response);
	DBG_CONN << "Respmsg: " << respmsg;
	return respmsg;
}

Json::Value Client::sendStrToServer(std::string serverurl, std::string bodymsg, unsigned int Timeout)
{
	return to_json( Client::sendStrToServerStr(serverurl, bodymsg, Timeout) );
}

Json::Value Client::sendToServer(std::string serverurl, Json::Value msg, unsigned int Timeout)
{
	static uint id = 1;
	msg["id"] = id++;
	std::string bodymsg;
	try {
		bodymsg = to_string( msg );
	} catch(std::exception &e) {
		std::cerr << __PRETTY_FUNCTION__ << e.what() << std::endl;
		return Json::Value();
	}
	return Client::sendStrToServer(serverurl, bodymsg, Timeout);
}


Json::Value Client::send(Json::Value msg, unsigned int Timeout) {
	return sendToServer(_serverurl, msg, Timeout);
}

std::string Client::Send_MaintenanceHello(std::string name)
{
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Maintenance.Hello";
	root["params"][0]["Name"] = name;
	Json::Value ret = send(root);
	return ret["result"]["Message"].asString();
}

std::string Client::Send_LocksCreateSemaphore(int CurrentVal, int MinVal, int MaxVal)
{
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Locks.CreateSemaphore";
	root["params"][0]["CurrentVal"] = CurrentVal;
	root["params"][0]["MinVal"] = MinVal;
	root["params"][0]["MaxVal"] = MaxVal;
	Json::Value ret = send(root);
	return ret["result"]["UUID"].asString();
}

std::string Client::Send_LocksDeleteSemaphore(std::string uuid)
{
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Locks.DeleteSemaphore";
	root["params"][0]["UUID"] = uuid;
	Json::Value ret = send(root);
	return ret["result"]["UUID"].asString();
}

std::string Client::Send_LocksDecrementSemaphore(std::string uuid)
{
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Locks.DecrementSemaphore";
	root["params"][0]["UUID"] = uuid;
	root["params"][0]["Owner"] = _owner;
	{
		std::lock_guard<std::mutex> guard(semsmutex);
		std::cerr << "Polling decremented semaphore" << std::endl;
		if ( sems.find(uuid) == sems.end() ) {
			sems.insert(std::make_pair(uuid, SemaphoreInfo(uuid)));
		}
		sems.find(uuid)->second.status = "Polling Decrement";
	}
	Json::Value ret = send(root, 0); // Timeout = 0 -> log polling
	std::string restr;
	try {
		restr = ret["result"]["UUID"].asString();
	} catch (std::exception &e) {
		restr = "";
	}
	{
		std::lock_guard<std::mutex> guard(semsmutex);
		if ( sems.find(uuid) == sems.end() ) {
			sems.insert(std::make_pair(uuid, SemaphoreInfo(uuid)));
		}
		if ( !restr.compare(uuid) ) {
			sems.find(uuid)->second.status = "Decrementing Succeded";
			sems.find(uuid)->second.decrement();
			std::cerr << "Decremented semaphore" << std::endl;
		} else {
			sems.find(uuid)->second.status = "Decrementing Failed";
			std::cerr << "Decremented semaphore" << std::endl;
		}
	}
	return restr;
}

std::string Client::Send_LocksIncrementSemaphore(std::string uuid)
{
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Locks.IncrementSemaphore";
	root["params"][0]["UUID"] = uuid;
	root["params"][0]["Owner"] = _owner;
	{
		std::lock_guard<std::mutex> guard(semsmutex);
		if ( sems.find(uuid) == sems.end() ) {
			sems.insert(std::make_pair(uuid, SemaphoreInfo(uuid)));
		}
		sems.find(uuid)->second.status = "Incrementing";
	}
	Json::Value ret = send(root);
	std::string restr;
	try {
		restr = ret["result"]["UUID"].asString();
	} catch (std::exception &e) {
		restr = "";
	}
	{
		std::lock_guard<std::mutex> guard(semsmutex);
		if ( sems.find(uuid) == sems.end() ) {
			sems.insert(std::make_pair(uuid, SemaphoreInfo(uuid)));
		}
		if ( !restr.compare(uuid) ) {
			sems.find(uuid)->second.status = "Incrementing Succeded";
			sems.find(uuid)->second.increment();
		} else {
			sems.find(uuid)->second.status = "Incrementing Failed";
		}
	}
	return restr;
}

std::string Client::Send_LocksDump()
{
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Locks.Dump";
	root["params"][0] = Json::Value();
	Json::Value ret = send(root);
	return to_string( ret["result"] );
}

std::string Client::Send_ClientProbe_Static(std::string clienturl, std::string InitiatorAddr)
{
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Client.Probe";
	root["params"][0]["InitiatorAddr"] = InitiatorAddr;
	Json::Value ret = sendToServer(clienturl, root);
	return to_string( ret["result"] );
}

std::string Client::Send_LocksProbe(std::string InitiatorAddr, std::string uuid)
{
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Locks.Probe";
	root["params"][0]["InitiatorAddr"] = InitiatorAddr;
	root["params"][0]["ReturnAddr"] = _owner;
	root["params"][0]["UUID"] = uuid;
	Json::Value ret = send(root);
	return to_string( ret["result"] );
}

void Client::run()
{
	while(!_stop) {
		_clientconsole.run();
	}
	_clientserverhttpserver.stop();
	for(auto &t : _clientserverthreads)
		t.join();
}

void Client::stop()
{
	DEBUGMSG(" ");
	Servicable::stop();
	_clientconsole.stop();
}

void Client::SendLocksToAllServersWhenClientProbe(std::string InitiatorAddr)
{
	std::lock_guard<std::mutex> guard(semsmutex);
	for(auto &s : sems) {
		 if ( !s.second.status.compare("Polling Decrement") ) {
			 (void)Send_LocksProbe(InitiatorAddr, s.first);
		}
	}
}
