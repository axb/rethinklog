#include "config.h"

#include <boost/property_tree/json_parser.hpp>

Config::Config(int argc, char * argv[]) {
   _me = "localhost";
   if (argc > 1)
      _me = argv[1];

   std::string cfn = "rl.config";
   if (argc > 2)
      cfn = argv[2];

   bpt::read_json(cfn, (bpt::ptree&)*this);

   //
   // informational output
   //
   std::cout
      << "config summary ------------" << std::endl
      << "me : " << _me << std::endl
      << "data_centers : " << get_child("data_centers").size() << std::endl
      << "nodes : " << get_child("nodes").size() << std::endl
      << "topics : " << get_child("topics").size() << std::endl
      << "---------------------------" << std::endl << std::endl;
}

Config::~Config() {}

std::string Config::me() {
   return _me;
}

std::string Config::dataDir() const {
   return "./_logz"; /// <fixme>??
}

uint16_t Config::clientIntfPort() const {
   return uint16_t();
}

uint16_t Config::clusterIntfPort() const {
   return uint16_t();
}

uint16_t Config::adminIntfPort() const {
   return uint16_t();
}

Config::TStripeConfigs Config::masterStripes() const {
   return TStripeConfigs();
}

Config::TStripeConfigs Config::replicaStripes() const {
   return TStripeConfigs();
}

Config::RemoteEndpoint Config::remoteClusterIntf(std::string node) {
   return RemoteEndpoint();
}

