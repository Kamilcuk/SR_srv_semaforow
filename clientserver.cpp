#include "clientserver.hpp"
#include "helpers.hpp"
#include "client.hpp"

ClientServer::ClientServer(Client *client) :
	_client(client)
{

}

void ClientServer::operatorO_parseJson_in(httpserverresponse &res, Json::Value root)
{
	// main switch, yay
	if ( !root["method"].compare("Maintenance.Hello") ) {
		return responseWithJsonTo(res, root, std::string(),
						 "Message", "Hello, "+root["params"][0]["Name"].asString());
	} else
	if ( !root["method"].compare("Maintenance.Heartbeat") ) {
		return responseWithJsonTo(res, root);
	}else
	if ( !root["method"].compare("Client.Probe") ) {
		return responseWithJsonTo(res, root, "Not yet implemented!");
	}else {
		return responseWithJsonTo(res, root, "No method field or unkown method field value in json body!");
	}

	return;
}
