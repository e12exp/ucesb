// -*- C++ -*-

#include "sst_ext.spec"

SIDEREM_BLOCK(sam,gtb,siderem)
{
  MEMBER(DATA12 data[4][512] ZERO_SUPPRESS);

  UINT32 header
    {
      0_11:  count;
      12_15: local_event_counter;
      16_19: local_trigger;
      // trigger = 3 = sync
      20_23: siderem = MATCH(siderem); // 1-15, 1-16, but 16 impossible!
      24_27: gtb = MATCH(gtb); // 0-1
      28_31: sam = MATCH(sam); // 3-14, 15 is data marker!
    }

  list(0<=index<header.count) 
    {
      UINT32 ch_data NOENCODE
	{
	  0_11:  value;
	  12_13: adc_no;  // adc no
	  16_24: channel;

	  28_31: 0xf; // data marker

	  ENCODE(data[adc_no][channel],(value=value));
	}
    }
}

SUBEVENT(SIDEREM01_VME)
{
  select several
    {
      norevisit external sst[0] = EXT_SST(siderem=1,gtb=0,sam=3);
      norevisit external sst[1] = EXT_SST(siderem=2,gtb=0,sam=3);
      norevisit external sst[2] = EXT_SST(siderem=3,gtb=0,sam=3);
      norevisit external sst[3] = EXT_SST(siderem=4,gtb=0,sam=3);

      norevisit external sst[4] = EXT_SST(siderem=1,gtb=0,sam=5);
      norevisit external sst[5] = EXT_SST(siderem=2,gtb=0,sam=5);
      norevisit external sst[6] = EXT_SST(siderem=3,gtb=0,sam=5);
      norevisit external sst[7] = EXT_SST(siderem=4,gtb=0,sam=5);

      norevisit external sst[8] = EXT_SST(siderem=1,gtb=1,sam=5);
      norevisit external sst[9] = EXT_SST(siderem=2,gtb=1,sam=5);
      norevisit external sst[10] = EXT_SST(siderem=3,gtb=1,sam=5);
      norevisit external sst[11] = EXT_SST(siderem=4,gtb=1,sam=5);

      norevisit external sst[12] = EXT_SST(siderem=1,gtb=0,sam=6);
      norevisit external sst[13] = EXT_SST(siderem=2,gtb=0,sam=6);
      norevisit external sst[14] = EXT_SST(siderem=3,gtb=0,sam=6);
      norevisit external sst[15] = EXT_SST(siderem=4,gtb=0,sam=6);

      norevisit external sst[16] = EXT_SST(siderem=1,gtb=1,sam=6);
      norevisit external sst[17] = EXT_SST(siderem=2,gtb=1,sam=6);
      norevisit external sst[18] = EXT_SST(siderem=3,gtb=1,sam=6);
      norevisit external sst[19] = EXT_SST(siderem=4,gtb=1,sam=6);
    }
  /*
  select several
    {
      sst[0] = SIDEREM_BLOCK(siderem=1,gtb=0,sam=3);
      sst[1] = SIDEREM_BLOCK(siderem=2,gtb=0,sam=3);
      sst[2] = SIDEREM_BLOCK(siderem=3,gtb=0,sam=3);
      sst[3] = SIDEREM_BLOCK(siderem=4,gtb=0,sam=3);

      sst[4] = SIDEREM_BLOCK(siderem=1,gtb=0,sam=5);
      sst[5] = SIDEREM_BLOCK(siderem=2,gtb=0,sam=5);
      sst[6] = SIDEREM_BLOCK(siderem=3,gtb=0,sam=5);
      sst[7] = SIDEREM_BLOCK(siderem=4,gtb=0,sam=5);

      sst[8] = SIDEREM_BLOCK(siderem=1,gtb=1,sam=5);
      sst[9] = SIDEREM_BLOCK(siderem=2,gtb=1,sam=5);
      sst[10] = SIDEREM_BLOCK(siderem=3,gtb=1,sam=5);
      sst[11] = SIDEREM_BLOCK(siderem=4,gtb=1,sam=5);
    }
  */
}


