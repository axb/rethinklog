#pragma once

#include <boost/asio.hpp>
#include "wire.h"


class Config;
class ReplicatedStorage;


/*
   Write protocol & pipeline
*/
class ProducerSession :
   public std::enable_shared_from_this<ProducerSession>
{
   boost::asio::ip::tcp::socket _soc;
   ReplicatedStorage& _stg;

   Msg _currentReq;
   void waitNextMsg();
   ///////////////////////////////////////////////////////////////////////////////////
   //
   // save -> ack -> process local tasks 
   //             -> notify subscribers 
   //
   ///////////////////////////////////////////////////////////////////////////////////
public:
   ProducerSession(boost::asio::ip::tcp::socket& s, ReplicatedStorage& stg);
};

/*
   Servers
*/
class MasterWriteSvc
{
   boost::asio::io_service& _io;
   Config& _cfg;
   ReplicatedStorage& _stg;

   boost::asio::ip::tcp::acceptor _acceptor;
   boost::asio::ip::tcp::socket _cliSocket;

   void waitNextClient();

public:
   MasterWriteSvc(boost::asio::io_service& io, Config& cfg, ReplicatedStorage& stg_);
};