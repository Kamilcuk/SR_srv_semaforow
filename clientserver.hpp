#ifndef CLIENTSERVER_HPP
#define CLIENTSERVER_HPP

#include "abstractserver.hpp"

class Client;

class ClientServer : public AbstractServer
{
	Client *_client;

public:
	ClientServer(Client *client);

	// AbstractServer interface
public:
	void operatorO_parseJson_in(httpserverresponse &res, Json::Value root);
};

#endif // CLIENTSERVER_HPP
