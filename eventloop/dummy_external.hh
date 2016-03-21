
#ifndef __DUMMY_EXTERNAL_HH__
#define __DUMMY_EXTERNAL_HH__

#include "../common/signal_id.hh"
#include "multi_info.hh"
#include "enumerate.hh"

struct watcher_event_info;
struct enumerate_watchers_info; // Forward declaration
struct enumerate_correlations_info;
struct correlation_list;
struct used_zero_suppress_info;
struct pretty_dump_info;
struct mix_rnd_seed;

/*---------------------------------------------------------------------------*/

#define DUMMY_EXTERNAL_MAP_ENUMERATE_MAP_MEMBERS(type) \
public: void enumerate_map_members(const signal_id &id, \
                                   const enumerate_info &info, \
                                   enumerate_fcn callback,void *extra) const { }

#define DUMMY_EXTERNAL_MAP_STRUCT_FORW(type) class type##_map
#define DUMMY_EXTERNAL_MAP_STRUCT(type) \
class type##_map \
{                 \
public:           \
  void map_members(const type &src MAP_MEMBERS_PARAM) const { } \
  DUMMY_EXTERNAL_MAP_ENUMERATE_MAP_MEMBERS(type); \
};

/*---------------------------------------------------------------------------*/

#define DUMMY_EXTERNAL_DUMP(type) \
public: void dump(const signal_id &id,pretty_dump_info &pdi) const { }
#define DUMMY_EXTERNAL_MAP_MEMBERS(type) \
"DUMMY_EXTERNAL_MAP_MEMBERS is deprecated .. mapping now done by ..._map class"
#define DUMMY_EXTERNAL_SHOW_MEMBERS(type) \
public: void show_members(const signal_id &id,const char *unit) const { }
#define DUMMY_EXTERNAL_ENUMERATE_MEMBERS(type) \
public: void enumerate_members(const signal_id &id, \
                               const enumerate_info &info, \
                               enumerate_fcn callback,void *extra) const { }
#define DUMMY_EXTERNAL_ZERO_SUPPRESS_INFO_PTRS(type) \
public: void zero_suppress_info_ptrs(used_zero_suppress_info &used_info) { }

/*---------------------------------------------------------------------------*/

#define DUMMY_EXTERNAL_WATCHER_STRUCT(type) \
template<typename Twatcher_channel>  \
class type##_watcher                 \
{                                    \
 public:                             \
  bool enumerate_watchers(const signal_id &id,enumerate_watchers_info *info) { return false; } \
  void watch_members(const type &src,watcher_event_info *watch_info WATCH_MEMBERS_PARAM) const { } \
};

/*---------------------------------------------------------------------------*/

#define DUMMY_EXTERNAL_CORRELATION_STRUCT(type) \
class type##_correlation             \
{                                    \
 public:                             \
  bool enumerate_correlations(const signal_id &id,enumerate_correlations_info *info) { return false; } \
  void add_corr_members(const type &src,correlation_list *list WATCH_MEMBERS_PARAM) const { } \
};

/*---------------------------------------------------------------------------*/

#define DUMMY_EXTERNAL_CALIB_MAP_STRUCT(type) \
class type##_calib_map                       \
{                                             \
 public:                                      \
  void show(const signal_id &id) { }          \
  void set_rnd_seed(const mix_rnd_seed &rnd_seed) { } \
  void clear() { }                            \
  void map_members(const type &src MAP_MEMBERS_PARAM) const { } \
  DUMMY_EXTERNAL_MAP_ENUMERATE_MAP_MEMBERS(type); \
};

/*---------------------------------------------------------------------------*/

#endif//__DUMMY_EXTERNAL_HH__
