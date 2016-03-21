// -*- C++ -*-

#include "spec/spec.spec"

#include "rpc2006_vme.spec"

EVENT
{
  vme = RPC2006_VME(type=10,subtype=1);




}

SIGNAL(SCI1_1_T,vme.tdc0.data[0],DATA12);
SIGNAL(SCI1_2_T,vme.tdc0.data[1],DATA12);
SIGNAL(SCI1_1_E,vme.qdc0.data[0],DATA12);
SIGNAL(SCI1_2_E,vme.qdc0.data[1],DATA12);


