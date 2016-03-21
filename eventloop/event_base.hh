
#ifndef __EVENT_BASE_HH__
#define __EVENT_BASE_HH__

#include "structures.hh"
#include "hex_dump_mark.hh"

class event_base
{
public:
#if USE_THREADING || USE_MERGING
  // FILE_INPUT_EVENT *event;
  void        *_file_event;
#endif
  hex_dump_mark_buf _unpack_fail;
  unpack_event _unpack;
#ifndef USE_MERGING
  raw_event    _raw;
  cal_event    _cal;

#ifdef USER_STRUCT
  USER_STRUCT   _user;
#endif
#endif//!USE_MERGING
};

// The event data structure!

// This should not be used by any user code!  
// Use CURRENT_EVENT instead!!
// Otherwise multi-threading/merging will not function properly!!!

extern event_base _static_event;

// See user.hh for the following entry...

#ifdef CALIB_STRUCT
extern CALIB_STRUCT      _calib;
#endif

#endif//__EVENT_BASE_HH__
