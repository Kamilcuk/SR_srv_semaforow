#include "clientserver.hpp"
#include "helpers.hpp"
#include "client.hpp"
#include <regex>

ClientServer::ClientServer(Client *client) :
	_client(client)
{

}

bool ClientServer::compareAddresses(std::string InitiatorAddr, std::string myaddr, std::string &errorcode)
{
	std::vector<std::string> words;
	words = split(InitiatorAddr, ':');
	if ( words.size() != 2 ) {
		errorcode="InitiatorAddr didn't get through split on ':' ";
	}
	std::string ip = words[0];
	int port;
	try {
		port = std::stoi(words[1]);
	} catch(std::exception &e) {
		errorcode="port = std::stoi(words[1]); + "+std::string(e.what());
		return false;
	}

	words = split(myaddr, ':');
	if ( words.size() != 2 ) {
		errorcode="InitiatorAddr didn't get through split on ':' ";
	}
	std::string ownerip = words[0];
	int ownerport;
	try {
		ownerport = std::stoi(words[1]);
	} catch(std::exception &e) {
		errorcode="owner = std::stoi(words[1]); + "+std::string(e.what());
		return false;
	}

	if ( ownerip.compare("0.0.0.0") || ip.compare("0.0.0.0") || !ownerip.compare(ip) ) {
		errorcode=" !ownerip.compare(ip) ";
		return false;
	}

	if ( port != ownerport ) {
		errorcode=" port != ownerport ";
		return false;
	}
	return true;
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
		std::string InitiatorAddr = root["params"][0]["InitiatorAddr"].asString();

		//std::regex reg("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}:[0-9]+");
		//if ( std::regex_match(InitiatorAddr, reg) ) {
//			return responseWithJsonTo(res, root, "InitiatorAddr didn't get through regex ");
//		}

		std::string errorcode;
		if ( !compareAddresses(InitiatorAddr, _client->getOwner(), errorcode) ) {
			// Otherwise, if client is not currently waiting for any resources, client MUST do nothing.
			// Otherwise, if client IS currently waiting for any resources, client MUST send a Locks.
			_client->SendLocksToAllServersWhenClientProbe(InitiatorAddr);
			return responseWithJsonTo(res, root, errorcode);
		}
		// If they are equal, client MUST print a message about deadlock detection.
		std::cerr << "DEADLOCK DETECTED!" << std::endl;
		_client->setDeadlock(true);
		return responseWithJsonTo(res, root, "DEADLOCK");
	}else {
		return responseWithJsonTo(res, root, "No method field or unkown method field value in json body!");
	}

	return;
}
