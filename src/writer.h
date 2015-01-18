#pragma once

/*
   Write related protocol & pipeline
*/

class Broker; 
class Storage;

class WriteSvc
{
public:
   WriteSvc(Broker& brk_, Storage& stg_);

   //
   // save -> ack -> process local tasks 
   //             -> notify subscribers 
   //
   void buildPipeline();

   //
   //  
   //
};