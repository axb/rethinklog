#pragma once

/*
   Write related protocol
*/

class Broker; 
class Storage;

class WriteSvc
{
public:
   WriteSvc(Broker& brk_, Storage& stg_);
};