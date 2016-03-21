
#ifndef __STRUCT_MAPPING_HH__
#define __STRUCT_MAPPING_HH__

#include "multi_chunk.hh"

void setup_unpack_map();
void do_unpack_map(MAP_MEMBERS_SINGLE_PARAM);
void do_raw_reverse_map(MAP_MEMBERS_SINGLE_PARAM);

#include "raw_data_map.hh"

// The const_cast< ptr<T>* > is to be able to keep the const on the
// source argument to map_members, while still being able to update
// the internal ptr<T> variables that point to particular instances
// within a certain event.

#define STRUCT_MIRROR_FCNS_DECL(name)           \
 public:                                        \
  void map_members(const name &src MAP_MEMBERS_PARAM) const; \
  void enumerate_map_members(const signal_id &id,   \
                             const enumerate_info &info, \
                             enumerate_fcn callback,void *extra) const; \
  template<typename __T,typename T_map> \
  void map_members(const multi_chunks<__T,T_map> &src,const ptr<__T> *single MAP_MEMBERS_PARAM) const \
  { *(const_cast< ptr<__T>* >(single)) = src.map_members(*this MAP_MEMBERS_ARG); }
#define STRUCT_MIRROR_TYPE(type)         type##_map
#define STRUCT_MIRROR_NAME(name)         name
#define STRUCT_MIRROR_STRUCT(type)       STRUCT_MIRROR_TYPE(type)
#define STRUCT_MIRROR_BASE(type)         STRUCT_MIRROR_TYPE(type)
#define STRUCT_MIRROR_TEMPLATE_ARG(arg)  arg##_map,arg
#define STRUCT_MIRROR_TEMPLATE_ARG_N(arg,array)  arg##_map array,arg array
#define STRUCT_ONLY_LAST_UNION_MEMBER    1

#include "gen/struct_mirror.hh"
#include "gen/raw_struct_mirror.hh"

#undef  STRUCT_MIRROR_FCNS_DECL
#undef  STRUCT_MIRROR_TYPE
#undef  STRUCT_MIRROR_NAME
#undef  STRUCT_MIRROR_STRUCT
#undef  STRUCT_MIRROR_BASE
#undef  STRUCT_MIRROR_TEMPLATE_ARG
#undef  STRUCT_MIRROR_TEMPLATE_ARG_N
#undef  STRUCT_ONLY_LAST_UNION_MEMBER

extern unpack_event_map the_unpack_event_map;
extern raw_event_map the_raw_event_reverse_map;

#endif//__STRUCT_MAPPING_HH__
