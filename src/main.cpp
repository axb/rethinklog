////////////////////////////////////////////////////
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

#include <chrono>
#include <conio.h>

int main(int argc, char *argv[]) {
   // TODO: setup logging

   Config cfg(argc, argv);

   /// tests
   std::cout << "RethinkLog - fucking revolution (version 100500)" << std::endl << "press any key to start..." << std::endl;
   _getch();
   {
      auto start = std::chrono::high_resolution_clock::now();

      auto st = Stripe::create("boo-ga-ga-000000111111", cfg);
      int x = 0;
      for (; x < 10 * 1000 * 1000; ++x) {
         st->append("dodood", "jshgjhslk\r\njghsdlkjfhglksdfhglkj\r\nfdshglkdshjfhsljhflksjhfdlj\r\nkshdflkjhsdljfhlkjdshgsgfdfgri"
                    "u3fhihfkldshlkfjhdslf\"\"\"\" sadasdsadsadasdsadsadasdad{}{} jhkjfhsdkjhfkjsdhkfhh}sdfdsfdsfdsfs}} "
                    "u3fhihfkldshlkfjhdslf\"\"\"\" sadasdsadsadasdsadsadasdad{}{} jhkjfhsdkjhfkjsdhkfhh}sdfdsfdsfdsfs}} "
                    "u3fhihfkldshlkfjhdslf\"\"\"\" sadasdsadsadasdsadsadasdad{}{} jhkjfhsdkjhfkjsdhkfhh}sdfdsfdsfdsfs}} "
                    "u3fhihfkldshlkfjhdslf\"\"\"\" sadasdsadsadasdsadsadasdad{}{} jhkjfhsdkjhfkjsdhkfhh}sdfdsfdsfdsfs}} "
                    "jshgjhslk\r\njghsdlkjfhglksdfhglkj\r\nfdshglkdshjfhsljhflksjhfdlj\r\nkshdflkjhsdljfhlkjdshgsgfdfgri", "");
         if (x % 50000 == 0)
            std::cout << "|";
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> diff = end - start;
      std::cout << std::endl
         << "made : " << x << " records, " << std::endl
         << "took : " << diff.count() << " seconds" << std::endl
         << "perf : " << x / diff.count() << " recs/sec " << std::endl
         << "press any key to exit...";
   }
   _getch();
   return 0;
   /// } end tests

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
      ReplicatedStorage stg(io, cfg);
      MasterWriteSvc    wrt(io, cfg, stg);
      WebSvc            web(io, cfg);

      // 
      // events' processing
      //
      boost::thread_group tg;
      for (int i = 0; i < boost::thread::hardware_concurrency(); ++i) {
         tg.create_thread([&io]() { io.run(); });
      }
      tg.join_all();
   }

   return 0;
}

