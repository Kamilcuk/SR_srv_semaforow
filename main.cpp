#include <iostream>
#include <json/json.h>
#include <boost/program_options.hpp>

using namespace std;

namespace po = boost::program_options;

bool process_command_line(int argc, char** argv,
						  bool &is_server,
						  std::string &serverurl)
{
	std::string type;

	try
	{
		po::options_description desc("Program Usage", 1024, 512);
		desc.add_options()
			("help",     "produce help message")
			("type,t",   po::value<std::string>(&type)->required(),      "type [client|server] mandatory")
			("server,s", po::value<std::string>(&serverurl)->required(), "serverurl in format x.x.x.x:port")
		;
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		if (vm.count("help")) {
			std::cout << desc << "\n";
			return false;
		}
		po::notify(vm);
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

	if ( type == "client" ) {
		is_server = 0;
	} else {
		is_server = 1;
	}

	return true;
}

int main(int argc, char *argv[])
{
	bool is_server;
	std::string serverurl;
	if ( !process_command_line(argc, argv, is_server, serverurl) ) {
		std::cout << "Error precessing command line arguments. " << std::endl;
		exit(1);
	}
	if ( is_server ) {
		run_server(serverurl);
	} else {
		run_client(serverurl);
	}
	cout << "Hello World!" << endl;
	return 0;
}
