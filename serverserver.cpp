#include "server.hpp"
#include "serverserver.hpp"

ServerServer::ServerServer(Server *s) :
	_server(s)
{

}

void ServerServer::operatorO_parseJson_in(httpserverresponse &res, Json::Value root)
{
	return _server->operatorO_parseJson_in(res, root);
}
