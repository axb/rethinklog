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

Storage::Storage(boost::asio::io_service & io, Config & cfg)
   : _config(cfg), _io(io) {}

Config & Storage::config() {
   return _config;
}

std::function<void()> Storage::writeTask() {
   return std::function<void()>();
}

Stripe * Storage::stripe(std::string topic_, int part_) {
   return NULL;
}

void Storage::housekeeping() {}

void Storage::abort() {
   // close all open files
}


Stripe::Stripe(std::string topic_, int part_, Config & cfg_) {}

void Stripe::append() {}
