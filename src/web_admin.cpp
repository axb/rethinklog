#include "web_admin.h"
#include <fstream>
#include <streambuf>
#include <boost/algorithm/string.hpp>

WebSvc::WebSvc(boost::asio::io_service & io, Config & cfg)
   : _cfg(cfg), http_server_type(io) {
   //
   // setup the server - connect the handlers
   //
   request_received_event([this](http_connection::weak_pointer weak_ptr,
      via::http::rx_request const& request,
      std::string const& body) {
      std::cout << "Rx request: " << request.to_string();
      std::cout << "Rx headers: " << request.headers().to_string();
      std::cout << "Rx body: " << body << std::endl;

      if (!request.is_chunked())
         respond_to_request(weak_ptr);
   });

   chunk_received_event([this](http_connection::weak_pointer weak_ptr,
      http_chunk_type const& chunk,
      std::string const& data) {
      std::cout << "Rx chunk: " << chunk.to_string() << "\n";
      std::cout << "Chunk data: " << data << std::endl;

      // Only send a response to the last chunk.
      if (chunk.is_last()) {
         std::cout << "Last chunk, extension: " << chunk.extension() << "\n";
         std::cout << "trailers: " << chunk.trailers().to_string() << std::endl;
         respond_to_request(weak_ptr);
      }
   });

   request_expect_continue_event([this](http_connection::weak_pointer weak_ptr,
      via::http::rx_request const& request,
      std::string const& /* body */) {
      static const size_t MAX_LENGTH(1048576);

      std::cout << "expect_continue_handler\n";
      std::cout << "rx request: " << request.to_string();
      std::cout << "rx headers: " << request.headers().to_string() << std::endl;

      // Reject the message if it's too big, otherwise continue
      via::http::tx_response response((request.content_length() > MAX_LENGTH) ?
                                      via::http::response_status::code::REQUEST_ENTITY_TOO_LARGE :
                                      via::http::response_status::code::CONTINUE);
      weak_ptr.lock()->send(response);
   });

   socket_disconnected_event([this](http_connection::weak_pointer /* weak_ptr */) {
      std::cout << "socket_disconnected_handler" << std::endl;
   });

   //
   // start accepting http connections on the given port
   // FIXME: get port from config
   //
   boost::system::error_code error(accept_connections(8080));
   if (error) {
      std::cerr << "Error: " << error.message() << std::endl;
   } else {
      std::cout << "web admin - accepting at port 8080" << std::endl;
   }
}

void WebSvc::respond_to_request(http_connection::weak_pointer weak_ptr) {
   /// A string to send in default responses.
   static const std::string response_body(std::string("<html>\r\n")
                                          + std::string("<head><title>Accepted</title></head>\r\n")
                                          + std::string("<body><h1>200 - Accepted</h1></body>\r\n")
                                          + std::string("</html>\r\n"));

   http_connection::shared_pointer connection(weak_ptr.lock());
   if (connection) {
      // Get the last request sent on this connection.
      via::http::rx_request const& request(connection->request());

      // The default response is 404 Not Found
      via::http::tx_response response(via::http::response_status::code::NOT_FOUND);
      response.add_server_header();
      response.add_date_header();

      //
      // dispatch request by Uri
      //
      if ((request.method() != "GET")) {
         response.set_status(via::http::response_status::code::METHOD_NOT_ALLOWED);
         response.add_header(via::http::header_field::id::ALLOW, "GET");
         connection->send(response);
      } else {
         response.set_status(via::http::response_status::code::OK);

         std::vector<std::string> path;
         std::string uri = request.uri();
         boost::trim_if(uri, boost::is_any_of("/"));
         if (uri.size()) boost::split(path, uri, boost::is_any_of("/"), boost::token_compress_on);

         // serve SPA files
         if (uri == ""
             || uri == "js/bootstrap.min.js"
             || uri == "css/bootstrap.min.css"
             || uri == "css/styles.css") {

            if (uri == "") uri = "index.html";

            std::string content_type("text/html");
            if (uri.rfind(".css") != std::string::npos) content_type = "text/css";
            if (uri.rfind(".js") != std::string::npos)  content_type = "text/javascript";
            response.add_header(via::http::header_field::id::CONTENT_TYPE, content_type); // !!!! цуко важно !!!

            // read the file from app dir, 'admin' subdir
            std::ifstream t("./admin/" + uri);
            std::stringstream buffer;
            buffer << t.rdbuf();
            connection->send(response, buffer.str());
         } else
            // serve API
         {
            connection->send(response, response_body);
         }
      }
   } else
      std::cerr << "Failed to lock http_connection::weak_pointer" << std::endl;
}

