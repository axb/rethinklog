#pragma once

#include <boost/asio.hpp>
#include "wire.h"


class Config;
class Storage;


/*
   Client protocol & pipeline
*/
class ClientAPISession : public std::enable_shared_from_this<ClientAPISession>
{
   boost::asio::ip::tcp::socket _soc;
   Storage& _stg;

   Msg _currentReq;
   void waitNextMsg();
   ///////////////////////////////////////////////////////////////////////////////////
   //
   // save -> ack -> process local tasks 
   //             -> notify subscribers 
   //
   ///////////////////////////////////////////////////////////////////////////////////
public:
   ClientAPISession(boost::asio::ip::tcp::socket& s, Storage& stg);
};

/*
   Servers
*/
class ClientAPISvc
{
   boost::asio::io_service& _io;
   Config& _cfg;
   Storage& _stg;

   boost::asio::ip::tcp::acceptor _acceptor;
   boost::asio::ip::tcp::socket _cliSocket;

   void waitNextClient();

public:
   ClientAPISvc(boost::asio::io_service& io, Config& cfg, Storage& stg_);
};