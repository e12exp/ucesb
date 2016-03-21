// -*- C++ -*-

SUBEVENT(IS446_VME)
{
  UINT32 dummy_header NOENCODE 
    { 
      0_31: 0xfefefefe;
    }
  
  select several
    {
      norevisit scaler[0] = VME_CAEN_V830(geom=31);

      norevisit adc[0] = VME_CAEN_V785(geom=31,crate= 1);
      norevisit adc[1] = VME_CAEN_V785(geom=31,crate= 2);
      norevisit adc[2] = VME_CAEN_V785(geom=31,crate= 3);
      norevisit adc[3] = VME_CAEN_V785(geom=31,crate= 4);
      norevisit adc[4] = VME_CAEN_V785(geom=31,crate= 5);
      norevisit adc[5] = VME_CAEN_V785(geom=31,crate= 6);
      norevisit adc[6] = VME_CAEN_V785(geom=31,crate= 7);

      norevisit tdc[0] = VME_CAEN_V785(geom=31,crate= 8);
      norevisit tdc[1] = VME_CAEN_V785(geom=31,crate= 9);
      norevisit tdc[2] = VME_CAEN_V785(geom=31,crate=10);
      norevisit tdc[3] = VME_CAEN_V785(geom=31,crate=11);
      norevisit tdc[4] = VME_CAEN_V785(geom=31,crate=12);
    }
  
  UINT32 dummy_footer NOENCODE 
    { 
      0_31: 0xefefefef;
    }

  // The DAQ is broken, cannot correctly account the
  // number of data-words in the event, and therefore
  // each event has an additional 32 bits of crap...
  
  UINT32 garbage_footer NOENCODE 
    { 
      0_31: garbage;
    }
  
}

SUBEVENT(IS446_VME2)
{
}
