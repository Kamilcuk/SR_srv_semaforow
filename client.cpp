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

Client::Client(std::string serverurl, std::string clientip, unsigned int port) :
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
	_clientserverthread = boost::thread(
				boost::bind(&decltype(_clientserverhttpserver)::run,
							&_clientserverhttpserver)
	);
}

std::string Client::sendStrToServerStr(std::string serverurl, std::string bodymsg)
{
	using namespace boost::network;
	using namespace boost::network::http;
	http::client client;
	DBG_CONN << "sendStrToServerStr: " << serverurl << " " << bodymsg << std::endl;
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

Json::Value Client::sendStrToServer(std::string serverurl, std::string bodymsg)
{
	return to_json( Client::sendStrToServerStr(serverurl, bodymsg) );
}

Json::Value Client::sendToServer(std::string serverurl, Json::Value msg)
{
	static uint id = 1;
	msg["id"] = id++;
	Json::FastWriter writer;
	std::string bodymsg;
	try {
		bodymsg = writer.write( msg );
	} catch(std::exception &e) {
		std::cerr << __PRETTY_FUNCTION__ << e.what() << std::endl;
		return Json::Value();
	}
	return Client::sendStrToServer(serverurl, bodymsg);
}


Json::Value Client::send(Json::Value msg) {
	return sendToServer(_serverurl, msg);
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
	while(!_stop) {
		_clientconsole.run();
	}
	_clientserverhttpserver.stop();
	_clientserverthread.join();
}

void Client::stop()
{
	DEBUGMSG(" ");
	Servicable::stop();
	_clientconsole.stop();
}
