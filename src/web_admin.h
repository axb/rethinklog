#pragma once

/*
   simple web-server for console

      gives statis js-based SPA
      serves config change requests
      notifies of status
*/

#include "via/http_server.hpp"
#include "via/comms/tcp_adaptor.hpp"

typedef via::http_server<via::comms::tcp_adaptor, std::string> http_server_type;
typedef http_server_type::http_connection_type http_connection;
typedef http_server_type::chunk_type http_chunk_type;

class Config;

class WebSvc : public http_server_type
{
   Config& _cfg;

   void respond_to_request(http_connection::weak_pointer weak_ptr);
public:
   WebSvc(boost::asio::io_service& io, Config& cfg);
};