#include "storage.h"
#include "log_entry.h"

#include <chrono>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
namespace bpt = boost::property_tree;

struct Config::TData : public bpt::ptree{};

Config::Config(int argc, char * argv[]) : _data( new TData() ) {
   _me = "localhost";
   if (argc > 1) 
      _me = argv[1];

   std::string cfn = "rl.config";
   if (argc > 2)
      cfn = argv[2];
   
   bpt::read_json( cfn, (bpt::ptree&)*_data);

   //
   // informational output
   //
   std::cout 
      << "config summary ------------" << std::endl
      << "me : " << _me << std::endl
      << "data_centers : " << _data->get_child("data_centers").size() << std::endl
      << "nodes : " << _data->get_child("nodes").size() << std::endl
      << "topics : " << _data->get_child("topics").size() << std::endl
      << "---------------------------" << std::endl << std::endl;
}

Config::~Config() {
   delete _data;
}

std::string Config::me() {
   return std::string();
}

std::string Config::dataDir() const {
   return "./_logz"; /// <fixme>
}

int Config::nthreads() const {
   return 0;
}

void Config::build(const std::string & src_) {}

std::string Config::serialize() {
   return std::string();
}

ReplicatedStorage::ReplicatedStorage(boost::asio::io_service & io, Config & cfg)
   : _cfg(cfg), _io(io) {
   /// <todo> init stripes & SLAs

}

Config & ReplicatedStorage::config() {
   return _cfg;
}

void ReplicatedStorage::publish(std::string stripe_, std::string key, std::string data, std::string localtime,
                                std::function< void(uint64_t offset) > whendone_) {
   uint64_t offset = 0;
   auto st = stripe(stripe_);
   if (!st) {
      offset = 0;
   } else {
      offset = st->append(key, data, localtime);

      /// <todo> replication SLA

   }
   whendone_(offset);
}

Stripe::pointer_type ReplicatedStorage::stripe(std::string stripe) {
   auto it = _stripes.find(stripe);
   if (it != _stripes.end())
      return it->second;
   /// <todo> check config for auto creation
   auto res = Stripe::create(stripe, _cfg);
   _stripes.insert({ stripe, res });
   return res;
}

void ReplicatedStorage::housekeeping() {}

void ReplicatedStorage::abort() {
   // close all open files
}


void Stripe::buildFrame() {
   //
   // build filename
   //
   std::stringstream dir;
   dir << _cfg.dataDir() << "/" << _id << "/";
   fs::create_directories(dir.str());

   std::stringstream fn;
   int ix = 0;
   std::time_t tm = std::time(0);
   do {
      fn.clear();
      fn << dir.str() << tm << "_" << ix << ".log";
      ++ix;
   } while (fs::exists(fn.str()));

   //
   // open mapped file
   //
   if (_frame.is_open())
      _frame.close();

   bs::mapped_file_params args;
   args.path = fn.str();
   args.new_file_size = FRAME_FILESIZE;
   _frame.open(bs::mapped_file_sink(args));
}

Stripe::Stripe(std::string id_, Config & cfg_) : _cfg(cfg_), _id(id_), _lastOffset(1) {
   /// <todo> read last offset from storage
}

uint64_t Stripe::append(std::string key, std::string data, std::string localtime) {
   ++_lastOffset;
   if (write(_lastOffset, key, data, localtime))
      return _lastOffset;
   else
      return 0; // error
}

bool Stripe::write(uint64_t offset, std::string key, std::string data, std::string localtime) {
   LogEntry le{ offset, key, data, localtime };

   if (!_frame.is_open() || (size_t)_frame.tellp() + le.size() >= FRAME_FILESIZE)
      buildFrame();

   _frame << le;

   return true;
}

Stripe::~Stripe() {
   if (_frame.is_open()) {
      std::flush(_frame);
      _frame.close();
   }
}
