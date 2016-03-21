// -*- C++ -*-

SUBEVENT(LAND_CAMAC_SCALER)
{
  scaler0 = CAMAC_LECROY_4434(channels=32);
  scaler1 = CAMAC_LECROY_4434(channels=32);
  scaler2 = CAMAC_LECROY_4434(channels=32);

  // On special conditions, the event ends here...

  if (EXTERNAL has_timestamp)
    {
      UINT32 timestamp;
      UINT32 endianess { 0_31: 0x87654321; }
    }
}
