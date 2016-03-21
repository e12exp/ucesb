// -*- C++ -*-

#include "spec/midas32.spec"

SUBEVENT(EV_EVENT)
{
  select several
    {
      adc[0] = MIDAS_CAEN_V785(group=0x20/*32*/);
      adc[1] = MIDAS_CAEN_V785(group=0x21/*33*/);
      tdc[0] = MIDAS_CAEN_V1190(group=0x18/*24*/);
      scaler = MIDAS_CAEN_V830(group=2);
    }
}


EVENT
{
  ev = EV_EVENT();
}

SIGNAL(ZERO_SUPPRESS: DSSSD1_F_1);
SIGNAL(ZERO_SUPPRESS: DSSSD1_B_1);

SIGNAL(DSSSD1_F_1_E, ev.adc[0].data[0],
       DSSSD1_F_16_E,ev.adc[0].data[15],(DATA12, float));
SIGNAL(DSSSD1_B_1_E, ev.adc[0].data[16],
       DSSSD1_B_16_E,ev.adc[0].data[31],(DATA12, float));
SIGNAL(GAS_E, ev.adc[1].data[0],(DATA12, float));
SIGNAL(SI_1_E, ev.adc[1].data[1],(DATA12, float));
SIGNAL(SI_2_E, ev.adc[1].data[2],(DATA12, float));
SIGNAL(PAD_E, ev.adc[1].data[3],(DATA12, float));


//SIGNAL(DSSSD2_F_1_E, ev.adc[1].data[0],
//       DSSSD2_F_16_E,ev.adc[1].data[15],(DATA12, float));
//SIGNAL(DSSSD2_B_1_E, ev.adc[1].data[16],
//       DSSSD2_B_16_E,ev.adc[1].data[31],(DATA12, float));

SIGNAL(DSSSD1_F_1_T, ev.tdc[0].data[0],
       DSSSD1_F_16_T,ev.tdc[0].data[15],(DATA16, float));

SIGNAL(COUNTER_1,    ev.scaler.data[0],DATA32);
SIGNAL(COUNTER_2,    ev.scaler.data[1],DATA32);
