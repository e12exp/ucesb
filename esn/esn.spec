// -*- C++ -*-

// #include "spec/spec.spec"

// #include "spec/vme_caen_v775.spec"

// #include "is430_05_vme.spec"

#define EVENT_FPP        1
#define EVENT_ANGLECUT   2
#define EVENT_UNCHECKED  3
#define EVENT_SCALER     4

external EXTERNAL_DATA_SKIP();

SCALER_EVENT()
{
  UINT16 header
    {
      0_15: EVENT_SCALER;
    }

  external data = EXTERNAL_DATA_SKIP(); 
};

MWPC()
{
  UINT16 fifo_start_dummy NOENCODE
    {
      0_15: undefined; // 0xffff;
    }
  
  UINT16 header
    {
      0_12: wc;
      13_15: 0b010;
    }

  UINT16 dummy NOENCODE
    {
      0_15: undefined; // 0xffff;
    }

  UINT16 event_counter;

  list(0<=index<header.wc)
    {
      UINT16 width_address NOENCODE
	{
	  0_15 : w;
	}
    }
  
  UINT16 end_marker NOENCODE
    {
      0_15: undefined; // 0x8083 usually...
    }
  
  if (EXTERNAL has_align32_word)
    {
      UINT16 fifo_end_dummy NOENCODE
	{
	  0_15: undefined; // 0x8003 usually...
	}
    }
}

VDC()
{
  UINT16 fifo_start_dummy NOENCODE
    {
      0_15: undefined; // 0xffff;
    }
  
  UINT16 header
    {
      0_12: wc;
      13_15: 0b010;
    }

  UINT16 dummy NOENCODE
    {
      0_15: undefined; // 0xffff;
    }

  UINT16 event_counter;

  list(0<=index<header.wc)
    {
      UINT16 width_address NOENCODE
	{
	  0_15 : w;
	}
    }

  UINT16 end_marker NOENCODE
    {
      0_15: undefined; // 0x8083 usually...
    }

  if (EXTERNAL has_align32_word)
    {
      UINT16 fifo_end_dummy NOENCODE
	{
	  0_15: undefined; // 0x8003 usually...
	}
    }
}

VME_CAEN_V775_16(geom,
		 crate)
{
  MEMBER(DATA12_OVERFLOW data[32] ZERO_SUPPRESS);

  UINT16 header1 NOENCODE
    {
      // 0_7: undefined;
      8_13:  count;
    }

  UINT16 header2 NOENCODE
    {
      0_7:   crate = MATCH(crate);
      8_10:  0b010;
      11_15: geom = MATCH(geom);
    }

  list(0<=index<header1.count)
    {
      UINT16 ch_data1 NOENCODE
	{
	  0_11:  value;

	  12:    overflow;
	  13:    underflow;
	  14:    valid;
	  
	  // 15: undefined;

	}

      UINT16 ch_data2 NOENCODE
	{
	  0_4:   channel;
	  
	  8_10:  0b000;
	  11_15: geom = CHECK(geom);

	}

      ENCODE(data[ch_data2.channel],(value=ch_data1.value,overflow=ch_data1.overflow));
    }
  
  UINT16 eob1
    {
      0_15:  event_number_lo;
    }
  
  UINT16 eob2
    {
      0_7:   event_number_hi;
      8_10:  0b100;
      11_15: geom = CHECK(geom);
      // NOENCODE;
    }
}


external EXT_MWPC();
external EXT_VDC();
external EXT_SCI();

NORMAL_EVENT()
{
  UINT16 header
    {
      0_15: eventtype = RANGE(EVENT_FPP,EVENT_UNCHECKED);
      // 0_15: 3;
    }

  UINT16 beam_pol;
  
  UINT16 monkey NOENCODE
    {
      0_15: 0xaffe;
      // 0_15: 0x0011;
    }
  
  UINT16 timer_high;
  UINT16 timer_low;

  // TODO: make this work!
  //  mwpc[0] = MWPC();
  //  mwpc[1] = MWPC();

  external mwpc[0] = EXT_MWPC();
  external mwpc[1] = EXT_MWPC();

  external vdc[0] = EXT_VDC();
  external vdc[1] = EXT_VDC();

  external sci = EXT_SCI();

  /*
  mwpc[0] = MWPC();
  mwpc[1] = MWPC();

  vdc[0] = MWPC();
  vdc[1] = MWPC();
  */
  /*
    These never existed, they were due to a bug in pax_input.cc
    giving the wrong event length
  UINT16 unknown1 NOENCODE
    {
      0_15: undefined;
    }

  UINT16 unknown2 NOENCODE
    {
      0_15: undefined;
    }
  */

  // Data followed by two caen modules

  // datasrc32 caen0 = VME_CAEN_V775(geom=0x1f,crate=0xff);
  // datasrc32 caen1 = VME_CAEN_V775(geom=0x1f,crate=0xff);



  //caen0 = VME_CAEN_V775_16(geom=0x1f,crate=0xff);
  //caen1 = VME_CAEN_V775_16(geom=0x1f,crate=0xff);

};

SUBEVENT(ESN_EVENT)
{
  select several
    {
      scalers = SCALER_EVENT();
      ev = NORMAL_EVENT();

    }
}

EVENT
{
  // dummy = DUMMY_SUBEVENT();
  // vme = IS430_05_VME(type=10,subtype=1);

  esn = ESN_EVENT();




}

SIGNAL(MWPC4_2H,,EXT_MWPC_RAW);
SIGNAL(VDC2_2H,,EXT_VDC_RAW);
