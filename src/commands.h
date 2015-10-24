#pragma once

#include <string>
#include <functional>

namespace commands
{
   class Cmd {
   public:
      virtual ~Cmd() {}
      virtual void run() = 0;
   };

   //
   // define class, alter methods, attributes etc
   //
   class Meta : public Cmd {
      std::string _qry;
   public:
      Meta( std::string qry ) : _qry(qry) {}
   };

   //
   // call instance or class method
   //
   class Call : public Cmd {
   public:
      Call( std::string path, std::string method, std::string args);
   };

   //
   // select and aggregate data
   //
   class Select : public Cmd {
      std::string _qry;
   public:
      Select( std::string qry ) : _qry(qry) {}
   };

   //
   // do some internal work
   //
   class Housekeeping : public Cmd {
   public:
   };

   //
   // execute number of commands (storm like)
   //
   class Stream: public Cmd {
   public:

   };
}
