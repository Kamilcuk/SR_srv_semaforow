#include <vector>
#include <json/json.h>
#include <iostream>
#include <boost/network/protocol/http/client.hpp>
#include "client.hpp"

#define DBG_CONN std::cout
//#define DBG_CONN if (1) {} else std::cout

Client::Client(std::string serverurl) :
	_serverurl(serverurl),
	_owner("127.0.0.1:10000")
{
}

Json::Value Client::send(Json::Value msg)
{
	using namespace boost::network;
	using namespace boost::network::http;

	static uint id = 1;

	http::client client;

	msg["id"] = id++;
	Json::FastWriter writer;
	std::string bodymsg = writer.write( msg );

	client::request request(_serverurl);
	DBG_CONN << "Conn: " << _serverurl << std::endl;
	request << header("Content-Type", "application/json");
	request << header("Content-Length", std::to_string( bodymsg.length() ));
	request << header("Connection", "close");
	request << body(bodymsg);
	DBG_CONN << "Reqe: " << bodymsg;
	client::response response = client.post(request);


	Json::Value ret;
	Json::Reader reader;
	std::string bodyresp = body(response);
	bool parsingSuccessful = reader.parse( bodyresp.c_str(), ret );
	if ( !parsingSuccessful ) {
		DBG_CONN << "Resp: " << body(response);
		throw std::runtime_error("Failed to parse" + reader.getFormattedErrorMessages());
	}
	DBG_CONN << "JRes: " << writer.write( ret );

	return ret;
}

std::string Client::Send_MaintenanceHello(std::string name)
{
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Maintenance.Hello";
	root["params"][0]["name"] = name;
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
	Json::FastWriter writer;
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Locks.DeleteSemaphore";
	root["params"][0]["UUID"] = uuid;
	Json::Value ret = send(root);
	return ret["result"]["UUID"].asString();
}

std::string Client::Send_LocksDecrementSemaphore(std::string uuid)
{
	Json::FastWriter writer;
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Locks.DecrementSemaphore";
	root["params"][0]["UUID"] = uuid;
	root["params"][0]["Owner"] = _owner;
	Json::Value ret = send(root);
	return ret["result"]["UUID"].asString();
}

std::string Client::Send_LocksIncrementSemaphore(std::string uuid)
{
	Json::FastWriter writer;
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Locks.IncrementSemaphore";
	root["params"][0]["UUID"] = uuid;
	root["params"][0]["Owner"] = _owner;
	Json::Value ret = send(root);
	return ret["result"]["UUID"].asString();
}

std::string Client::Send_LocksDump()
{
	Json::FastWriter writer;
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Locks.Dump";
	Json::Value ret = send(root);
	return writer.write( ret["result"] );
}

void Client::run()
{
	while(1) {

	}
	std::cout << Send_MaintenanceHello("world") << std::endl;
	std::string uuid = Send_LocksCreateSemaphore(1,0,2);
	std::cout << uuid << std::endl;
	std::cout << Send_MaintenanceHello("world") << std::endl;
	std::cout << Send_LocksIncrementSemaphore(uuid);
	std::cout << Send_LocksIncrementSemaphore(uuid);
	std::cout << Send_LocksIncrementSemaphore(uuid);
	std::cout << Send_LocksIncrementSemaphore(uuid);
	std::cout << Send_LocksDeleteSemaphore(uuid) << std::endl;
}
