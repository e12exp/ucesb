
#ifndef __SST_EXT_HH__
#define __SST_EXT_HH__

#include "data_src.hh"
#include "dummy_external.hh"
#include "external_data.hh"
#include "zero_suppress.hh"
#include "raw_data.hh"

/*---------------------------------------------------------------------------*/

DUMMY_EXTERNAL_MAP_STRUCT_FORW(EXT_SST);

union EXT_SST_header
{
  struct
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint32 count : 12; // 0..11
    uint32 local_event_counter : 4; // 12..15
    uint32 local_trigger : 4; // 16..19
    uint32 siderem : 4; // 20..23
    uint32 gtb : 4; // 24..27
    uint32 sam : 4; // 28..31
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32 sam : 4; // 28..31
    uint32 gtb : 4; // 24..27
    uint32 siderem : 4; // 20..23
    uint32 local_trigger : 4; // 16..19
    uint32 local_event_counter : 4; // 12..15
    uint32 count : 12; // 0..11
#endif
  };
  uint32  u32;
};

class EXT_SST
{
public:
  EXT_SST();
  ~EXT_SST();

public:
  EXT_SST_header header;
  //raw_array_zero_suppress<DATA12,DATA12,512> data[4];

  raw_array_zero_suppress<DATA12,DATA12,1024> data;

public:
  void __clean();
  EXT_DECL_UNPACK_ARG(uint32 sam,uint32 gtb,uint32 siderem);
  EXT_DECL_UNPACK_ARG(uint32 sam,uint32 gtb,uint32 siderem,uint32 branch);
  // Needed if it is part of a select statement
  EXT_DECL_MATCH_ARG(uint32 sam,uint32 gtb,uint32 siderem);
  EXT_DECL_MATCH_ARG(uint32 sam,uint32 gtb,uint32 siderem,uint32 branch);


public:
  DUMMY_EXTERNAL_DUMP(EXT_SST);
  DUMMY_EXTERNAL_SHOW_MEMBERS(EXT_SST);
  DUMMY_EXTERNAL_ENUMERATE_MEMBERS(EXT_SST);
  DUMMY_EXTERNAL_ZERO_SUPPRESS_INFO_PTRS(EXT_SST);
};

DUMMY_EXTERNAL_MAP_STRUCT(EXT_SST);
DUMMY_EXTERNAL_WATCHER_STRUCT(EXT_SST);
DUMMY_EXTERNAL_CORRELATION_STRUCT(EXT_SST);

/*---------------------------------------------------------------------------*/

#endif//__SST_EXT_HH__
