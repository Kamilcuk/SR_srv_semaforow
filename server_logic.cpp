#include <json/json.h>
#include <boost/network/protocol/http/response.hpp>
#include "server.hpp"
#include <map>
#include <string>
#include <iostream>
#include "helpers.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

std::map<std::string,Semaphore> Server::sems = std::map<std::string,Semaphore>();

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

/** still too much haskell */
void responseWithJson(httpserverresponse &res, std::string method) {
	Json::Value root;
	root["method"] = method;
	fillrespOK(res, printJson(root));
}
template<class...Rest>
void responseWithJson(httpserverresponse &res, std::string method, const Rest&... rest)
{
	Json::Value root;
	root["method"] = method;
	root["params"][0] = fillParams(Json::Value(), rest...);
	fillrespOK(res, printJson(root));
}
void responseWithJsonTo(httpserverresponse &res, Json::Value realroot)
{
	responseWithJson(res, realroot["method"].asString());
}
template<class...Rest>
void responseWithJsonTo(httpserverresponse &res, Json::Value realroot, const Rest&... rest)
{
	responseWithJson(res, realroot["method"].asString(), rest...);
}

void Server::operatorO_parseJson_in(httpserverresponse &res, Json::Value root)
{
	std::cerr << printJson(root);

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
		sems.insert(std::make_pair(u, Semaphore(CurrentVal, MinVal, MaxVal)));
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
		auto isem = sems.find(u);
		if ( isem == sems.end() ) {
			return responseWithJsonTo(res, root, "ERROR", "no such semaphores with guven uuid - "+u);
		}
		bool val = isem->second.decrement(o);
		if ( val == false ) {
			return responseWithJsonTo(res, root, "ERROR", "could not decrement semaphore");
		}
		return responseWithJsonTo(res, root, "UUID", u);
	}else
	if ( !root["method"].compare("Locks.IncrementSemaphore") ) {
		std::string u = root["params"][0]["UUID"].asString();
		std::string o = root["params"][0]["Owner"].asString();
		auto isem = sems.find(u);
		if ( isem == sems.end() ) {
			return responseWithJsonTo(res, root, "ERROR", "no such semaphores with guven uuid - "+u);
		}
		bool val = isem->second.increment(o);
		if ( val == false ) {
			return responseWithJsonTo(res, root, "ERROR", "could not decrement semaphore");
		}
		return responseWithJsonTo(res, root, "UUID", u);
	}else
	if ( !root["method"].compare("Locks.Dump") ) {
		Json::Value params;
		unsigned int i=0;
		for(auto const& isems : sems) {
			params[i]["UUID"] = isems.first;
			Semaphore s = isems.second;
			params[i]["owners_size"] = s.owners.size();
			for(unsigned int j=0;i<s.owners.size();++j) {
				params[i]["owners"][j] = s.owners[j];
			}
			params[i]["CurrentVal"] = s.cur;
			params[i]["MaxVal"] = s.max;
			params[i]["MinVal"] = s.min;
			i++;
		}
		Json::Value root;
		root["method"] = "Locks.Dump";
		root["params"][0] = params;
		return fillrespOKJson(res, root);
	}else
	if ( !root["method"].compare("Locks.Probe") ) {

	}else {
		return fillrespERR(res, "No method field or unkown method field value in json body!");
	}

	return;
}


void Server::operatorO_parseJson(httpserverresponse &res, Json::Value root)
{
	try {
		return operatorO_parseJson_in(res, root);
	} catch(std::exception & e) {
		std::cerr << e.what() << std::endl;
		fillrespERR(res, "Interal error - poinformuj o tym Kamila Cukrowskiego");
	}
}
