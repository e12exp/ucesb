// -*- C++ -*-

#include "spec/spec.spec"

// #include "cros3_v27rewrite.spec"

external EXTERNAL_DATA16();

SUBEVENT(CROS3_SUBEVENT)
{
  // We'll eat the entire subevent...
  
  external data16 = EXTERNAL_DATA16();
}

EVENT
{
  // handle a cros3 subevent

  cros3 = CROS3_SUBEVENT(type=10,subtype=1,control=9);


}

/*
SIGNAL(SCI1_1_T,vme.tdc0.data[0],DATA12);
SIGNAL(SCI1_2_T,vme.tdc0.data[1],DATA12);
SIGNAL(SCI1_1_E,vme.qdc0.data[0],DATA12);
SIGNAL(SCI1_2_E,vme.qdc0.data[1],DATA12);
*/

// SIGNAL(SST1_1_E,sst.sst1.data[0][0],DATA12);

