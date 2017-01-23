#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cstdio>
#include "server.hpp"
#include <boost/uuid/uuid.hpp>

boost::uuids::uuid getUuid();
boost::uuids::uuid getUuid(std::string u);
std::string getUuidStr();
std::string getUuidStr(boost::uuids::uuid u);

#define DEBUGMSG(fmt, ...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
	__FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define _unused_ __attribute__((unused))

void fillrespERR(httpserverresponse &res, std::string msg,
			  httpserverresponse::status_type type = httpserverresponse::forbidden);
void fillrespOK(httpserverresponse &res, std::string msg,
				httpserverresponse::status_type type = httpserverresponse::ok);

#endif // HELPERS_HPP
