// -*- C++ -*-

#include "spec/spec.spec"

TACQUILA_GTB(sam,gtb)
{
  MEMBER(DATA12 tac[32][16] ZERO_SUPPRESS);
  MEMBER(DATA12 adc[32][16] ZERO_SUPPRESS);
  
  UINT32 header
    {
      0_9:   count;

      16_23: local_event_counter;
      24:    gtb = MATCH(gtb);
      28_31: sam = MATCH(sam);
    }

  list(0 <= index < header.count)
    {
      UINT32 ch_data NOENCODE
	{
	  0_11:  tac_value;
	  12_21: adc_value;
	  22_25: channel;
	  // 26: 0;
	  27_31: module;

	  ENCODE(tac[module][channel],(value=tac_value));
	  ENCODE(adc[module][channel],(value=adc_value));
	}
    }
}


SUBEVENT(TACQUILA_RPC)
{
  select several
    {
      norevisit tacq[0]  = TACQUILA_GTB(gtb=0,sam=3);
      norevisit tacq[1]  = TACQUILA_GTB(gtb=1,sam=3);
      norevisit tacq[2]  = TACQUILA_GTB(gtb=0,sam=4);
      norevisit tacq[3]  = TACQUILA_GTB(gtb=1,sam=4);
      norevisit tacq[4]  = TACQUILA_GTB(gtb=0,sam=5);
      norevisit tacq[5]  = TACQUILA_GTB(gtb=1,sam=5);
      norevisit tacq[6]  = TACQUILA_GTB(gtb=0,sam=6);
      norevisit tacq[7]  = TACQUILA_GTB(gtb=1,sam=6);
      norevisit tacq[8]  = TACQUILA_GTB(gtb=0,sam=7);
      norevisit tacq[9]  = TACQUILA_GTB(gtb=1,sam=7);
      norevisit tacq[10] = TACQUILA_GTB(gtb=0,sam=8);
      norevisit tacq[11] = TACQUILA_GTB(gtb=1,sam=8);
      norevisit tacq[12] = TACQUILA_GTB(gtb=0,sam=9);
      norevisit tacq[13] = TACQUILA_GTB(gtb=1,sam=9);
      norevisit tacq[14] = TACQUILA_GTB(gtb=0,sam=10);
    }
}

EVENT
{
  rpc = TACQUILA_RPC(type=15,subtype=2605);

  ignore_unknown_subevent;
}

