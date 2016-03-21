
#ifndef __HACKY_USER_STRUCT_HH__
#define __HACKY_USER_STRUCT_HH__

// Careful.  These structures are also parsed by psdc in order to
// prepare for enumeration for ntuple writing.  I.e. only a small
// subset of C struct features are allowed!

#ifndef __PSDC__
#include "zero_suppress.hh"
#endif

struct CROS3_wire_hit
{
  uint16 wire;
  uint16 start;

  USER_STRUCT_FCNS_DECL;
};

struct CROS3_hits
{
  // to handle all possible data, it should be 256*128 (0x8000), 
  // ntuple has other limits, so lets say four hits per wire

  raw_list_zero_suppress<CROS3_wire_hit,CROS3_wire_hit,0x0400> hits;

  USER_STRUCT_FCNS_DECL;
};

struct hacky_user_struct
{
  CROS3_hits ccb[2];

  USER_STRUCT_FCNS_DECL;
};

#endif//__HACKY_USER_STRUCT_HH__

