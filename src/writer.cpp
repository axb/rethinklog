#include "writer.h"
#include "storage.h"

MasterWriteSvc::MasterWriteSvc(boost::asio::io_service & io, Config & cfg, ReplicatedStorage & stg_)
   : _io(io), _cfg(cfg), _stg(stg_), _cliSocket(io),
   _acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8081)) { //< TODO read port (all endpoint) from config      
   waitNextClient();
}

void MasterWriteSvc::waitNextClient() {
   _acceptor.async_accept(_cliSocket, [this](boost::system::error_code ec) {
      if (ec) 
         return;

      std::make_shared<ProducerSession>(_cliSocket, _stg);
      waitNextClient();
   });
}

ProducerSession::ProducerSession(boost::asio::ip::tcp::socket& s, ReplicatedStorage& stg)
   : _soc(std::move(s)), _stg( stg ) {
   waitNextMsg();
}

void ProducerSession::waitNextMsg() {
   _currentReq.clear();
   auto me(shared_from_this()); // passing 'me' to lambda keeps from being destructed
   boost::asio::async_read(_soc, boost::asio::buffer(_currentReq.headerBuf(), Msg::header_len),
                           [this, me](boost::system::error_code ec, std::size_t /*length*/) {
      if ( ec || !_currentReq.decodeHeader()) 
         return;

      // read body
      boost::asio::async_read(_soc, boost::asio::buffer(_currentReq.bodyBuf(), _currentReq.Header.body_len),
                              [this, me](boost::system::error_code ec, std::size_t /*length*/) {
         if (ec) 
            return;

         // make protocol
         {
            wire::PublishCmd cmd;
            if (!cmd.ParseFromString(_currentReq._wire_body))
               return;

            _stg.publish(cmd.topic(), cmd.partition(), cmd.key(), cmd.data(), cmd.localtime(), [this, me, cmd](uint64_t offset) {
               wire::PublishResp resp; resp.set_clientseq(cmd.clientseq()); resp.set_offset(offset);
               if (!offset) resp.set_error("bad things happened");

               Msg wr; wr.pack(resp);
               boost::asio::async_write(_soc, boost::asio::buffer(wr._wire_send),
                                        [this](boost::system::error_code ec, std::size_t done) {
                  /// <todo> react if smth went wrong
               });
            });
         }
         waitNextMsg(); // all good, continue waiting
      });
   });
}
