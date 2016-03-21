// -*- C++ -*-

#include "spec/land_std_vme.spec"

SUBEVENT(RPC2006_VME)
{
  header = LAND_STD_VME();

  select several
    {
      norevisit tdc0 = VME_CAEN_V775(geom=16,crate=129);
      norevisit qdc0 = VME_CAEN_V792(geom=0,crate=1);
      norevisit adc0 = VME_CAEN_V785(geom=1,crate=2);
      norevisit scaler0 = VME_CAEN_V830(geom=30);
    }
}
