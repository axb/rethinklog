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
// File based logs storage
//    Filename is like "<log_dir>/<stripe_id>/<secs_from_epoch>_<num>.log"
//
//
// usage:
//    auto wr = storage.writer( "stripe_id", config );
//    wr( ..... ); // write data
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

   Stripe( std::string id_, Config& cfg_ );
public:
   typedef std::shared_ptr<Stripe> TPtr;
   typedef std::function< uint64_t( uint64_t, std::string, std::string, std::string ) > TWriter;

   static TPtr create( std::string id_, Config& cfg_ ) { return TPtr{ new Stripe( id_, cfg_ ) }; }

   //
   // usage:
   //    auto wr = storage.writer( "stripe_id", config );
   //    wr( ..... ); // write data
   //
   static TWriter writer( std::string id_, Config& cfg_ ) {
      auto st = create( id_, cfg_ );
      return [ st ] ( uint64_t offset, std::string key, std::string data, std::string localtime ) {
         return ( *st )( offset, key, data, localtime );
      };
   }

   // 
   // for primary nodes
   // for replicas offset = UINT64_MAX
   //
   uint64_t operator() ( uint64_t offset, std::string key, std::string data, std::string localtime );

   virtual ~Stripe();
};

///////////////////////////////////////////////////////////////////////////////////
//
// Disk based storage manager
//
///////////////////////////////////////////////////////////////////////////////////

class Storage
{
   boost::asio::io_service& _io;
   Config& _cfg;

   typedef std::map< std::string, Stripe::TPtr > TStripes;
   TStripes _stripes;
   Stripe::TPtr stripe( std::string stripeId );

public:
   Storage( boost::asio::io_service& io, Config& cfg );

   Config& config();

   //
   // right way to immediately log
   //
   Stripe::TWriter writer( std::string stripeId ) {
      auto st = stripe( stripeId );
      return [ st ] ( uint64_t offset, std::string key, std::string data, std::string localtime ) {
         return ( *st )( offset, key, data, localtime );
      };
   }

   //
   // right way to log and allow storage make SLA
   //
   void publish( std::string stripe, std::string key, std::string data, std::string localtime,
                 std::function< void( uint64_t offset ) > whendone_ );

   //
   // based on settings: archive and merge-by-key old logs
   //
   void housekeeping();

   // when finishing by ctrl-c
   void abort();
};

