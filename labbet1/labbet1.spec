// -*- C++ -*-

#include "spec/midas.spec"

SUBEVENT(EV_EVENT)
{
  select several
    {
      adc1 = SIMPLE_DATA(group=0x20);
    }
}


EVENT
{
  // vme = RPC2006_VME(type=10,subtype=1);

  ev = EV_EVENT();


  
}

SIGNAL(DSSSD1_F_1_E, ev.adc1.data[0],
       DSSSD1_F_16_E,ev.adc1.data[15],(DATA16, float));
SIGNAL(DSSSD1_B_1_E, ev.adc1.data[16],
       DSSSD1_B_16_E,ev.adc1.data[31],(DATA16, float));

/*
SIGNAL(SCI1_1_T,vme.tdc0.data[0],DATA12);
SIGNAL(SCI1_2_T,vme.tdc0.data[1],DATA12);
SIGNAL(SCI1_1_E,vme.qdc0.data[0],DATA12);
SIGNAL(SCI1_2_E,vme.qdc0.data[1],DATA12);
*/

