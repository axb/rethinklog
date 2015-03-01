#include "log_entry.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace bpt = boost::property_tree;

LogEntry::LogEntry(uint64_t offset, std::string key, std::string data, std::string localtime) 
   : _offset(offset), _key(key), _data(data), _localtime (localtime){
}

size_t LogEntry::size() const {
   if (!_json.size()) {
      bpt::ptree pt;
      pt.put("offset", _offset);
      pt.put("key", _key);
      pt.put("localtime", _localtime);
      pt.put("data", _data);
      std::ostringstream buf;
      write_json(buf, pt, false);
      _json = buf.str();
   }
   return _json.size();
}

std::ostream& operator << (std::ostream& os, const LogEntry& le) {
   le.size(); /// < force building json
   os << le._json;
   return os;
}
