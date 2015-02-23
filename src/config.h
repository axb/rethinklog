#pragma once

#include <list>
#include <boost/property_tree/ptree.hpp>
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
   struct StripeConfig
   {
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
   struct RemoteEndpoint
   {
      std::string host;
      uint16_t    port;
   };
   RemoteEndpoint remoteClusterIntf(std::string node);

};

