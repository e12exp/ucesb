// -*- C++ -*-

#include "spec/spec.spec"

#include "cros3_ext.spec"
#include "land_type_subtype.spec"

EVENT
{
  // Old markings
  cros3 = CROS3_SUBEVENT(type=10,subtype=1,control=9);
  // Also eat events with unambigous type/subtype
  cros3 = CROS3_SUBEVENT(type=85,subtype=8500);

  cros3_rewrite = CROS3_REWRITE_SUBEVENT(type=85,subtype=8510);

  ignore_unknown_subevent;
}

/*
SIGNAL(SCI1_1_T,vme.tdc0.data[0],DATA12);
SIGNAL(SCI1_2_T,vme.tdc0.data[1],DATA12);
SIGNAL(SCI1_1_E,vme.qdc0.data[0],DATA12);
SIGNAL(SCI1_2_E,vme.qdc0.data[1],DATA12);
*/

// SIGNAL(SST1_1_E,sst.sst1.data[0][0],DATA12);

