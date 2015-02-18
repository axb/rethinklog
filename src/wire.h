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
   std::string _wire_send;

   // accessors
   char* bodyBuf() { return const_cast<char*>(_wire_body.data()); }
   char* headerBuf() { return const_cast<char*>(_wire_hdr.data() ); }
   char* sendBuf() { return const_cast<char*>(_wire_send.data()); }

   // parse header string
   bool decodeHeader() {
      Header.body_len = atoi(_wire_hdr.c_str()); // TODO use protobuf encoding
      _wire_body.reserve(Header.body_len);
      return true;
   }

   void clear() {
      _wire_hdr.clear();
      _wire_hdr.reserve(header_len);
      _wire_body.clear();
   }
   
   // format for sending
   void pack(const google::protobuf::Message& m) {
      _wire_body = m.SerializeAsString();

      int len = _wire_body.size();
      char buf[header_len + 1];
      buf[header_len] = 0;
      sprintf(buf, "%0*d", header_len, len);
      _wire_hdr.assign(buf, header_len);

      _wire_send.reserve(header_len + len);
      _wire_send = _wire_hdr + _wire_body;
   }
};
