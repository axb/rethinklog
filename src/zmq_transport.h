#pragma once


/*
   Networking
*/

class Broker;
class Config;

class ZmqTransport
{
public:
   ZmqTransport(Broker& brk_, Config& cfg_);
};