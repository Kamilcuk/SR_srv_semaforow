
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



Json::Value fillResult(Json::Value root) {
	return root;
}

void responseWithJsonTo(httpserverresponse &res, Json::Value realroot, std::string error)
{
	Json::Value jsonresp;
	jsonresp["method"] = realroot["method"];
	jsonresp["id"] = realroot["id"];
	if ( !error.empty() ) {
		jsonresp["error"] = error;
	}
	jsonresp["result"] = Json::Value();
	fillrespOK(res, to_string(jsonresp));
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

#include <string>
#include <sstream>
#include <vector>


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}
