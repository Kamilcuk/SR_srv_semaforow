#include "clientconsole.hpp"
#include <iostream>
#include <regex>
#include <unistd.h>
#include "client.hpp"

ClientConsole::ClientConsole(std::string serverurl) :
	_serverurl(serverurl)
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

void ClientConsole::run()
{
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
	} else {
		std::cout << "UNKNOWN LINE - omiting..." << std::endl;
		std::cout << "Supported commands:" << std::endl;
		std::cout << "{\"<some string> - send this string to server" << std::endl;
		std::cout << "send <some string>" << std::endl;
		std::cout << "sendraw <some data> " << std::endl;
		std::cout << "quit" << std::endl;
		return;
	}
}
