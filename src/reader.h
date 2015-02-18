#pragma once

/*
   Read related protocol & pipeline
*/

class Broker;
class ReplicatedStorage;

class ReadSvc
{
public:
   ReadSvc(Broker& brk_, ReplicatedStorage& stg_);
};