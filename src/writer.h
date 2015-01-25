#pragma once

#include <boost/asio.hpp>

/*
   Write related protocol & pipeline
*/

class Config;
class Storage;

class WriteSvc
{
   boost::asio::io_service& _io;
   Config& _cfg;
   Storage& _stg;
public:
   WriteSvc(boost::asio::io_service& io, Config& cfg, Storage& stg_);

   ///////////////////////////////////////////////////////////////////////////////////
   //
   // save -> ack -> process local tasks 
   //             -> notify subscribers 
   //
   ///////////////////////////////////////////////////////////////////////////////////
   void buildPipeline();

};