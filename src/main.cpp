////////////////////////////////////////////////////
//
// parameters:
//    config 
//    data directory
//
//    functionality:
//       web ui
//       writer
//       reader
//       subscribe 
//       bridge
//
////////////////////////////////////////////////////

#include "storage.h"
#include "writer.h"
#include "web_admin.h"
#include <boost/thread.hpp>

int main(int argc, char *argv[]) {

   // TODO: setup logging

   Config cfg(argc, argv);
   {
      //
      // task queue
      //
      boost::asio::io_service io;

      // Ctrl^C and kill
      boost::asio::signal_set sigs(io, SIGINT, SIGTERM);
      sigs.async_wait([&](const boost::system::error_code& error, int signal_number) {
         if (!error) {
            std::cout << "Ctrl^C invoked." << std::endl << "Shutting down the system." << std::endl;
            // TODO shutdown services
            io.stop();
         }
      });

      //
      // services
      // TODO: replication, local tasks, bridge, producer
      //
      ReplicatedStorage  stg(io, cfg);
      MasterWriteSvc wrt(io, cfg, stg);
      WebSvc   web(io, cfg);

      // 
      // events' processing
      //
      boost::thread_group tg;
      for (int i = 0; i < boost::thread::hardware_concurrency(); ++i ) {
         tg.create_thread([&io]() { io.run(); });
      }
      tg.join_all();
   }

   return 0;
}

