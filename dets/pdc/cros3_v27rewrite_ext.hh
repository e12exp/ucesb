
#ifndef __CROS3_V27REWRITE_EXT_HH__
#define __CROS3_V27REWRITE_EXT_HH__

#include "data_src.hh"
#include "dummy_external.hh"
#include "external_data.hh"

/*---------------------------------------------------------------------------*/

struct WIRE_START_END
{
  uint8 wire;
  uint8 start;
  uint8 stop;  

public:
  bool operator<(const wire_hit &rhs) const
  {
    int wd = wire - rhs.wire;

    if (wd)
      return wd < 0;

    return start < rhs.start;
  }

  void __clean()
  {
    wire  = 0;
    start = 0;
    stop  = 0;
  }

public:
  void dump(const signal_id &id,pretty_dump_info &pdi) const
  {
    char buf[32];
    sprintf(buf,"(%d,%d,%d)",wire,start,stop);
    pretty_dump(id,buf,pdi);
  }

public:
  DUMMY_EXTERNAL_SHOW_MEMBERS(WIRE_START_END);
  DUMMY_EXTERNAL_ENUMERATE_MEMBERS(WIRE_START_END);
  DUMMY_EXTERNAL_ZERO_SUPPRESS_INFO_PTRS(WIRE_START_END);
};

DUMMY_EXTERNAL_MAP_STRUCT(WIRE_START_END);
DUMMY_EXTERNAL_CALIB_MAP_STRUCT(WIRE_START_END);
DUMMY_EXTERNAL_WATCHER_STRUCT(WIRE_START_END);
DUMMY_EXTERNAL_CORRELATION_STRUCT(WIRE_START_END);

inline int get_enum_type(WIRE_START_END *) { return 0; }

/*---------------------------------------------------------------------------*/

#endif//__CROS3_V27REWRITE_EXT_HH__
