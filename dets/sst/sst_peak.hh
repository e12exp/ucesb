
#ifndef __SID_PEAK_HH__
#define __SID_PEAK_HH__

#include "cluster_struct.hh"

struct sid_data_val
{
  int   strip;
  float value;
};

struct sid_data
{
  int          n;
  sid_data_val data[1024];
};

void siderem_peaks(int det,sid_data &data,
		   hit_event_PEAK &peaks);

#endif//__SID_PEAK_HH__
