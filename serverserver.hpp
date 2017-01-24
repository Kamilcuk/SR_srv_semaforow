#ifndef SERVERSERVER_HPP
#define SERVERSERVER_HPP

#include <string>
#include "abstractserver.hpp"
#include "semaphore.h"

class Server;

class ServerServer : public AbstractServer
{
	Server *_server;

public:
	ServerServer(Server *s);
	void operatorO_parseJson_in(httpserverresponse &res, Json::Value root);
};

#endif // SERVERSERVER_HPP
