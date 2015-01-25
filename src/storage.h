#pragma once

#include <string>
#include <functional>
#include <boost/asio.hpp>

///////////////////////////////////////////////////////////////////////////////////
//
// json config
//
///////////////////////////////////////////////////////////////////////////////////
// Cluster blueprint
// [ data_center {
//       name : 
// } ]
//
// [ node {
//       name
//       data_center
//       client interface : { host, port}
//       cluster interface: { host, port }
//       admin interface: { host, port } 
// } ]
//
//
///////////////////////////////////////////////////////////////////////////////////
// Stripes & topics configuration
// [ topic { 
//    name
//    partitioning_algo : ? may be a part of client application logic ?
//    SLA :
//    housekeeping : 
//    [ stripe {
//       id : 
//       primary : node 
//       secondary : [ node, ]
//       }
//    ]
// } ]
//
///////////////////////////////////////////////////////////////////////////////////

class Config
{
   struct TData;
   TData* _data;
public:
   Config(int argc, char *argv[]);

   // local node 
   std::string me();
   std::string dataDir() const;
   int nthreads() const;

   // 
   void build(const std::string& src_);
   std::string serialize();

   // 
};

///////////////////////////////////////////////////////////////////////////////////
//
// Disk based storage manager
//
///////////////////////////////////////////////////////////////////////////////////
class Stripe;

class Storage
{
   boost::asio::io_service& _io;
   Config& _config;

   //
   Stripe* stripe(std::string topic_, int part_);

public:
   Storage( boost::asio::io_service& io, Config& cfg );

   Config& config();

   //
   // 'write task' builder
   //
   std::function<void ()> writeTask();

   void housekeeping();

   // when finishing by ctrl-c
   void abort();
};

///////////////////////////////////////////////////////////////////////////////////
//
// File based storage
//
///////////////////////////////////////////////////////////////////////////////////
class Stripe
{

public:
   Stripe(std::string topic_, int part_, Config& cfg_);


   void append();
};