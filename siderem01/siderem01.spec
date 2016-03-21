// -*- C++ -*-

#include "spec/spec.spec"

#include "siderem01_vme.spec"
#include "land_type_subtype.spec"

EVENT
{
  sst = SIDEREM01_VME(type = 10,           subtype = 1, control=91);
  sst = SIDEREM01_VME(type = 10,           subtype = 1, control=9); // r3-12
  /*
  sst1 = SIDEREM01_VME(type = SUBEVENT_SST, subtype = SUBEVENT_SST_DATA8200, procid = 40);
  sst2 = SIDEREM01_VME(type = SUBEVENT_SST, subtype = SUBEVENT_SST_DATA8200, procid = 42);
  */
  sst = SIDEREM01_VME(type = SUBEVENT_SST, subtype = SUBEVENT_SST_DATA8200, procid = 40);
  sst = SIDEREM01_VME(type = SUBEVENT_SST, subtype = SUBEVENT_SST_DATA8200, procid = 42);

  sstrewrite = SIDEREM01_VME(type = 82, subtype = 8210);

  ignore_unknown_subevent;
}

SIGNAL(PREV_TRIG,,uint16);
/*
SIGNAL(SST1_1_E,    sst.sst[0].data[0][0],
       SST1_320_E,  sst.sst[0].data[0][319], (DATA12,float));
SIGNAL(SST1_321_E,  sst.sst[0].data[1][0],
       SST1_640_E,  sst.sst[0].data[1][319], (DATA12,float));
SIGNAL(SST1_641_E,  sst.sst[0].data[2][0],
       SST1_1024_E, sst.sst[0].data[2][383], (DATA12,float));

SIGNAL(SST2_1_E,    sst.sst[1].data[0][0],
       SST2_320_E,  sst.sst[1].data[0][319], (DATA12,float));
SIGNAL(SST2_321_E,  sst.sst[1].data[1][0],
       SST2_640_E,  sst.sst[1].data[1][319], (DATA12,float));
SIGNAL(SST2_641_E,  sst.sst[1].data[2][0],
       SST2_1024_E, sst.sst[1].data[2][383], (DATA12,float));

SIGNAL(SST3_1_E,    sst.sst[2].data[0][0],
       SST3_320_E,  sst.sst[2].data[0][319], (DATA12,float));
SIGNAL(SST3_321_E,  sst.sst[2].data[1][0],
       SST3_640_E,  sst.sst[2].data[1][319], (DATA12,float));
SIGNAL(SST3_641_E,  sst.sst[2].data[2][0],
       SST3_1024_E, sst.sst[2].data[2][383], (DATA12,float));

SIGNAL(SST4_1_E,    sst.sst[3].data[0][0],
       SST4_320_E,  sst.sst[3].data[0][319], (DATA12,float));
SIGNAL(SST4_321_E,  sst.sst[3].data[1][0],
       SST4_640_E,  sst.sst[3].data[1][319], (DATA12,float));
SIGNAL(SST4_641_E,  sst.sst[3].data[2][0],
       SST4_1024_E, sst.sst[3].data[2][383], (DATA12,float));
			     			     
SIGNAL(SST5_1_E,    sst.sst[4].data[0][0],
       SST5_320_E,  sst.sst[4].data[0][319], (DATA12,float));
SIGNAL(SST5_321_E,  sst.sst[4].data[1][0],
       SST5_640_E,  sst.sst[4].data[1][319], (DATA12,float));
SIGNAL(SST5_641_E,  sst.sst[4].data[2][0],
       SST5_1024_E, sst.sst[4].data[2][383], (DATA12,float));
			     			     
SIGNAL(SST6_1_E,    sst.sst[5].data[0][0],
       SST6_320_E,  sst.sst[5].data[0][319], (DATA12,float));
SIGNAL(SST6_321_E,  sst.sst[5].data[1][0],
       SST6_640_E,  sst.sst[5].data[1][319], (DATA12,float));
SIGNAL(SST6_641_E,  sst.sst[5].data[2][0],
       SST6_1024_E, sst.sst[5].data[2][383], (DATA12,float));
			     			     
SIGNAL(SST7_1_E,    sst.sst[6].data[0][0],
       SST7_320_E,  sst.sst[6].data[0][319], (DATA12,float));
SIGNAL(SST7_321_E,  sst.sst[6].data[1][0],
       SST7_640_E,  sst.sst[6].data[1][319], (DATA12,float));
SIGNAL(SST7_641_E,  sst.sst[6].data[2][0],
       SST7_1024_E, sst.sst[6].data[2][383], (DATA12,float));
			     			     
SIGNAL(SST8_1_E,    sst.sst[7].data[0][0],
       SST8_320_E,  sst.sst[7].data[0][319], (DATA12,float));
SIGNAL(SST8_321_E,  sst.sst[7].data[1][0],
       SST8_640_E,  sst.sst[7].data[1][319], (DATA12,float));
SIGNAL(SST8_641_E,  sst.sst[7].data[2][0],
       SST8_1024_E, sst.sst[7].data[2][383], (DATA12,float));
			     			     
SIGNAL(SST9_1_E,    sst.sst[8].data[0][0],
       SST9_320_E,  sst.sst[8].data[0][319], (DATA12,float));
SIGNAL(SST9_321_E,  sst.sst[8].data[1][0],
       SST9_640_E,  sst.sst[8].data[1][319], (DATA12,float));
SIGNAL(SST9_641_E,  sst.sst[8].data[2][0],
       SST9_1024_E, sst.sst[8].data[2][383], (DATA12,float));
			     			     
SIGNAL(SST10_1_E,    sst.sst[9].data[0][0],
       SST10_320_E,  sst.sst[9].data[0][319], (DATA12,float));
SIGNAL(SST10_321_E,  sst.sst[9].data[1][0],
       SST10_640_E,  sst.sst[9].data[1][319], (DATA12,float));
SIGNAL(SST10_641_E,  sst.sst[9].data[2][0],
       SST10_1024_E, sst.sst[9].data[2][383], (DATA12,float));
			     			     
SIGNAL(SST11_1_E,    sst.sst[10].data[0][0],
       SST11_320_E,  sst.sst[10].data[0][319], (DATA12,float));
SIGNAL(SST11_321_E,  sst.sst[10].data[1][0],
       SST11_640_E,  sst.sst[10].data[1][319], (DATA12,float));
SIGNAL(SST11_641_E,  sst.sst[10].data[2][0],
       SST11_1024_E, sst.sst[10].data[2][383], (DATA12,float));
			     			     
SIGNAL(SST12_1_E,    sst.sst[11].data[0][0],
       SST12_320_E,  sst.sst[11].data[0][319], (DATA12,float));
SIGNAL(SST12_321_E,  sst.sst[11].data[1][0],
       SST12_640_E,  sst.sst[11].data[1][319], (DATA12,float));
SIGNAL(SST12_641_E,  sst.sst[11].data[2][0],
       SST12_1024_E, sst.sst[11].data[2][383], (DATA12,float));
*/		     			     

SIGNAL(SST12_1024_E,,(DATA12,float));
SIGNAL(ZERO_SUPPRESS: SST1_1024);

