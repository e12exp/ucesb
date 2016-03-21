
#ifndef __PDC_USER_STRUCT_HH__
#define __PDC_USER_STRUCT_HH__

// Careful.  These structures are also parsed by psdc in order to
// prepare for enumeration for ntuple writing.  I.e. only a small
// subset of C struct features are allowed!

#ifndef __PSDC__
#include "zero_suppress.hh"
#endif

struct PDC_wire_hit
{
  uint8 wire;
  uint8 start;
  uint8 stop;

  USER_STRUCT_FCNS_DECL;
};

#define MAX_CROS3_hits 0x0400

struct PDC_hits
{
  // to handle all possible data, it should be 256*128 (0x8000), 
  // ntuple has other limits, so lets say four hits per wire

  raw_list_ii_zero_suppress<PDC_wire_hit,PDC_wire_hit,MAX_CROS3_hits> hits;

  USER_STRUCT_FCNS_DECL;
};

#endif//__PDC_USER_STRUCT_HH__

