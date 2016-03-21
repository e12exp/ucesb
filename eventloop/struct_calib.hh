
#ifndef __STRUCT_CALIB_HH__
#define __STRUCT_CALIB_HH__

//void setup_calib_map();
void clear_calib_map();
void set_rnd_seed_calib_map();
void show_calib_map();
void do_calib_map();

#include "raw_calib_map.hh"

#define STRUCT_MIRROR_FCNS_DECL(name)           \
 public:                                        \
  void show(const signal_id &id);               \
  void set_rnd_seed(const mix_rnd_seed &rnd_seed); \
  void clear();                                 \
  void map_members(const name &src) const;      \
  void enumerate_map_members(const signal_id &id,   \
                             const enumerate_info &info, \
                             enumerate_fcn callback,void *extra) const;
#define STRUCT_MIRROR_TYPE(type)         type##_calib_map
#define STRUCT_MIRROR_NAME(name)         name
#define STRUCT_MIRROR_STRUCT(type)       STRUCT_MIRROR_TYPE(type)
#define STRUCT_MIRROR_BASE(type)         STRUCT_MIRROR_TYPE(type)
#define STRUCT_MIRROR_TEMPLATE_ARG(arg)  arg##_calib_map,arg
#define STRUCT_MIRROR_TEMPLATE_ARG_N(arg,array)  arg##_calib_map array,arg array
#define STRUCT_ONLY_LAST_UNION_MEMBER    1

#include "gen/raw_struct_mirror.hh"

#undef  STRUCT_MIRROR_FCNS_DECL
#undef  STRUCT_MIRROR_TYPE
#undef  STRUCT_MIRROR_NAME
#undef  STRUCT_MIRROR_STRUCT
#undef  STRUCT_MIRROR_BASE
#undef  STRUCT_MIRROR_TEMPLATE_ARG
#undef  STRUCT_MIRROR_TEMPLATE_ARG_N
#undef  STRUCT_ONLY_LAST_UNION_MEMBER

extern raw_event_calib_map the_raw_event_calib_map;

template<typename T_src>
template<typename T_dest>
void raw_to_cal<T_src>::set_dest(T_dest *dest)
{
  const zero_suppress_info *info;

  info = get_ptr_zero_suppress_info(dest,NULL,false);

  if (info->_type == ZZP_INFO_CALL_LIST_INDEX ||
      info->_type == ZZP_INFO_CALL_LIST_II_INDEX)
    dest = (T_dest *) (((char*) dest) - info->_list._dest_offset);

  _dest = dest;
  _zzp_info = info;
}

#endif//__STRUCT_CALIB_HH__
