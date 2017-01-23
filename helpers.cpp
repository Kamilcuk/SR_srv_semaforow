
#include "server.hpp"
#include "helpers.hpp"

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
