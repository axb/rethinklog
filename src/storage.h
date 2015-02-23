#pragma once

#include "config.h"
#include <string>
#include <functional>
#include <map>
#include <boost/asio.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

namespace bs = boost::iostreams;

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

