
#include "server.hpp"
#include "helpers.hpp"

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>


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
