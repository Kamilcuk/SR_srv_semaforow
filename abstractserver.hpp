#ifndef ABSTRACTSERVER_HPP
#define ABSTRACTSERVER_HPP

#include <boost/network/protocol/http/server.hpp>
#include <json/json.h>
#include <vector>
#include "semaphore.hpp"
#include <mutex>
#include <map>

struct AbstractServer;
typedef boost::network::http::server<AbstractServer> myhttpserver;
//typedef std::shared_ptr<boost::network::http::async_connection<boost::network::http::tags::http_server, Server> > httpserverresponse;
//typedef myhttpserver::request httpserverrequest;
typedef boost::network::http::basic_request<boost::network::http::tags::http_server>  httpserverrequest;
typedef boost::network::http::basic_response<boost::network::http::tags::http_server> httpserverresponse;

struct AbstractServer
{
	void operator ()( httpserverrequest const &request,
					 httpserverresponse &response);
	void log(const myhttpserver::string_type &info);

	void operatorO_parseJson(httpserverresponse &res, Json::Value root);

	virtual void operatorO_parseJson_in(httpserverresponse &res, Json::Value root) = 0;

	std::map<std::string, std::string> headersToMap(auto headers);

public:
	void in_operator(const httpserverrequest &req, httpserverresponse &res);
};

#endif // ABSTRACTSERVER_HPP
