#include "clientconsole.hpp"
#include <iostream>
#include <regex>
#include <unistd.h>
#include "client.hpp"
#include "helpers.hpp"
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf


ClientConsole::ClientConsole(Client *client, std::string serverurl) :
	_serverurl(serverurl),
	_client(client)
{

}

void ClientConsole::sendline(std::string msg) {
	std::cout << "Sending:   " << msg << std::endl;
	try {
		std::cout << "Received:  " << Client::sendStrToServerStr(_serverurl, msg) << std::endl;
	} catch(std::exception &e) {
		std::cout << "ERROR sendign str to serveR: " << e.what() << std::endl;
	}
}

Json::Value ClientConsole::sendlineJsonJson(Json::Value msg)
{
	Json::Value ret;
	std::cout << "Sending:   " << to_string(msg) << std::endl;
	try {
		ret = Client::sendToServer(_serverurl, msg);
		std::cout << "Received:  " << to_string(ret) << std::endl;
	} catch(std::exception &e) {
		std::cout << "ERROR sendign str to serveR: " << e.what() << std::endl;
	}
	return ret;
}

void ClientConsole::usage()
{
	std::cout << "UNKNOWN LINE - omiting..." << std::endl
	 << "Supported commands:" << std::endl
	 << "{\"<some string> - send this string to server" << std::endl
	 << "send <some string>" << std::endl
	 << "sendraw <some data> " << std::endl
	 << "printinfo" << std::endl
	 << "Maintenance.Hello <name>" << std::endl
	 << "Locks.CreateSemaphore <CurrentVal> <MinVal> <MaxVal>" << std::endl
	 << "Locks.DeleteSemaphore <uuid>" << std::endl
	 << "Locks.DecrementSemaphore <uuid>" << std::endl
	 << "Locks.IncrementSemaphore <uuid>" << std::endl
	 << "Locks.Dump" << std::endl
	 << "Locks.Probe <InitiatorAddr> <uuid>" << std::endl
	 << "Client.Probe <clienturl> <InitiatorAddr>" << std::endl
	 << "log" << std::endl
	 << "quit" << std::endl;
}

void ClientConsole::one_run()
{
	std::string method;
	std::string result;

	if ( _client->getDeadlock() ) {
		std::cout << "DEADLOCK DETECTED! \n" << std::endl;
	}

	std::cout << "> ";
	std::string line;
	std::cin.clear();
	std::getline(std::cin, line);
	if ( std::regex_match(line, std::regex("send .*")) ) {
		sendline( line.substr(5) );
	} else
	if ( std::regex_match(line, std::regex("sendraw .*")) ) {
		sendline( line.substr(8) );
	} else
	if ( std::regex_match(line, std::regex("\\{\".*")) ) {
		sendline( line );
	} else
	if ( std::regex_match(line, std::regex("quit")) ) {
		this->stop();
		raise(SIGINT); // yay!
#define CHECK(x) ({ method = (x); std::regex_match(line, std::regex(method+".*")); })
	} else
	if ( CHECK("printinfo") || CHECK("log") ) {
		std::cout << "Printing information about owned sems \n";
		std::cout << "Initial sem state was 0 \n";
		 for(auto &s : _client->getSems() ) {
			 std::cout
					<< " s.f.uuid: " << s.first
					<< " s.s.uuid: " << s.second.uuid
					<< " s.value:  " << s.second.value
					<< " s.status: " << s.second.status
					<< std::endl;
		 }
	} else
	if ( CHECK("Locks.CreateSemaphore") ) {
		std::istringstream iss(line); iss >> method;
		int cur, min, max; iss >> cur; iss >> min; iss >> max;
		result = _client->Send_LocksCreateSemaphore(cur, min, max);
		std::cout << "Method: " << method << std::endl;
		std::cout << "Recv:   " << result << std::endl;
	} else
	if ( CHECK("Maintenance.Hello") ) {
		std::istringstream iss(line); iss >> method;
		std::string name; iss >> name;
		result = _client->Send_MaintenanceHello(name);
		std::cout << "Method: " << method << std::endl;
		std::cout << "Recv:   " << result << std::endl;
	} else
	if ( CHECK("Locks.DeleteSemaphore") ) {
		std::istringstream iss(line); iss >> method;
		std::string uuid; iss >> uuid;
		result = _client->Send_LocksDeleteSemaphore(uuid);
		std::cout << "Method: " << method << std::endl;
		std::cout << "Recv:   " << result << std::endl;
	} else
	if ( CHECK("Locks.DecrementSemaphore") ) {
		std::istringstream iss(line); iss >> method;
		std::string uuid; iss >> uuid;
		result = _client->Send_LocksDecrementSemaphore(uuid);
		std::cout << "Method: " << method << std::endl;
		std::cout << "Recv:   " << result << std::endl;

	} else
	if ( CHECK("Locks.IncrementSemaphore") ) {
		std::istringstream iss(line); iss >> method;
		std::string uuid; iss >> uuid;
		result = _client->Send_LocksIncrementSemaphore(uuid);
		std::cout << "Method: " << method << std::endl;
		std::cout << "Recv:   " << result << std::endl;
	} else
	if ( CHECK("Locks.Dump") ) {
		std::istringstream iss(line); iss >> method;
		result = _client->Send_LocksDump();
		std::cout << "Method: " << method << std::endl;
		std::cout << "Recv:   " << result << std::endl;
	} else
	if ( CHECK("Locks.Probe") ) {
		std::istringstream iss(line); iss >> method;
		std::string InitiatorAddr, UUID; iss >> InitiatorAddr; iss >> UUID;
		result = _client->Send_LocksProbe(InitiatorAddr, UUID);
		std::cout << "Method: " << method << std::endl;
		std::cout << "Recv:   " << result << std::endl;
	} else
	if ( CHECK("Client.Probe") ) {
		std::istringstream iss(line); iss >> method;
		std::string clienturl; iss >> clienturl;
		std::string InitiatorAddr; iss >> InitiatorAddr;
		result = _client->Send_ClientProbe_Static(clienturl, InitiatorAddr);
		std::cout << "Method: " << method << std::endl;
		std::cout << "Recv:   " << result << std::endl;
	} else {
		/*std:: string Send_LocksCreateSemaphore(int CurrentVal, int MinVal, int MaxVal);
		std::string Send_MaintenanceHello(std::string name);
		std::string Send_LocksDeleteSemaphore(std::string uuid);
		std::string Send_LocksDecrementSemaphore(std::string uuid);
		std::string Send_LocksIncrementSemaphore(std::string uuid);
		std::string Send_LocksDump();*/
		usage();
		return;
	}
}

void ClientConsole::run() {
	while(!_stop) {
		try	{
			this->one_run();
		} catch (std::exception &e) {
			std::cerr << "std:: excetion + " << e.what() << std::endl;
		}
	}
}
