#pragma once

#include <boost/asio.hpp>
#include "wire.h"

/*
   Write protocol & pipeline
*/
class WriteSession :
   public std::enable_shared_from_this<WriteSession>
{
   boost::asio::ip::tcp::socket _soc;
   Msg _currentReq;
   void readNextMsg();
   ///////////////////////////////////////////////////////////////////////////////////
   //
   // save -> ack -> process local tasks 
   //             -> notify subscribers 
   //
   ///////////////////////////////////////////////////////////////////////////////////
public:
   WriteSession(boost::asio::ip::tcp::socket& s);
};


class Config;
class Storage;

/*
   Server
*/
class MasterWriteSvc
{
   boost::asio::io_service& _io;
   Config& _cfg;
   Storage& _stg;

   boost::asio::ip::tcp::acceptor _acceptor;
   boost::asio::ip::tcp::socket _cliSocket;

   void async_accept();

public:
   MasterWriteSvc(boost::asio::io_service& io, Config& cfg, Storage& stg_);
};