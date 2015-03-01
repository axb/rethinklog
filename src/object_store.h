#pragma once

#include <memory>
#include <string>

namespace MappedObjects {

///////////////////////////////////////////////////////////////////////////////////
//
// Store - mem-mapped file with typed aggregates.
//
//    Main goal: in-memory and on-disk representations are the same.
//
//    Object may take one or many 'pages'.
//    If object's resize requires more space then we have - we split current file.
//
//    Reference are managed centrally (on per file basis).
//    Internal references are in-placed, external ( both incoming and outgoing(?) ) - centralized.
//
//    Indices.
//    ?
//
// Type model -----------------------------------------------------------------------------
//    entry -> scalar
//             -> number
//             -> datetime
//             -> fixed-length string(bytes)
//             -> reference
//          -> composite
//             -> aggregate : list<entry> (multiple types)
//             -> varying bytes
//             -> array (indexed, multyindexed) (single type)
//             -> tag list
//
// Store model ----------------------------------------------------------------------------
//    heap
//    named collections (groups)
//    named hierarchies
//
// Behavior model -------------------------------------------------------------------------
//    messages (instance methods)
//    static (class) methods
//    event listeners (hooks)
//       ->on instance
//       ->on class
//
///////////////////////////////////////////////////////////////////////////////////
class Store;

class Entry
{
   /// position and size
   uint64_t _offset;
   uint64_t _size;

   /// data in the mapping
   /// == mmap_base + _offset
   void* _ptr; 

public:
   // stored size (enum?)

   // Moniker
};

class Scalar : public Entry {};

class Composite : public Entry
{
public:
   // names' resolution
   //    instanciates all the path
   virtual Entry* byRef( std::string ref ) { return nullptr; }

   // called by child when it needs more space
   //    moves all the data after given point
   virtual void reserve( uint64_t where, uint64_t howMuch) {}
};

class Aggregate : public Composite
{

};

class Array : public Composite
{

};

//
// Naming beast
// ref is like 
//    'store_name.{guid_of_top_level_object}.field.field[index].field["key_in_map"].field["index_name" : "key_in_multyindexed_map"]'
//
class MonikerBase
{
protected:
   /// <todo>  can be made static if 'one file == one process'
   //          other way - get from prefix of 'ref' and then from static array of Store
   Store* _store; 

   std::string _ref;
   uint64_t _offset;
   Entry* _ptr;
public:
   MonikerBase( std::string ref, Store* store = nullptr ) : _ptr( nullptr ) {} ///< <fixme> resolve   

   bool resolve( Store* store ) { return true; }

   std::string ref() const { return _ref; }
   Store* store() const { return _store; }
   bool isNull() const { return _ptr; }
};

template< class TType = Entry >
class Moniker : public MonikerBase
{
public:
   Moniker( std::string ref, Store* store = nullptr ) : MonikerBase( ref, store) {}

   /// <todo> check types - stored against requested
   TType* operator ->( ) { return static_cast<TType*>(_ptr); } 
   TType* operator *( ) { return static_cast<TType*>(_ptr); }
};

///////////////////////////////////////////////////////////////////////////////////
//
// Manages 
//    file mapping, 
//    splitting, 
//    gives context to stored aggregates,
//    allows movement of data between files
//
///////////////////////////////////////////////////////////////////////////////////
class Store
{
   // instantiated object model
   //    + resolved monikers
   //    + top level objects' index
   // used to 
   //    change offsets and pointers when moving data
   //    cache instances & resolve refs   

public:
   bool start( std::string fname ) { return true; }
   void finish() {}

   ///  <fixme> get top level composite by ref
   Entry* byRef( std::string ref ) { return nullptr; }
};

//
// optimized for one type of objects
//
class HeapStore : public Store {};

//
// optimized for named collections (flat lists) of references
//
class CollectionsStore : public Store {};

//
// optimized for trees (hierarchies) of references
//
class TreeStore : public Store {};

} //< namespace MappedObjects
