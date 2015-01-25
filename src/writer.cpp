#include "writer.h"

WriteSvc::WriteSvc(boost::asio::io_service & io, Config & cfg, Storage & stg_)
   : _io(io), _cfg(cfg), _stg(stg_) {}

void WriteSvc::buildPipeline() {}
