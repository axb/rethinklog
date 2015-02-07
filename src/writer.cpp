#include "writer.h"

MasterWriteSvc::MasterWriteSvc(boost::asio::io_service & io, Config & cfg, Storage & stg_)
   : _io(io), _cfg(cfg), _stg(stg_), _cliSocket(io),
   _acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8081)) { //< TODO read port (and all endpoint) from config      
   async_accept();
}

void MasterWriteSvc::async_accept() {
   _acceptor.async_accept(_cliSocket, [this](boost::system::error_code ec) {
      if (ec) {
         // kill me
         return;
      }
      std::make_shared<WriteSession>(_cliSocket);
      async_accept();
   });
}

WriteSession::WriteSession(boost::asio::ip::tcp::socket& s)
   : _soc(std::move(s)) {
   readNextMsg();
}

void WriteSession::readNextMsg() {
   auto me(shared_from_this());
   _currentReq.clear();
   boost::asio::async_read(_soc, boost::asio::buffer(_currentReq.headerBuf(), Msg::header_len),
                           [this, me](boost::system::error_code ec, std::size_t /*length*/) {
      if (!_currentReq.decodeHeader()) {
         // kill me
         return;
      }

      // read body
      boost::asio::async_read(_soc, boost::asio::buffer(_currentReq.bodyBuf(), _currentReq.Header.body_len),
                              [this, me](boost::system::error_code ec, std::size_t /*length*/) {
         if (ec) {
            // kill me
            return;
         }

         // make protocol
         {

         }
         readNextMsg();
      });
   });
}
