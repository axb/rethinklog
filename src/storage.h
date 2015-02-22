#pragma once

#include <string>
#include <functional>
#include <map>
#include <boost/asio.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
namespace bs = boost::iostreams;

///////////////////////////////////////////////////////////////////////////////////
//
// json config
//
///////////////////////////////////////////////////////////////////////////////////
/// Cluster blueprint
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
/// Stripes & topics configuration
// [ topic { 
//    name
//    SLA : 
//    housekeeping : 
//    [ stripe {
//       id :   string, unique across all topics
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

   /// local node 
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
// File based storage
//    Filename is like "<log_dir>/<stripe_id>/<secs_from_epoch>_<num>.log"
//
///////////////////////////////////////////////////////////////////////////////////
class Stripe : public std::enable_shared_from_this< Stripe >
{
   std::string _id;
   Config& _cfg;

   uint64_t _lastOffset;

   void buildFrame();
   const uint64_t FRAME_FILESIZE = 64 * 1024 * 1024; 
   bs::stream<bs::mapped_file_sink>  _frame;

   Stripe(std::string id_, Config& cfg_);
public:
   typedef std::shared_ptr<Stripe> pointer_type;
   static pointer_type create(std::string id_, Config& cfg_) { return pointer_type{ new Stripe(id_, cfg_) }; }

   uint64_t append(std::string key, std::string data, std::string localtime);

   bool write(uint64_t offset, std::string key, std::string data, std::string localtime);

   virtual ~Stripe();
};

///////////////////////////////////////////////////////////////////////////////////
//
// Disk based storage manager
//
///////////////////////////////////////////////////////////////////////////////////

class ReplicatedStorage
{
   boost::asio::io_service& _io;
   Config& _cfg;

   typedef std::map< std::string, Stripe::pointer_type > TStripes;
   TStripes _stripes;
   Stripe::pointer_type stripe(std::string stripeId);

public:
   ReplicatedStorage(boost::asio::io_service& io, Config& cfg);

   Config& config();

   void publish(std::string stripe, std::string key, std::string data, std::string localtime,
                std::function< void(uint64_t offset) > whendone_);

   void housekeeping();

   // when finishing by ctrl-c
   void abort();
};

