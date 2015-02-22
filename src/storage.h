#pragma once

#include <string>
#include <functional>
#include <map>
#include <boost/asio.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bs = boost::iostreams;
namespace bpt = boost::property_tree;

///////////////////////////////////////////////////////////////////////////////////
//
// json config
//    
//     Cluster blueprint
//
//
//{
//   "data_centers" :
//   [
//      { "name" : "PDC"}
//   ],
//
//   "nodes" :
//   [
//      {
//         "name" : "node1",
//         "data_center" : "PDC",
//         "client_interface" : { "bind":"all", "port" : 8081 },
//         "cluster_interface" : { "bind":"all", "port" : 8082 },
//         "admin_interface" : { "bind":"all", "port" : 8083 }
//      }
//   ], // nodes
//
//         // Stripes & topics configuration
//   "topics":
//   [
//      {
//         "name":"test1",
//         "SLA" : "1-wait",
//         "housekeeping" : "key",
//         "stripes" :
//         [
//            {
//               "id" : "test1_115",
//               "primary" : "node1",
//               "replicas" : "node2, node3"
//            },
//            {
//               "id" : "test1_116",
//               "primary" : "node2",
//               "replicas" : "node2, node3"
//            }
//         ]
//      },
//      {
//         "name":"test2",
//         "SLA" : "no-wait",
//         "housekeeping" : "key",
//         "stripes" :
//         [
//            {
//               "id" : "test2_1",
//               "primary" : "node1",
//               "replicas" : "node3"
//            }
//         ]
//      }
//   ] // topics
//}
//
///////////////////////////////////////////////////////////////////////////////////

class Config : public bpt::ptree
{
   std::string _me;
public:
   Config(int argc, char *argv[]);
   virtual ~Config();

   // local node 
   std::string me();
   std::string dataDir() const;

   // my network services end-points
   uint16_t clientIntfPort() const;
   uint16_t clusterIntfPort() const;
   uint16_t adminIntfPort() const;

   // my master stripes
   struct StripeConfig {
      std::string 
         id, 
         primary, 
         replicas;
   };
   typedef std::list< StripeConfig > TStripeConfigs;
   TStripeConfigs masterStripes() const;

   // my replica stripes
   TStripeConfigs replicaStripes() const;

   // cluster interface by node name
   struct RemoteEndpoint {
      std::string host;
      uint16_t    port;
   };
   RemoteEndpoint remoteClusterIntf(std::string node);

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
   static const uint64_t FRAME_FILESIZE = 64 * 1024 * 1024; // 64 Mb
   bs::stream<bs::mapped_file_sink>  _frame;

   Stripe(std::string id_, Config& cfg_);
public:
   typedef std::shared_ptr<Stripe> pointer_type;
   static pointer_type create(std::string id_, Config& cfg_) { return pointer_type{ new Stripe(id_, cfg_) }; }

   // for primary nodes
   uint64_t append(std::string key, std::string data, std::string localtime);

   // for replicas
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

