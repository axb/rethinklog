#pragma once

#include <string>
#include <functional>

/*
   Task processor
*/

class Broker
{
public:
   Broker();

   // starts worker threads
   void start( int nthreads);

   // stops and joins worker threads
   void stop();

   // get or create tasks' queue by key
   int queue(std::string key_);

   // submit the task to given queue
   void submit(int q_, std::function<void()> f_);


   void abort();
};
