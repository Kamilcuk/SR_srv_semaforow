#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/network/protocol/http/server.hpp>
#include <json/json.h>

struct Server;
typedef boost::network::http::server<Server> myhttpserver;
// typedef std::shared_ptr<boost::network::http::async_connection<boost::network::http::tags::http_server, Server> > httpserverresponse;
//typedef myhttpserver::request httpserverrequest;
typedef boost::network::http::basic_request<boost::network::http::tags::http_server>  httpserverrequest;
typedef boost::network::http::basic_response<boost::network::http::tags::http_server> httpserverresponse;

struct Server
{
	Server();

	void operator ()( httpserverrequest const &request,
					 httpserverresponse &response);
	void log(const myhttpserver::string_type &info);

	void operatorO_parseJson(httpserverresponse &res, Json::Value root);
};

#endif // SERVER_HPP
