#ifndef __RAW_DATA_MAP_HH__
#define __RAW_DATA_MAP_HH__

#include "raw_data.hh"

#include "enumerate.hh"

#include "zero_suppress_map.hh"

template<typename T>
bool do_set_dest(void *void_src_map,
		 void *void_dest);

template<typename T>
class data_map
{
public:
  data_map()
  {
    // src  = NULL;
    _dest     = NULL;
    _zzp_info = NULL;
  }

  void enumerate_map_members(const signal_id &id,
			     const enumerate_info &info,
			     enumerate_fcn callback,void *extra) const
  { 
    callback(id,enumerate_info(info,this,get_enum_type((T *) NULL)).set_dest(do_set_dest<T>),extra);
  }

public:
  // T *src;
  T                        *_dest;
  const zero_suppress_info *_zzp_info;

public:
  bool set_dest(T *dest);

public:
  void map_members(const T &src MAP_MEMBERS_PARAM) const;
};

typedef data_map<uint8>  uint8_map;
typedef data_map<uint16> uint16_map;
typedef data_map<uint32> uint32_map;
typedef data_map<uint64> uint64_map;

typedef data_map<DATA64> DATA64_map;
typedef data_map<DATA32> DATA32_map;
typedef data_map<DATA24> DATA24_map;
typedef data_map<DATA16> DATA16_map;
typedef data_map<DATA12> DATA12_map;
typedef data_map<DATA12_OVERFLOW> DATA12_OVERFLOW_map;
typedef data_map<DATA12_RANGE> DATA12_RANGE_map;
typedef data_map<DATA8>  DATA8_map;
typedef data_map<float>  float_map;

// TODO: Make sure that the user cannot specify source array indices
// in SIGNAL which are outside the available items.  Bad names get
// caught by the compiler, array indices not.

template<typename Tsingle_map,typename Tsingle,typename T_map,typename T,int n>
class raw_array_map
{
public:
  T_map _items[n];

public:
  T_map &operator[](size_t i) 
  { 
    // This function is used by the setting up of the arrays, i.e. we
    // can have checks here
    if (i < 0 || i >= n) 
      ERROR("Mapping index outside bounds (%d >= %d)",i,n); 
    return _items[i]; 
  }
  const T_map &operator[](size_t i) const 
  {
    // This function is used by the mapping operations (since that one
    // needs a const function), no checks here (expensive, since
    // called often)
    return _items[i]; 
  }

public:
  void map_members(const raw_array<Tsingle,T,n> &src MAP_MEMBERS_PARAM) const;
  void map_members(const raw_array_zero_suppress<Tsingle,T,n> &src MAP_MEMBERS_PARAM) const;
  template<int max_entries>
  void map_members(const raw_array_multi_zero_suppress<Tsingle,T,n,max_entries> &src MAP_MEMBERS_PARAM) const;
  void map_members(const raw_list_zero_suppress<Tsingle,T,n> &src MAP_MEMBERS_PARAM) const;

  void enumerate_map_members(const signal_id &id,
			     const enumerate_info &info,
			     enumerate_fcn callback,void *extra) const
  { 
    for (int i = 0; i < n; ++i)
      _items[i].enumerate_map_members(signal_id(id,i),info,callback,extra);
  }

};

template<typename Tsingle_map,typename Tsingle,typename T_map,typename T,int n,int n1>
class raw_array_1_map
{
public:
  raw_array_map<Tsingle_map,Tsingle,Tsingle_map,Tsingle,n1> _items[n];

public:
  raw_array_map<Tsingle_map,Tsingle,Tsingle_map,Tsingle,n1> &operator[](size_t i) 
  { 
    // This function is used by the setting up of the arrays, i.e. we
    // can have checks here
    if (i < 0 || i >= n) 
      ERROR("Mapping index outside bounds (%d >= %d)",i,n); 
    return _items[i]; 
  }
  const raw_array_map<Tsingle_map,Tsingle,Tsingle_map,Tsingle,n1> &operator[](size_t i) const 
  {
    // This function is used by the mapping operations (since that one
    // needs a const function), no checks here (expensive, since
    // called often)
    return _items[i]; 
  }

public:
  void map_members(const raw_array_zero_suppress_1<Tsingle,T,n,n1> &map MAP_MEMBERS_PARAM) const;

public:
  void enumerate_map_members(const signal_id &id,
			     const enumerate_info &info,
			     enumerate_fcn callback,void *extra) const
  { 
    for (int i = 0; i < n; ++i)
      for (int i1 = 0; i1 < n1; ++i1)
	_items[i][i1].enumerate_map_members(signal_id(signal_id(id,i),i1),info,callback,extra);
  }
};

// For the mirror data structures, which will use the
// raw_array_zero_suppress_map name
#define raw_array_zero_suppress_map raw_array_map
#define raw_array_zero_suppress_1_map raw_array_1_map

#define raw_list_zero_suppress_map  raw_array_map
#define raw_list_zero_suppress_1_map  raw_array_1_map

// For the list without indices, we just need a simple
// wrapper to handle the standard Tsingle, T and n template
// parameters

template<typename Tsingle_map,typename Tsingle,typename T_map,typename T,int n>
class raw_list_ii_map 
  : public T_map
{
public:
  void map_members(const raw_list_ii_zero_suppress<Tsingle,T,n> &src MAP_MEMBERS_PARAM) const;
};

#define raw_list_ii_zero_suppress_map  raw_list_ii_map

// For the multi-entry array (which has two 'indices', one index, and
// one max-number-of-entries), we only need to have a map with one index

template<typename Tsingle_map,typename Tsingle,typename T_map,typename T,int n,int max_entries>
class raw_array_multi_map
  : public raw_array_map<Tsingle_map,Tsingle,T_map,T,n>
{
};

#define raw_array_multi_zero_suppress_map raw_array_multi_map

/*
template<typename Tsingle,typename T,int n>
class raw_array_zero_suppress_map
{
public:
  data_map<T> _items[n];

public:
  data_map<T> &operator[](size_t i) { if (i < 0 || i >= n) ERROR("Mapping index outside bounds (%d >= %d)",i,n); return _items[i]; }
  const data_map<T> &operator[](size_t i) const { return _items[i]; }
};
*/
class unpack_subevent_base_map
{
public:
  void map_members(const unpack_subevent_base &src MAP_MEMBERS_PARAM) const { }

  void enumerate_map_members(const signal_id &id,
			     const enumerate_info &info,
			     enumerate_fcn callback,void *extra) const
  {
  }

};

class unpack_event_base_map
{
public:
  void map_members(const unpack_event_base &src MAP_MEMBERS_PARAM) const { }

  void enumerate_map_members(const signal_id &id,
			     const enumerate_info &info,
			     enumerate_fcn callback,void *extra) const
  {
  }

};

class raw_event_base_map
{
public:
  void map_members(const raw_event_base &src MAP_MEMBERS_PARAM) const { }

  void enumerate_map_members(const signal_id &id,
			     const enumerate_info &info,
			     enumerate_fcn callback,void *extra) const
  {
  }

};

#endif//__RAW_DATA_MAP_HH__

