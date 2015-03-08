#pragma once

#include <memory>
#include <string>
#include <unordered_set>

///////////////////////////////////////////////////////////////////////////////////
//
// Store - mem-mapped file with typed aggregates.
//
//    Main goal: in-memory and on-disk representations are the same.
//
//    Object may take one or many 'pages'.
//    If object's resize requires more space then we have, we may split current file.
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
//             -> array (indexed, multyindexed) (single type, may be polymorphic types)
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
//       -> on instance
//       -> on class
//
///////////////////////////////////////////////////////////////////////////////////
namespace MappedObjects {

class Store;
class Entry;
class Composite;
class MonikerBase;

///////////////////////////////////////////////////////////////////////////////////
//
// Single flat-memory store.
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
   std::unordered_set< Moniker<Composite> > _topLevelComposites;

   // used to 
   //    change offsets and pointers when moving data
   //    cache instances & resolve refs   

public:
   bool open(std::string fname) { return true; }
   void close() {}

   ///  <fixme> get top level composite by ref
   Composite* byRef(std::string ref) { return nullptr; }
   ///
   bool save(Composite& topLevelEntry) { return true; }

   Composite* topLevelComposite(Entry* inner) { return nullptr; }
};

///////////////////////////////////////////////////////////////////////////////////
//
// Naming beast
// ref is like 
//    'store_name.{guid_of_top_level_object}.field.field[index].field["key_in_map"].field["index_name" : "key_in_multyindexed_map"]'
//
///////////////////////////////////////////////////////////////////////////////////
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
   MonikerBase(std::string ref, Store* store = nullptr) : _ptr(nullptr) {} ///< <fixme> resolve   

   Entry* resolve(Store* store = nullptr) { return _ptr; }

   std::string ref() const { return _ref; }
   Store* store() const { return _store; }
   bool isNull() const { return _ptr; }
};

template< class TType = Entry >
class Moniker : public MonikerBase
{
public:
   Moniker(std::string ref, Store* store = nullptr) : MonikerBase(ref, store) {}

   /// <todo> check types - stored against requested
   TType* operator ->() { return static_cast<TType*>(_ptr); }
   TType* operator *() { return static_cast<TType*>(_ptr); }
};


/// <todo>
class HeapStore : public Store {}; // optimized for one type of objects
class CollectionsStore : public Store {}; // optimized for named collections (flat lists) of references
class TreeStore : public Store {}; // optimized for trees (hierarchies) of references
class DisctributedStore : public Store {}; // optimized for distributed (across network) systems

///////////////////////////////////////////////////////////////////////////////////
//
// Manipulator classes
//
///////////////////////////////////////////////////////////////////////////////////
class Entry
{
protected:
   // data in the mapping
   // == mmap_base + _offset
   void* _ptr;
   uint32_t _size;

public:
   // stored size
   virtual uint32_t size() const { return _size; }

   // Moniker
   virtual std::string path() const { return ""; }
};

//
// scalar fixed-sized basic types
//
class FixedSizeScalar : public Entry {};

class Int32 : public FixedSizeScalar {};
class Int64 : public FixedSizeScalar {};
class Float : public FixedSizeScalar {};
class DateTime : public FixedSizeScalar {};

//
// scalar var-sized basic types
//
class VariableSizeEntry : public Entry
{
public:
   // called by child when it needs more space
   //    moves all the subsequent data after given point
   virtual void requestNewSize(uint32_t size) {}
};

class String : public VariableSizeEntry {};
class ByteArray : public VariableSizeEntry {};
class Blob : public VariableSizeEntry {};

//
// composite types
//
class Composite : public VariableSizeEntry
{
public:
   // names' resolution
   //    instanciates all the path
   virtual Entry* byRef(std::string ref) { return nullptr; }
};

class Aggregate : public Composite
{
public:
};

class Array : public Composite
{
public:
   Entry* operator[] (int ix) { return nullptr; }
   Entry* operator[] (std::string ix) { return nullptr; }
};

template <class TItem>
class Collection : public Array
{
public:
   TItem* operator[] (int ix) { return nullptr; }
   TItem* operator[] (std::string ix) { return nullptr; }
};

} //< namespace MappedObjects

///////////////////////////////////////////////////////////////////////////////////
//
// tests
//
///////////////////////////////////////////////////////////////////////////////////
namespace MappedObjects {

class PartyData;

class Agreement : public Aggregate
{
public:
   DateTime _date;
   String _number;
   Moniker<PartyData> _customer;
   Collection < String > _covenants;
};

class PartyData : public Aggregate
{
public:
   String _name;

   Collection< Agreement > _docs;
};
}
