#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cstdio>
#include "abstractserver.hpp"
#include <boost/uuid/uuid.hpp>
#include <json/json.h>


boost::uuids::uuid getUuid();
boost::uuids::uuid getUuid(std::string u);
std::string getUuidStr();
std::string getUuidStr(boost::uuids::uuid u);

#define DEBUGMSG(fmt, ...) fprintf(stderr, "DBG: %s:%d:%s(): " fmt, \
	__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define _unused_ __attribute__((unused))

void fillrespERR(httpserverresponse &res, std::string msg,
			  httpserverresponse::status_type type = httpserverresponse::forbidden);
void fillrespOK(httpserverresponse &res, std::string msg,
				httpserverresponse::status_type type = httpserverresponse::ok);

void fillrespOKJson(httpserverresponse &res, Json::Value root);

std::string to_string(Json::Value root);
Json::Value to_json(std::string str);


/** too much haskell ... */
Json::Value fillParams(Json::Value root);
template<class Param, class Value, class...Rest>
Json::Value fillParams(Json::Value root, Param& param, Value& value, const Rest&... rest) {
	root[param]=value;
	return fillParams(root, rest...);
}

/** still too much haskell */
void responseWithJson(httpserverresponse &res, std::string method);
template<class...Rest>
void responseWithJson(httpserverresponse &res, std::string method, const Rest&... rest)
{
	Json::Value root;
	root["method"] = method;
	root["params"][0] = fillParams(Json::Value(), rest...);
	fillrespOK(res, to_string(root));
}

void responseWithJsonTo(httpserverresponse &res, Json::Value realroot);
template<class...Rest>
void responseWithJsonTo(httpserverresponse &res, Json::Value realroot, const Rest&... rest)
{
	responseWithJson(res, realroot["method"].asString(), rest...);
}

#endif // HELPERS_HPP
