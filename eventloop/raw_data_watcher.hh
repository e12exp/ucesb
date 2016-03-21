#ifndef __RAW_DATA_WATCHER_HH__
#define __RAW_DATA_WATCHER_HH__

#define __LAND02_CODE__

#include "typedef.hh"
#include "watcher_channel.hh"

#include "detector_requests.hh"

#include <map>

typedef std::map<signal_id,watcher_present_channels*> id_present_channels_map;

struct enumerate_watchers_info
{
  watcher_window    *_watcher;
  detector_requests *_requests;

  uint _rescale_min;
  uint _rescale_max;

  id_present_channels_map _id_present_channels;

  int _map_no;
};

// #include "zero_suppress_watcher.hh"

template<typename T,typename Twatcher_channel>
class watcher_channel_wrap
{
public:
  Twatcher_channel _data;

public:
  void event(DATA8  value,watcher_event_info *watch_info);
  void event(DATA12 value,watcher_event_info *watch_info);
  //void event(DATA12_OVERFLOW value);
  //void event(DATA12_RANGE value);
  void event(DATA16 value,watcher_event_info *watch_info);
  void event(DATA24 value,watcher_event_info *watch_info);
  void event(DATA32 value,watcher_event_info *watch_info);
  void event(DATA64 value,watcher_event_info *watch_info);
  void event(uint8  value,watcher_event_info *watch_info);
  void event(uint16 value,watcher_event_info *watch_info);
  void event(uint32 value,watcher_event_info *watch_info);
  void event(uint64 value,watcher_event_info *watch_info);
  void event(float  value,watcher_event_info *watch_info);
  void event(double value,watcher_event_info *watch_info);
  //void event(uint32 value);
};

template<typename T,typename Twatcher_channel>
class data_watcher
{
public:
  data_watcher()
  {
    // src  = NULL;
    // _dest     = NULL;
    // _zzp_info = NULL;

    _watch = NULL;
  }

public:
  // T *src;
  // T                        *_dest;
  // const zero_suppress_info *_zzp_info;

  watcher_channel_wrap<T,Twatcher_channel> *_watch;

public:
  // void set_dest(T *dest);

public:
  void watch_members(const T &src,watcher_event_info *watch_info WATCH_MEMBERS_PARAM) const;
  bool enumerate_watchers(const signal_id &id,enumerate_watchers_info *info);

};


#define DECL_PRIMITIVE_WATCHER(type)					\
  template<typename Twatcher_channel>					\
  class type##_watcher : public data_watcher<type,Twatcher_channel>	\
  { };

DECL_PRIMITIVE_WATCHER(uint8);
DECL_PRIMITIVE_WATCHER(uint16);
DECL_PRIMITIVE_WATCHER(uint32);
DECL_PRIMITIVE_WATCHER(uint64);

DECL_PRIMITIVE_WATCHER(DATA8);
DECL_PRIMITIVE_WATCHER(DATA12);
DECL_PRIMITIVE_WATCHER(DATA12_RANGE);
DECL_PRIMITIVE_WATCHER(DATA12_OVERFLOW);
DECL_PRIMITIVE_WATCHER(DATA16);
DECL_PRIMITIVE_WATCHER(DATA24);
DECL_PRIMITIVE_WATCHER(DATA32);
DECL_PRIMITIVE_WATCHER(DATA64);
DECL_PRIMITIVE_WATCHER(rawdata8);
DECL_PRIMITIVE_WATCHER(rawdata12);
DECL_PRIMITIVE_WATCHER(rawdata16);
DECL_PRIMITIVE_WATCHER(rawdata24);
DECL_PRIMITIVE_WATCHER(rawdata32);
DECL_PRIMITIVE_WATCHER(rawdata64);
DECL_PRIMITIVE_WATCHER(float);
DECL_PRIMITIVE_WATCHER(double);

