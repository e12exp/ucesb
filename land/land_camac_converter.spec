// -*- C++ -*-

SUBEVENT(LAND_CAMAC_CONVERTER)
{
  UINT16 tpat;
  UINT16 coinc2 { 0_15: 0x0000; }
  UINT16 coinc3 { 0_15: 0x0000; }
  UINT16 pad { 
    0_15: 0x4321;
  }

  tPHI7079  = CAMAC_PHILLIPS_7164H(channels=16,mark_channel_no=1);
  tPHI7186  = CAMAC_PHILLIPS_7164H(channels=16,mark_channel_no=1); // not before COSMIC2
  aPHI12160 = CAMAC_PHILLIPS_7186H(channels=16,mark_channel_no=1);

  qSIA0415 = CAMAC_SILENA_4418Q(channels=8,mark_channel_no=0);
  qSIA0458 = CAMAC_SILENA_4418Q(channels=8,mark_channel_no=0);
  qSIA0418 = CAMAC_SILENA_4418Q(channels=8,mark_channel_no=0);
  tSIA0428 = CAMAC_SILENA_4418T(channels=8,mark_channel_no=0);
  tSIA0400 = CAMAC_SILENA_4418T(channels=8,mark_channel_no=0);
  tSIA0426 = CAMAC_SILENA_4418T(channels=8,mark_channel_no=0);
  tSIA0401 = CAMAC_SILENA_4418T(channels=8,mark_channel_no=0);
}
