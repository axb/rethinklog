#pragma once

///////////////////////////////////////////////////////////////////////////////////
//
// Store - mem-mapped file with typed aggregates.
//
//    In-memory and on-disk representations are the same.
//
//    Object may take one or many 'pages'.
//    If object's resize requires more space then we have - we split current file.
//
//    Reference are managed centrally.
//    Internal references are in-placed, external ( both incoming and outgoing(?) ) - centralized.
//
//    Indices.
//
//
///////////////////////////////////////////////////////////////////////////////////

class AggrHandler
{
public:
   /// scalars handling

   // strings (bytes) resize

   /// composites handling
   
   // array resize

   // map resize


   /// references handling

   // root object reference

   // sub-element reference

   /// init from page
};

///////////////////////////////////////////////////////////////////////////////////
//
// Manages file mapping, splitting, gives context to stored aggregates.
//
///////////////////////////////////////////////////////////////////////////////////
class PersistentState
{

};