// TODO: Make sure that the user cannot specify source array indices
// in SIGNAL which are outside the available items.  Bad names get
// caught by the compiler, array indices not.

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n>
class raw_array_watcher
{
public:
  T_watcher _items[n];

public:
  T_watcher &operator[](size_t i) 
  { 
    // This function is used by the setting up of the arrays, i.e. we
    // can have checks here
    if (i < 0 || i >= n) 
      ERROR("Watcher index outside bounds (%d >= %d)",i,n); 
    return _items[i]; 
  }
  const T_watcher &operator[](size_t i) const 
  {
    // This function is used by the mapping operations (since that one
    // needs a const function), no checks here (expensive, since
    // called often)
    return _items[i]; 
  }

public:
  static void watch_item(const T &src,const T_watcher &watch,
			 watcher_event_info *watch_info  
			 WATCH_MEMBERS_PARAM);

public:
  void watch_members(const raw_array<Tsingle,T,n> &src,
		     watcher_event_info *watch_info 
		     WATCH_MEMBERS_PARAM) const;
  void watch_members(const raw_array_zero_suppress<Tsingle,T,n> &src,
		     watcher_event_info *watch_info 
		     WATCH_MEMBERS_PARAM) const;
  template<int max_entries>
  void watch_members(const raw_array_multi_zero_suppress<Tsingle,T,n,max_entries> &src,
		     watcher_event_info *watch_info 
		     WATCH_MEMBERS_PARAM) const;
  void watch_members(const raw_list_zero_suppress<Tsingle,T,n> &src,
		     watcher_event_info *watch_info
		     WATCH_MEMBERS_PARAM) const;
  void watch_members(const raw_list_ii_zero_suppress<Tsingle,T,n> &src,
		     watcher_event_info *watch_info
		     WATCH_MEMBERS_PARAM) const;

public:
  bool enumerate_watchers(const signal_id &id,enumerate_watchers_info *info);

};

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n,int n1>
class raw_array_watcher_1 :
  public raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>
{
public:
  bool enumerate_watchers(const signal_id &id,enumerate_watchers_info *info);
};

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n,int n1,int n2>
class raw_array_watcher_2 :
  public raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>
{
public:
  bool enumerate_watchers(const signal_id &id,enumerate_watchers_info *info);
};


// For the mirror data structures, which will use the
// raw_array_zero_suppress_watcher name
#define raw_array_zero_suppress_watcher   raw_array_watcher
#define raw_list_zero_suppress_watcher    raw_array_watcher
#define raw_list_ii_zero_suppress_watcher raw_array_watcher

#define raw_array_zero_suppress_1_watcher raw_array_watcher_1
#define raw_array_zero_suppress_2_watcher raw_array_watcher_2
#define raw_list_zero_suppress_1_watcher  raw_array_watcher_1
#define raw_list_zero_suppress_2_watcher  raw_array_watcher_2

// The multi-entry array should eat one index

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n,int max_entries>
class raw_array_multi_watcher :
  public raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>
{
};

#define raw_array_multi_zero_suppress_watcher raw_array_multi_watcher

template <typename Twatcher_channel>
class unpack_subevent_base_watcher
{
public:
  void watch_members(const unpack_subevent_base &src,
		     watcher_event_info *watch_info
		     WATCH_MEMBERS_PARAM) const { }
  bool enumerate_watchers(const signal_id &id,enumerate_watchers_info *info) { return false; }

};

template <typename Twatcher_channel>
class unpack_event_base_watcher
{
public:
  void watch_members(const unpack_event_base &src,
		     watcher_event_info *watch_info
		     WATCH_MEMBERS_PARAM) const { }
  bool enumerate_watchers(const signal_id &id,enumerate_watchers_info *info) { return false; }

};

template <typename Twatcher_channel>
class raw_event_base_watcher
{
public:
  void watch_members(const raw_event_base &src,
		     watcher_event_info *watch_info
		     WATCH_MEMBERS_PARAM) const { }
  bool enumerate_watchers(const signal_id &id,enumerate_watchers_info *info) { return false; }

};

template <typename Twatcher_channel>
class cal_event_base_watcher
{
public:
  void watch_members(const cal_event_base &src,
		     watcher_event_info *watch_info
		     WATCH_MEMBERS_PARAM) const { }
  bool enumerate_watchers(const signal_id &id,enumerate_watchers_info *info) { return false; }

};

#endif//__RAW_DATA_WATCHER_HH__

