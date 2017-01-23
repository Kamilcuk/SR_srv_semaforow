#include <json/json.h>
#include <boost/network/protocol/http/response.hpp>
#include "server.hpp"
#include <map>
#include <string>
#include <iostream>
#include "helpers.hpp"

static std::string printJson(Json::Value root) {
	Json::FastWriter writer;
	return writer.write( root );
}

void fillrespOKJson(httpserverresponse &res, Json::Value root)
{
	fillrespOK(res, printJson(root));
}

/** too much haskell ... */
Json::Value fillParams(Json::Value root) {
	return root;
}
template<class Param, class Value, class...Rest>
Json::Value fillParams(Json::Value root, Param& param, Value& value, const Rest&... rest) {
	root[param]=value;
	return fillParams(root, rest...);
}

template<class...Rest>
void responseJsonWith(httpserverresponse &res, std::string method, const Rest&... rest)
{
	Json::Value root;
	root["method"] = method;
	root["param"][0] = fillParams(Json::Value(), rest...);
	fillrespOK(res, printJson(root));
}

void Server::operatorO_parseJson(httpserverresponse &res, Json::Value root)
{
	std::cerr << printJson(root);

	// main switch, yay
	if ( !root["method"].compare("Maintenance.Hello") ) {
		return responseJsonWith(res, "Maintenance.Hello",
						 "Message", "Hello, "+root["params"][0]["Name"].asString());
	} else
	if ( !root["method"].compare("Maintenance.Heartbeat") ) {

	} else
	if ( !root["method"].compare("Locks.CreateSemaphore") ) {

	} else
	if ( !root["method"].compare("Locks.DeleteSemaphore") ) {

	} else
	if ( !root["method"].compare("Locks.CreateSemaphore") ) {

	} else
	if ( !root["method"].compare("Locks.DeleteSemaphore") ) {

	} else
	if ( !root["method"].compare("Locks.DecrementSemaphore") ) {

	}else
	if ( !root["method"].compare("Locks.IncrementSemaphore") ) {

	}else
	if ( !root["method"].compare("Locks.Dump") ) {

	}else
	if ( !root["method"].compare("Locks.Probe") ) {

	}else {
		return fillrespERR(res, "No method field or unkown method field value in json body!");
	}
	return;
}
