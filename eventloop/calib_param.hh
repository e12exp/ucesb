
#ifndef __CALIB_PARAM_HH__
#define __CALIB_PARAM_HH__

#include "struct_calib.hh"

#include "event_base.hh"

//#define CALIB_SLOPE_OFFSET(slope,offset) new_slope_offset(/*__src_ptr*/&(_event._raw.channel),(slope),(offset))
//#define CALIB_OFFSET_SLOPE(offset,slope) new_offset_slope(__src_ptr,(offset),(slope))

#define NEW_SLOPE_OFFSET  new_slope_offset
#define NEW_OFFSET_SLOPE  new_offset_slope

//#define NAME_SLOPE_OFFSET(slope,offset)  _slope_offset
//#define NAME_OFFSET_SLOPE(slope,offset)  _offset_slope

#define CALIB_PARAM(channel,type,...) { \
  the_raw_event_calib_map.channel._calib = NEW_##type(&(_event._raw.channel), \
                                                      &(_event._cal.channel), \
                                                      __VA_ARGS__); \
}

#endif//__CALIB_PARAM_HH__
