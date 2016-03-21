
#ifndef __DSSSD_STRUCT_HH__
#define __DSSSD_STRUCT_HH__

// Careful.  These structures are also parsed by psdc in order to
// prepare for enumeration for ntuple writing.  I.e. only a small
// subset of C struct features are allowed!

#ifndef __PSDC__
#include "zero_suppress.hh"
#endif

struct hit_event_DSSSD_one
{
  float T;
  float E;

  float X;
  float Y;

  USER_STRUCT_FCNS_DECL;
};

struct hit_event_DSSSD
{
  raw_list_zero_suppress<hit_event_DSSSD_one,hit_event_DSSSD_one,32> hits;

  USER_STRUCT_FCNS_DECL;
};

#endif//__DSSSD_STRUCT_HH__

