#pragma once

#include <string>

class Broker;
class Stripe;

/*
   Disk related defines and operations
*/


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
   // Stripes configuration
   // [ topic { 
   //    name
   //    partitioning_algo : ? may be a part of client application logic ?
   //    SLA :
   //    housekeeping : 
   //    [ stripe {
   //       id : 
   //       primary : node 
   //       secondary : [ node ]
   //       }
   //    ]
   // } ]
   //


   // 
   void build(const std::string& src_);
   std::string serialize();

   // 
};

class Storage
{
   Config _config;
public:
   Storage(Broker& bk, int argc, char *argv[]);

   Config& config();

   //
   Stripe* stripe(std::string topic_, int part_);

   // 
   void housekeeping();

   // when finishing by ctrl-c
   void abort();
};

class Stripe
{
public:
   Stripe(std::string topic_, int part_, Config& cfg_);


   void append();


};