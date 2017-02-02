#include "abstractserver.hpp"
#include <boost/network/protocol/http/server.hpp>
#include <string>
#include <iostream>
#include "helpers.hpp"
#include <map>
#include <string>

std::map<std::string,std::string> AbstractServer::headersToMap(auto headers) {
	std::map<std::string,std::string> ret;
	for(unsigned int i=0;i<headers.size();i++) {
		std::string first = headers[i].name;
		std::string second = headers[i].value;
		ret.insert(std::pair<std::string, std::string>(first, second) );
		//std::cerr << "ret.insert( " << first << ", " << second <<")\n";
	}
	return ret;
}

void AbstractServer::operator() (httpserverrequest const &req, httpserverresponse &res) {
	DEBUGMSG("--> new conn %s from %s with body %s", req.method.c_str(), req.source.c_str(), req.body.c_str());
	in_operator(req, res);
	DEBUGMSG("--> answering %s with body %s", req.method.c_str(), res.content.c_str());
}

void AbstractServer::in_operator (httpserverrequest const &req, httpserverresponse &res) {
	//std::cout << request.headers[0].string();
	//DEBUGMSG("new conn %s from %s with body %s", req.method.c_str(), req.source.c_str(), req.body.c_str());
	/*std::cout << request.source << "\n";
	for(int i=0;i<request.headers.size();i++) {
		std::cout << request.headers[i].name <<"="<<request.headers[i].value<< "\n";
	}
	std::cout << "query = " << request.method << "\n";
	std::cout << request.body << "\n";
	response = httpserverresponse::stock_reply(
		httpserverresponse::ok, "Hello, world!");*/
	if ( req.method == "POST" ) {
		/* check post */
		auto headers = headersToMap(req.headers);
		auto conetenttype = headers.find("Content-Type");
		if ( conetenttype == headers.end() ) {
			return fillrespERR(res, "ERR: header \"Content-Type\" not found\n");
		} else if ( conetenttype->second.compare("application/json") ) {
			return fillrespERR(res, "ERR: header \"Content-Type\" = "+conetenttype->second+" != \"application/json\" \n");
		}

		if ( req.destination.compare("/v1") ) {
			return fillrespERR(res, "ERR: currently only verion 1 api is supported. That means that url path should be exactly '/v1'");
		}

		/* get json body */
		std::string body = req.body;
		Json::Value root;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse( body.c_str(), root );     //parse process
		if ( !parsingSuccessful ) {
			return fillrespERR(res, "ERR: failed to parse body string into json: \""+body);
		}

		this->operatorO_parseJson(res, root);

	} else {
		return fillrespERR(res, "ERR: currently supporting only POST http requests");
	}
}

void AbstractServer::log(myhttpserver::string_type const &info) {
	std::cerr << "ERROR: " << info << '\n';
}


void AbstractServer::operatorO_parseJson(httpserverresponse &res, Json::Value root)
{
	try {
		return operatorO_parseJson_in(res, root);
	} catch(std::exception & e) {
		std::cerr << e.what() << std::endl;
		fillrespERR(res, "Interal error - poinformuj o tym Kamila Cukrowskiego");
	}
}
