#pragma once

//#include <cstdint>
#include <istream>
#include <ostream>

///////////////////////////////////////////////////////////////////////////////////
//
// Stream based operations on log messages
//
///////////////////////////////////////////////////////////////////////////////////

class LogEntry
{
   mutable std::string _json;
public:
   uint64_t _offset;
   std::string _key, _data, _localtime;

   LogEntry() {}

   LogEntry(uint64_t offset, std::string key, std::string data, std::string localtime );
   ~LogEntry() {}

   size_t size() const;

   friend std::ostream& operator << (std::ostream& os, const LogEntry& le);
};

