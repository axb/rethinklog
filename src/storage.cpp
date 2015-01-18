#include "storage.h"

Storage::Storage(Broker & bk, int argc, char * argv[])
   : _config( argc, argv )
{
   // setup data dir
}

Config & Storage::config()
{
   return _config;
}

Stripe * Storage::stripe(std::string topic_, int part_)
{
   return NULL;
}

void Storage::housekeeping()
{
}

void Storage::abort()
{
   // close all open files
}

Config::Config(int argc, char * argv[])
{
   // 
}

std::string Config::dataDir() const
{
   return std::string();
}

int Config::nthreads() const
{
   return 0;
}

Stripe::Stripe(std::string topic_, int part_, Config & cfg_)
{
}

void Stripe::append()
{
}
