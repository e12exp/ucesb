
#ifndef __CLUSTER_STRUCT_HH__
#define __CLUSTER_STRUCT_HH__

// Careful.  These structures are also parsed by psdc in order to
// prepare for enumeration for ntuple writing.  I.e. only a small
// subset of C struct features are allowed!

#ifndef __PSDC__
#include "struct_fcns.hh"
#include "zero_suppress.hh"
#endif

struct hit_event_CLUSTER_one
{
  uint16 start;
  uint16 end;

  float sum;

  USER_STRUCT_FCNS_DECL;
};

struct hit_event_CLUSTER
{
  raw_list_zero_suppress<hit_event_CLUSTER_one,hit_event_CLUSTER_one,512> hits;

  USER_STRUCT_FCNS_DECL;
};

struct hit_event_PEAK_one
{
  float center;
  float sum;
  float c0;

  USER_STRUCT_FCNS_DECL;
};

struct hit_event_PEAK
{
  raw_list_zero_suppress<hit_event_PEAK_one,hit_event_PEAK_one,256> hits;

  USER_STRUCT_FCNS_DECL;
};

#endif//__CLUSTER_STRUCT_HH__

