// -*- C++ -*-

AD413A_4CH()
{
  MEMBER(DATA16 data[4] ZERO_SUPPRESS);
  
  list(0 <= index < 4)
    {
      UINT16 item NOENCODE
	{
	  0_12: value;
	  13_15: 0;

	  ENCODE(data[index],(value=value));
	}
    }
}


SUBEVENT(GAMMA_K8_AD413A)
{
  select several
    {
      multi adc = AD413A_4CH();
    }
}

EVENT
{
  ad413a = GAMMA_K8_AD413A(type=10,subtype=1);

}

SIGNAL(EA , ad413a.adc.data[0],(DATA16, float));
SIGNAL(EB , ad413a.adc.data[1],(DATA16, float));
SIGNAL(T ,  ad413a.adc.data[3],(DATA16, float));

/*
SIGNAL(MWPC_X_TL, ev.tdc.data[0],DATA12);
SIGNAL(MWPC_X_TR, ev.tdc.data[1],DATA12);
SIGNAL(MWPC_Y_TU, ev.tdc.data[2],DATA12);
SIGNAL(MWPC_Y_TD, ev.tdc.data[3],DATA12);
SIGNAL(MWPC_X_EO, ev.qdc.data[0],DATA12);
SIGNAL(MWPC_X_EE, ev.qdc.data[1],DATA12);
SIGNAL(MWPC_Y_EO, ev.qdc.data[2],DATA12);
SIGNAL(MWPC_Y_EE, ev.qdc.data[3],DATA12);

SIGNAL(SCIN_T,    ev.tdc.data[8],DATA12);
SIGNAL(SCIN_E,    ev.qdc.data[8],DATA12);

*/
