// -*- C++ -*-

// The DAQ is rewriting headers, and abusing(?)
// undefined bits
#define VME_CAEN_V830_HEADER_24_25_UNDEFINED

#include "spec/spec.spec"

#include "is446_vme.spec"

EVENT
{
  vme  = IS446_VME(type=10,subtype=1,subcrate=0);
  vme2 = IS446_VME2(type=10,subtype=1,subcrate=1);




}
/*
SIGNAL(SCI1_1_T,vme.tdc0.data[0],DATA12);
SIGNAL(SCI1_2_T,vme.tdc0.data[1],DATA12);
SIGNAL(SCI1_1_E,vme.qdc0.data[0],DATA12);
SIGNAL(SCI1_2_E,vme.qdc0.data[1],DATA12);
*/
/*
SIGNAL(SCI1_1_T,vme.tdc[0].data[0],(DATA12,float));
SIGNAL(SCI1_1_E,vme.adc[0].data[0],(DATA12,float));
*/

SIGNAL(TEBIS         ,                     ,(DATA32,float));
SIGNAL(TSHORT        ,                     ,(DATA32,float));

SIGNAL(OLDTEBIS      ,                     ,(DATA32,float));

SIGNAL(OLDTSHORT     ,vme.scaler[0].data[0],(DATA32,float));
SIGNAL(EBIS          ,vme.scaler[0].data[1],(DATA32,float));
SIGNAL(NACCEPT       ,vme.scaler[0].data[2],(DATA32,float));

SIGNAL(BACK_1_E      ,vme.adc[6].data[0],   (DATA12,float));
SIGNAL(BACK_2_E      ,vme.adc[6].data[1],   (DATA12,float));

SIGNAL(BACK_1_T      ,vme.tdc[4].data[0],   (DATA12,float));
SIGNAL(BACK_2_T      ,vme.tdc[4].data[1],   (DATA12,float));

SIGNAL(MON_1_E       ,vme.adc[6].data[2],   (DATA12,float));
SIGNAL(MON_2_E       ,vme.adc[6].data[3],   (DATA12,float));

SIGNAL(MON_1_T       ,vme.tdc[4].data[4],   (DATA12,float));
SIGNAL(MON_2_T       ,vme.tdc[4].data[5],   (DATA12,float));

SIGNAL(D_1_F_1_E ,vme.adc[0].data[0],
       D_1_F_32_E,vme.adc[0].data[31],  (DATA12,float));
SIGNAL(D_1_B_1_E ,vme.adc[1].data[0],
       D_1_B_32_E,vme.adc[1].data[31],  (DATA12,float));
SIGNAL(D_1_F_1_T ,vme.tdc[0].data[0],
       D_1_F_32_T,vme.tdc[0].data[31],  (DATA12,float));
SIGNAL(D_1_B_1_T ,vme.tdc[1].data[0],
       D_1_B_32_T,vme.tdc[1].data[31],  (DATA12,float));

SIGNAL(D_2_F_1_E ,vme.adc[2].data[0],
       D_2_F_32_E,vme.adc[2].data[31],  (DATA12,float));
SIGNAL(D_2_B_1_E ,vme.adc[3].data[0],
       D_2_B_32_E,vme.adc[3].data[31],  (DATA12,float));
SIGNAL(D_2_F_1_T ,vme.tdc[2].data[0],
       D_2_F_32_T,vme.tdc[2].data[31],  (DATA12,float));

SIGNAL(D_3_F_1_E ,vme.adc[4].data[0],
       D_3_F_32_E,vme.adc[4].data[31],  (DATA12,float));
SIGNAL(D_3_B_1_E ,vme.adc[5].data[0],
       D_3_B_32_E,vme.adc[5].data[31],  (DATA12,float));
SIGNAL(D_3_F_1_T ,vme.tdc[3].data[0],
       D_3_F_32_T,vme.tdc[3].data[31],  (DATA12,float));

SIGNAL(ZERO_SUPPRESS: D_1_F_1);
SIGNAL(ZERO_SUPPRESS: D_1_B_1);

SIGNAL(TLAST,,DATA32);
SIGNAL(TLONG,,DATA32);
