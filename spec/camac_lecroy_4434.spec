// -*- C++ -*-

CAMAC_LECROY_4434(channels)
{
  MEMBER(DATA24 data[32] ZERO_SUPPRESS);

  list(0<=index<channels)
    {
      UINT32 ch_data NOENCODE
	{
	  0_23:  value;
	  24_31: 0;
	  
	  ENCODE(data[index],(value=value));
	}
    }
}



