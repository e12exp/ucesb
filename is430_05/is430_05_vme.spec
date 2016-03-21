// -*- C++ -*-

#include "spec/land_std_vme.spec"

SUBEVENT(IS430_05_VME)
{
  header = LAND_STD_VME();

  select several
    {
      multi scaler0 = VME_CAEN_V830(geom=30);
      
      multi tdc[0] = VME_CAEN_V775(geom=16,crate=129);
      multi tdc[1] = VME_CAEN_V775(geom=17,crate=130);
      multi tdc[2] = VME_CAEN_V775(geom=18,crate=131);

      multi adc[0] = VME_CAEN_V792(geom=0,crate=1);
      multi adc[1] = VME_CAEN_V785(geom=1,crate=2);
      multi adc[2] = VME_CAEN_V785(geom=2,crate=3);
      multi adc[3] = VME_CAEN_V785(geom=3,crate=4);
      multi adc[4] = VME_CAEN_V785(geom=4,crate=5);
    }
}
