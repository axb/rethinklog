#include "storage.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

struct Config::TData : public boost::property_tree::ptree{

};

Config::Config(int argc, char * argv[]) {
   // 

}

std::string Config::me() {
   return std::string();
}

std::string Config::dataDir() const {
   return std::string();
}

int Config::nthreads() const {
   return 0;
}

void Config::build(const std::string & src_) {}

std::string Config::serialize() {
   return std::string();
}

ReplicatedStorage::ReplicatedStorage(boost::asio::io_service & io, Config & cfg)
   : _config(cfg), _io(io) {
   /// <todo> init stripes & SLAs

}

Config & ReplicatedStorage::config() {
   return _config;
}

void ReplicatedStorage::publish(std::string topic, uint32_t partition, std::string key, std::string data, std::string localtime,
                                std::function< void(uint64_t offset) > whendone_) {
   uint64_t offset = 0;
   Stripe* st = stripe(topic, partition);
   if (!st) {
      offset = 0;
   } else {
      offset = st->append(key, data, localtime);

      /// <todo> replication SLA

   }
   whendone_(offset);
}

Stripe * ReplicatedStorage::stripe(std::string topic_, uint32_t part_) {
   auto it = _stripes.find({ topic_, part_ });
   if (it != _stripes.end())
      return it->second;
   /// <todo> check config for auto creation
   Stripe* res = new Stripe(topic_, part_, _config);
   _stripes.insert({ { topic_, part_ }, res });
   return res;
}

void ReplicatedStorage::housekeeping() {}

void ReplicatedStorage::abort() {
   // close all open files
}


Stripe::Stripe(std::string topic_, uint32_t  part_, Config & cfg_) {
   _file_name = cfg_.dataDir() + "/" + topic_; /// <todo> +itoa(part_);
   _file.open(_file_name, std::ios_base::out | std::ios_base::app);
}

uint64_t Stripe::append(std::string key, std::string data, std::string localtime) {
   uint64_t offset = _file.tellp();   /// <todo> filesize
   if (write(offset, key, data, localtime)) // write to file
      return offset;
   else
      return 0; // error
}

bool Stripe::write(uint64_t offset, std::string key, std::string data, std::string localtime) {

   // if frame can handle this -> write to frame
   // else buildFrame
   // buildFrame -> checks if new file shall be created
   // opens new if required, does mmap

   // write data
   if (!_file.is_open()) 
      _file.open(_file_name, std::ios_base::out | std::ios_base::app);
   if (_file.is_open()) {
      _file.write(data.data(), data.size());
      _file.close();
   } else {
      return false;
   }
   /// <todo> write index

   return false;
}
