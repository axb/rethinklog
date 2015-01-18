////////////////////////////////////////////////////
//
// parameters:
//    config 
//    data directory
//
//    web ui
//    pub|sub
//    reader
//    writer
//    bridge
//    producer - functionality to turn on/off
//
////////////////////////////////////////////////////
#include <conio.h>

#include "broker.h"
#include "storage.h"
#include "zmq_transport.h"
#include "writer.h"
#include "reader.h"
#include "pubsub.h"

int main(int argc, char *argv[]) {

   // TODO: setup logging

   Broker broker;
   {
      // start services
      Storage stg(broker, argc, argv);
      ZmqTransport net(broker, stg.config());
      WriteSvc wrt(broker, stg);
      ReadSvc rdr(broker, stg);
      PubSubSvc pubsub(broker);

      // TODO: replication, admin, local tasks, bridge, producer

      broker.start(stg.config().nthreads());

      // simple wait
      _getch();
   }
   broker.stop();

   return 0;
}

