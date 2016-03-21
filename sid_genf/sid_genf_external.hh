
#ifndef __SID_GENF_EXTERNAL_HH__
#define __SID_GENF_EXTERNAL_HH__

#include "data_src.hh"
#include "dummy_external.hh"
#include "external_data.hh"

/*---------------------------------------------------------------------------*/

DUMMY_EXTERNAL_MAP_STRUCT_FORW(EXT_LADDER);

class EXT_LADDER
{
public:
  EXT_LADDER();
  ~EXT_LADDER();

  /*
public:
  uint16 *_scramble_buffer;
  size_t  _scramble_length;

public:
  uint32 *_dest_buffer;
  size_t  _dest_length;

public:
  wire_hits data;

  cros3_threshold_stat trc;
  */

public:
  void __clean();
  EXT_DECL_UNPACK_ARG(uint16 id);
  // Needed if it is part of a select statement
  EXT_DECL_MATCH_ARG(uint16 id);

public:
  DUMMY_EXTERNAL_DUMP(EXT_LADDER);
  DUMMY_EXTERNAL_SHOW_MEMBERS(EXT_LADDER);
  DUMMY_EXTERNAL_ENUMERATE_MEMBERS(EXT_LADDER);
  DUMMY_EXTERNAL_ZERO_SUPPRESS_INFO_PTRS(EXT_LADDER);
};

DUMMY_EXTERNAL_MAP_STRUCT(EXT_LADDER);
DUMMY_EXTERNAL_WATCHER_STRUCT(EXT_LADDER);
DUMMY_EXTERNAL_CORRELATION_STRUCT(EXT_LADDER);

/*---------------------------------------------------------------------------*/

#endif//__SID_GENF_EXTERNAL_HH__
