#include "broker.h"

Broker::Broker()
{
}

void Broker::start( int nthreads )
{
   // begin worker threads
}

void Broker::stop()
{
}

int Broker::queue(std::string name_)
{
   return 0;
}

void Broker::submit(int q_, std::function<void()> f_)
{
   // FIXME: enqueue
   f_();
}
