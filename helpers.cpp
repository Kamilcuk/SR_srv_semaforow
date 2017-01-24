
#include "abstractserver.hpp"
#include "helpers.hpp"

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <json/json.h>

std::string to_string(Json::Value root) {
	Json::FastWriter writer;
	return writer.write( root );
}

void fillrespOKJson(httpserverresponse &res, Json::Value root)
{
	fillrespOK(res, to_string(root));
}

void fillrespERR( httpserverresponse &res, std::string msg,
			  httpserverresponse::status_type type)
{
	std::string __msg = (msg);
	DEBUGMSG("Answering client with \"%s\"\n", __msg.c_str());
	res = httpserverresponse::stock_reply(type, __msg);
}

void fillrespOK(httpserverresponse &res, std::string msg,
				httpserverresponse::status_type type)
{
	fillrespERR(res, msg, type);
}

std::string getUuidStr(boost::uuids::uuid u) {
	return boost::lexical_cast<std::string>( u );
}

std::__cxx11::string getUuidStr()
{
	return getUuidStr( getUuid() );
}
boost::uuids::uuid getUuid()
{
	return boost::uuids::random_generator()();
}

boost::uuids::uuid getUuid(std::__cxx11::string u)
{
	return boost::uuids::string_generator()(u);
}



Json::Value fillParams(Json::Value root) {
	return root;
}

void responseWithJson(httpserverresponse &res, std::__cxx11::string method) {
	Json::Value root;
	root["method"] = method;
	fillrespOK(res, to_string(root));
}

void responseWithJsonTo(httpserverresponse &res, Json::Value realroot)
{
	responseWithJson(res, realroot["method"].asString());
}

Json::Value to_json(std::__cxx11::string str)
{
	Json::Value ret;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( str, ret );
	if ( !parsingSuccessful ) {
		throw std::runtime_error("to_json error - failed to parse" + reader.getFormattedErrorMessages());
	}
	return ret;
}
