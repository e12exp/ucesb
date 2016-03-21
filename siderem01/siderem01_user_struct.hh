
#ifndef __SIDEREM01_USER_STRUCT_HH__
#define __SIDEREM01_USER_STRUCT_HH__

#include "cluster_struct.hh"

struct hit_event_siderem01
{
  hit_event_CLUSTER CLUSTER[4];

  hit_event_PEAK    PEAK_RAW[4];
  hit_event_PEAK    PEAK[4];

  USER_STRUCT_FCNS_DECL;
};

#endif//__SIDEREM01_USER_STRUCT_HH__

