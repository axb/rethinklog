#include "client_api.h"
#include "storage.h"

ClientAPISvc::ClientAPISvc(boost::asio::io_service & io, Config & cfg, Storage & stg_)
   : _io(io), _cfg(cfg), _stg(stg_), _cliSocket(io),
   _acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), _cfg.clientIntfPort() )) { 
   waitNextClient();
}

void ClientAPISvc::waitNextClient() {
   _acceptor.async_accept(_cliSocket, 
                          [this](boost::system::error_code ec) {
      if (ec)
         return; // shit happened, full stop

      std::make_shared<ClientAPISession>(_cliSocket, _stg);
      waitNextClient();
   });
}

ClientAPISession::ClientAPISession(boost::asio::ip::tcp::socket& s, Storage& stg)
   : _soc(std::move(s)), _stg(stg) {
   waitNextMsg();
}

void ClientAPISession::waitNextMsg() {
   _currentReq.clear();
   auto me(shared_from_this()); // passing 'me' to lambda keeps from being destroyed ahead of time
   boost::asio::async_read(_soc, boost::asio::buffer(_currentReq.headerBuf(), Msg::header_len),
                           [this, me](boost::system::error_code ec, std::size_t /*length*/) {
      if (ec || !_currentReq.decodeHeader())
         return;

      // read body
      boost::asio::async_read(_soc, boost::asio::buffer(_currentReq.bodyBuf(), _currentReq.Header.body_len),
                              [this, me](boost::system::error_code ec, std::size_t /*length*/) {
         if (ec)
            return;

         /// make protocol
         {
//            wire::APIReq req;
//            if (!req.ParseFromString(_currentReq._wire_body))
//               return;

            //_stg.publish(cmd.stripe(), cmd.key(), cmd.data(), cmd.localtime(), [this, me, cmd](uint64_t offset) {
            //   wire::PublishRes resp; resp.set_clientseq(cmd.clientseq()); resp.set_offset(offset);
            //   if (!offset) resp.set_error("bad things happened");
            //   Msg wr; wr.pack(resp);
            //   boost::asio::async_write(_soc, boost::asio::buffer(wr._wire_send), [this](boost::system::error_code ec, std::size_t done) { 
            //      /// <todo> react if smth went wrong
            //   });
            //});
         }
         waitNextMsg(); // all good, continue waiting
      });
   });
}
