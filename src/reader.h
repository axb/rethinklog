#pragma once

/*
   Read related protocol & pipeline
*/

class Broker;
class Storage;

class ReadSvc
{
public:
   ReadSvc(Broker& brk_, Storage& stg_);
};