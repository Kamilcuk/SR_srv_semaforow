#include <stddef.h>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <json/json.h>
#include <boost/network/protocol/http/client.hpp>
#include "client.hpp"

//#define DBG_CONN std::cout
#define DBG_CONN if (1) {} else std::cout

Client::Client(std::string serverurl) :
	_serverurl(serverurl)
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
	DBG_CONN << "Conn: " << _serverurl;
	request << header("Content-Type", "application/json");
	request << header("Content-Length", std::to_string( bodymsg.length() ));
	request << header("Connection", "close");
	request << body(bodymsg);
	DBG_CONN << "Req:  " << bodymsg;
	client::response response = client.post(request);
	DBG_CONN << "Resp: " << body(response);


	Json::Value ret;
	Json::Reader reader;
	std::string bodyresp = body(response);
	bool parsingSuccessful = reader.parse( bodyresp.c_str(), ret );
	if ( !parsingSuccessful ) {
		throw std::runtime_error("Failed to parse" + reader.getFormattedErrorMessages());
	}
	DBG_CONN << "JRes: " << writer.write( ret );

	return ret;
}

void Client::run()
{
	Json::Value root;   // will contains the root value after parsing.
	root["method"] = "Maintenance.Hello";
	root["params"][0]["name"] = "world";
	send(root);
}
