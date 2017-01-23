#include <iostream>
#include <boost/program_options.hpp>
#include "client.hpp"
#include "server.hpp"
#include <boost/asio/io_service.hpp>

using namespace std;

namespace po = boost::program_options;

struct config_s {
	bool is_server = false;
	string serverurl = "http://127.0.0.1:7890/v1/";
	int port = 7890;
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
				"serverurl. \n"
				"For client in format of [http://]x.x.x.x:port/v1\n"
				"For server default is 0.0.0.0")
			("port,p", po::value<int>(&config.port),
				"port to listen on. Default 7890 for server, 7891 for client")
		;
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		if (vm.count("help")) {
			std::cout << desc << "\n";
			return false;
		}
		po::notify(vm);

		if ( type == "client" ) {
			config.is_server = 0;
		} else {
			config.is_server = 1;
		}

		if (!vm.count("serverurl")) {
			if ( !config.is_server ) {
				std::cout << desc << "\n";
				std::cout << "If klient to wtedy trzeba podac server url." << std::endl;
				return false;
			} else {
				config.serverurl = "0.0.0.0";
			}
		}

		if ( !vm.count("port")) {
			if ( config.is_server) {
				config.port = 7890;
			} else {
				config.port = 7891;
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

static boost::shared_ptr<boost::asio::io_service> io_service { boost::make_shared<boost::asio::io_service>() };

static void signalfunc(int signo) {
	std::cerr << "Signal " << signo <<" caught! shutting down!" << std::endl;
	io_service->stop();
}

int main(int argc, char *argv[])
{
	config_s config;

	if ( !process_command_line(argc, argv, config) ) {
		std::cout << "Error precessing command line arguments. " << std::endl;
		exit(1);
	}

	signal(SIGINT, signalfunc);

	try {
		if ( config.is_server ) {
			Server server;
			myhttpserver::options options(server);
			myhttpserver server_(
					options.address(config.serverurl).port(std::to_string(config.port)).io_service(io_service)
			);
			server_.run();
		} else {
			Client client(config.serverurl);
			client.run();
		}
	} catch(std::exception & e) {
		std::cerr << e.what() << std::endl;
		io_service->stop();
		return 1;
	}
	return 0;
}
