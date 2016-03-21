// -*- C++ -*-

LAND_STD_VME()
{
  UINT32 failure
    {
      0:  fail_general;
      1:  fail_data_corrupt;
      2:  fail_data_missing;
      3:  fail_data_too_much;
      4:  fail_event_counter_mismatch;
      5:  fail_readout_error_driver;
      6:  fail_unexpected_trigger;

      27: has_no_zero_suppression;
      28: has_multi_adctdc_counter0;
      29: has_multi_scaler_counter0;
      30: has_multi_event;
      31: has_time_stamp;
    }
  
  if (failure.has_time_stamp) {
    UINT32 time_stamp;
  }
  
  if (failure.has_multi_event) {
    UINT32 multi_events;
  }
  
  if (failure.has_multi_scaler_counter0) {
    UINT32 multi_scaler_counter0;
  }

  if (failure.has_multi_adctdc_counter0) {
    UINT32 multi_adctdc_counter0;
  }
  
  select several
    {
      norevisit tdc0 = VME_CAEN_V775(geom=20,crate=20);
    }  
}



SUBEVENT(LAND_VME)
{
  vme = LAND_STD_VME();



}
