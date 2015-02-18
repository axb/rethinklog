#pragma once

#include <string>
#include <functional>
#include <map>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

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

class ReplicatedStorage
{
   boost::asio::io_service& _io;
   Config& _config;

   typedef std::pair< std::string, uint32_t > TStripeKey;
   typedef std::map< TStripeKey, Stripe* > TStripes;
   TStripes _stripes;
   Stripe* stripe(std::string topic_, uint32_t part_);

public:
   ReplicatedStorage(boost::asio::io_service& io, Config& cfg);

   Config& config();

   void publish(std::string topic, uint32_t partition, std::string key, std::string data, std::string localtime,
                std::function< void(uint64_t offset) > whendone_);

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
   uint64_t _lastOffset;

   boost::filesystem::path _file_name;
   boost::filesystem::ofstream _file;

public:
   Stripe(std::string topic_, uint32_t  part_, Config& cfg_);

   uint64_t append(std::string key, std::string data, std::string localtime);

   bool write(uint64_t offset, std::string key, std::string data, std::string localtime);
};