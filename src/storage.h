#pragma once

#include <string>

class Broker;
class Stripe;

/*
   Disk related defines and operations
*/

class Config
{
public:
   Config(int argc, char *argv[] );

   std::string dataDir() const;
   int nthreads() const;


};

class Storage
{
   Config _config;
public:
   Storage(Broker& bk, int argc, char *argv[]);

   Config& config();

   //
   Stripe* stripe(std::string topic_, int part_);

   // 
   void housekeeping();

   // when finishing by ctrl-c
   void abort();
};

class Stripe
{
public:
   Stripe(std::string topic_, int part_, Config& cfg_);


   void append( );
};