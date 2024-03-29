// -*- C++ -*-

#define VME_CAEN_V792 VME_CAEN_V775
#define VME_CAEN_V785 VME_CAEN_V775

VME_CAEN_V775(geom,
	      crate)
{
  MEMBER(DATA12_OVERFLOW data[32] ZERO_SUPPRESS);

  UINT32 header NOENCODE
    {
      // 0_7: undefined;
      8_13:  count;
      16_23: crate = MATCH(crate);
      24_26: 0b010;
      27_31: geom = MATCH(geom);
    }

  list(0<=index<header.count)
    {
      UINT32 ch_data NOENCODE
	{
	  0_11:  value;

	  12:    overflow;
	  13:    underflow;
	  14:    valid;
	  
	  // 15: undefined;

	  16_20: channel;
	  
	  24_26: 0b000;
	  27_31: geom = CHECK(geom);

	  ENCODE(data[channel],(value=value,overflow=overflow));
	}
    }
  
  UINT32 eob
    {
      0_23:  event_number;
      24_26: 0b100;
      27_31: geom = CHECK(geom);
      // NOENCODE;
    }
}

