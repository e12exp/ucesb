// -*- C++ -*-

#include "spec/spec.spec"

#include "is430_05_vme.spec"

EVENT
{
  vme = IS430_05_VME(type=10,subtype=1);




}

// This enforces zero suppression at strip level

SIGNAL(ZERO_SUPPRESS: DSSSD2_F_32);
SIGNAL(ZERO_SUPPRESS: DSSSD2_B_32);

// ADC channels

SIGNAL(BACK1_E,      vme.adc[0].data[0], (DATA12, float));
SIGNAL(BACK2_E,      vme.adc[0].data[1], (DATA12, float));

SIGNAL(MONE_E,       vme.adc[0].data[17],(DATA12, float));
SIGNAL(MONDE_E,      vme.adc[0].data[18],(DATA12, float));
SIGNAL(MONTGT_E,     vme.adc[0].data[22],(DATA12, float));

SIGNAL(DSSSD1_F_1_E, vme.adc[1].data[0],
       DSSSD1_F_32_E,vme.adc[1].data[31],(DATA12, float));

SIGNAL(DSSSD1_B_17_E,vme.adc[2].data[0],
       DSSSD1_B_32_E,vme.adc[2].data[15],(DATA12, float));

SIGNAL(DSSSD1_B_1_E, vme.adc[2].data[16],
       DSSSD1_B_16_E,vme.adc[2].data[31],(DATA12, float));

SIGNAL(DSSSD2_F_1_E, vme.adc[3].data[0],
       DSSSD2_F_32_E,vme.adc[3].data[31],(DATA12, float));

SIGNAL(DSSSD2_B_1_E, vme.adc[4].data[0],
       DSSSD2_B_32_E,vme.adc[4].data[31],(DATA12, float));

// TDC channels

SIGNAL(DSSSD1_FT,    vme.tdc[0].data[0],(DATA12, float));
SIGNAL(DSSSD1_BT,    vme.tdc[0].data[1],(DATA12, float));
SIGNAL(DSSSD2_FT,    vme.tdc[0].data[2],(DATA12, float));
SIGNAL(DSSSD2_BT,    vme.tdc[0].data[3],(DATA12, float));
SIGNAL(TMON,         vme.tdc[0].data[4],(DATA12, float));
SIGNAL(TBACK,        vme.tdc[0].data[5],(DATA12, float));

SIGNAL(DSSSD2_B_1_T, vme.tdc[1].data[0],
       DSSSD2_B_32_T,vme.tdc[1].data[31],(DATA12, float));

SIGNAL(DSSSD2_F_1_T, vme.tdc[2].data[0],
       DSSSD2_F_32_T,vme.tdc[2].data[31],(DATA12, float));

// Scaler channels

SIGNAL(CLOCK,        vme.scaler0.data[0],DATA32);
SIGNAL(CNTPROTONS,   vme.scaler0.data[1],DATA32);
SIGNAL(CNTEBIS,      vme.scaler0.data[2],DATA32);

// Values that will be calculated in the user function

SIGNAL(TEBIS,                           ,(DATA32, float));
SIGNAL(TSHORT,                          ,(DATA32, float));

SIGNAL(TLAST,,DATA32);

// This one should not map!!!  One can just as well take
// it from the unpack event.  It is here just to test LAST_EVENT
SIGNAL(LAST_EVENT: TIME,     vme.header.time_stamp,uint32);

