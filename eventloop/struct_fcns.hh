
#ifndef __STRUCT_FCNS_HH__
#define __STRUCT_FCNS_HH__

#include "../common/signal_id.hh"
#include "enumerate.hh"

struct used_zero_suppress_info;
struct pretty_dump_info;

#define CALIB_STRUCT_FCNS_DECL                  \
 public:                                        \
  void __clean();                               \
  void dump(const signal_id &id,                \
            pretty_dump_info &pdi) const;       \
  void show_members(const signal_id &id,const char *unit) const; \
  void enumerate_members(const signal_id &id,   \
                         const enumerate_info &info, \
                         enumerate_fcn callback,void *extra) const; \

#define USER_STRUCT_FCNS_DECL                   \
  CALIB_STRUCT_FCNS_DECL                        \
  void zero_suppress_info_ptrs(used_zero_suppress_info &used_info);

#define STRUCT_FCNS_DECL(name)                  \
  USER_STRUCT_FCNS_DECL;                        \
 public:                                        \
  void map_members(const class name##_map &map MAP_MEMBERS_PARAM); /* non-const for multi ptr assignment*/ \
  /*void map_members(const struct name##_calib_map &map) const;*/ \
  uint32 get_event_counter() const; /* only implemented for some modules! */ \
  uint32 get_event_counter_offset(uint32 start) const; /* only implemented for some modules! */ \
  bool good_event_counter_offset(uint32 expect) const; /* only implemented for some modules! */ \
  ;

#define UNIT(x)   // replace with nothing, handled by PSDC

#endif//__STRUCT_FCNS_HH__

