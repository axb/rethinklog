#pragma once

#include <string>
#include "wire.pb.h"

struct Msg
{
   // parsed format
   struct
   {
      unsigned int body_len;
   } Header;
   enum {header_len = 15 };

   // wire format
   std::string _wire_hdr;
   std::string _wire_body;

   // accessors
   char* bodyBuf() { return const_cast<char*>(_wire_body.data()); }
   char* headerBuf() { return const_cast<char*>(_wire_hdr.data() ); }

   // parse header string
   bool decodeHeader() {
      Header.body_len = atoi(_wire_hdr.c_str());
      _wire_body.reserve(Header.body_len + 1);
      return true;
   }

   void clear() {
      _wire_hdr.clear();
      _wire_hdr.reserve(header_len + 1);
      _wire_body.clear();
   }
   // format header string
   // pack();
};
