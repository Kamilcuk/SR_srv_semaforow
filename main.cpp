#include <iostream>
#include <boost/program_options.hpp>
#include "client.hpp"
#include "server.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <unistd.h>

using namespace std;

namespace po = boost::program_options;

struct config_s {
	bool is_server = false;
	string serverurl = "http://127.0.0.1:7890/v1/";
	int listenport = 7890;
	std::string listenip = "127.0.0.1";
};

bool process_command_line(int argc, char** argv,struct config_s &config)
{
	std::string type;

	try
	{
		po::options_description desc("Program Usage", 1024, 512);
		desc.add_options()
			("help,h",     "produce help message")
			("type,t",   po::value<std::string>(&type)->required(),
				"type [client|server] mandatory")
			("serverurl,s", po::value<std::string>(&config.serverurl),
				"serverurl to connect to when beeing a client.\n"
				"For client in format of x.x.x.x:port\n"
				"For server is ignored\n")
			("listenport,p", po::value<int>(&config.listenport),
				"port to listen on. Default 7890 for server, 7891 for client")
			("listenip,l", po::value<std::string>(&config.listenip),
				"ip address to listen on\n"
				"defaults to 127.0.0.1 for clients, and 0.0.0.0 for server.\n")
		;
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		if (vm.count("help")) {
			std::cout << desc << "\n";
			return false;
		}
		po::notify(vm);

		std::cout << "Type = \"" << type << "\""<< std::endl;

		if ( !type.compare("client") ) {
			config.is_server = false;
		} else if ( !type.compare("server") ) {
			config.is_server = true;
		} else {
			std::cerr << "WRONG type argument must be client or server";
			return false;
		}

		if ( !config.is_server ) {
			if (!vm.count("serverurl")) {
				std::cout << desc << "\n";
				std::cout << "If klient to wtedy trzeba podac server url." << std::endl;
				return false;
			} else {
				config.serverurl = "http://"+config.serverurl+"/v1";
			}
			if ( !vm.count("listenport")) {
				config.listenport = 7891;
			}
			if ( !vm.count("listenip") ) {
				config.listenip = "127.0.0.1";
			}
		} else {
			if ( !vm.count("listenport")) {
				config.listenport = 7890;
			}
			if ( !vm.count("listenip") ) {
				config.listenip = "0.0.0.0";
			}
		}



	}
	catch(std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return false;
	}
	catch(...)
	{
		std::cerr << "Unknown error!" << "\n";
		return false;
	}
	return true;
}

Servicable *service = nullptr;
std::thread quittingthread;

void waitSecThenExit(unsigned int sec) {
	sleep(sec);
	std::cerr << "SOMETHING INCREDIBLY WRONG. Forcing quit";
	exit(1);
}

void signalfunc(int signum) {
	std::cerr << "Rreceived signal: " << signum;
	std::cerr << " starting quitting thread ";
	quittingthread = std::thread(waitSecThenExit, 5);
	if ( service  ) {
		std::cerr << " stopping services "<<std::endl;
		try {
			service->stop();
		} catch(std::exception & e) {
			std::cerr << "ERROR in signalfunc(): " << e.what() << std::endl;
		}
	} else {
		std::cerr << std::endl;
	}
}

int main(int argc, char *argv[])
{
	config_s config;

	if ( !process_command_line(argc, argv, config) ) {
		std::cout << "Error precessing command line arguments. " << std::endl;
		exit(1);
	}
	std::cout << "Params: " << std::string(config.is_server?"server":"client")
			<<" listenip: "<< config.listenip
		   <<" listenport: "<<config.listenport
		  <<" serverurl: "<<config.serverurl<<" \n";

	signal(SIGINT, signalfunc);
	try {
		if ( config.is_server ) {
			service = new Server(config.listenip, config.listenport);
		} else {
			service = new Client(config.serverurl, config.listenip, config.listenport);
		}
		std::cout << "Started up, ready to serve!\n";
		service->run();
		delete service;
	} catch(std::exception & e) {
		std::cerr << "ERROR in main(): " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
