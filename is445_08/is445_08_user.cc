
#include "structures.hh"

#include "user.hh"

#include "multi_chunk_fcn.hh"

#include "is445_may08_repack.hh"

// int multi_events = 0;
// int multi_subevents = 0;

#define USE_IS445_MAY08_UNPACK 1
#define USE_IS445_MAY08_NTUPLE 1

#if !USE_CERNLIB
#undef  USE_IS445_MAY08_NTUPLE
#endif

#if USE_IS445_MAY08_UNPACK
is445_may08_unpack _state;
#define INVALIDATE_SCALER_STATE _state.invalidate_scaler_state();
#if USE_IS445_MAY08_NTUPLE
const char* _fortntu_name = NULL;
#endif
#else
#define INVALIDATE_SCALER_STATE 
#endif

int is445_08_user_function_multi(unpack_event *event)
{
  // Our job is to
  //
  // * make sure the event is intact (i.e. the data can safely be unpacked)
  //
  // * make the mapping to subevents for all the modules

#define EVENT_FAILURE_ERROR(failmode,message) \
  { if (event->vme.header.failure.failmode) { INVALIDATE_SCALER_STATE; ERROR(message); } }
#define EVENT_FAILURE_WARNING(failmode,message) \
  { if (event->vme.header.failure.failmode) { INVALIDATE_SCALER_STATE; WARNING(message); } }

  EVENT_FAILURE_ERROR(fail_general,               "Event with DAQ general failure.");
  EVENT_FAILURE_ERROR(fail_data_corrupt,          "Event with DAQ data corruption.");
  EVENT_FAILURE_ERROR(fail_data_missing,          "Event with DAQ data missing.");
  EVENT_FAILURE_ERROR(fail_data_too_much,         "Event with DAQ data too much.");
  EVENT_FAILURE_ERROR(fail_event_counter_mismatch,"Event with DAQ event counter mismatch.");
  EVENT_FAILURE_ERROR(fail_readout_error_driver,  "Event with DAQ readout driver error.");
  EVENT_FAILURE_WARNING(fail_unexpected_trigger,  "Event with DAQ unexpected trigger.");

  // event->vme.header.multi_events
  // event->vme.header.multi_events
  // event->vme.header.multi_events
  // event->vme.header.multi_events

  // Since the code has been built for handling multi-event,
  // we must do the mapping of the modules!

  uint32 scaler_counter0;
  uint32 adctdc_counter0;
  uint32 multi_events;

  if (event->vme.header.failure.has_multi_event)
    {
      // When running in multi-event mode, we must
      // know the start value for the event counters for the modules

      if (!event->vme.header.failure.has_multi_scaler_counter0)
	ERROR("Event counter for scaler at start unknown.");

      // Loop over all the modules which are multi-event, mapping
      // their event using their event counters

      if (!event->vme.header.failure.has_multi_adctdc_counter0)
	ERROR("Event counter for adc/tdc at start unknown.");

      scaler_counter0 = event->vme.header.multi_scaler_counter0;
      adctdc_counter0 = event->vme.header.multi_adctdc_counter0;

      /*
      {
	multi_events++;
	multi_subevents += event->vme.header.multi_events;
	
	if ((multi_events % 100000) == 0)
	  {
	    printf ("Multi: %d (%d, %6.1f)...\n",
		    multi_events,multi_subevents,
		    ((double) multi_subevents) / multi_events);
	    
	  }
      } 
      */

      multi_events = event->vme.header.multi_events;
    }
  else
    {
      // When not running in multi-event mode, the start counters may
      // not be known.  In which case we simply check that they match,
      // to whatever value they have.


      if (event->vme.header.failure.has_multi_scaler_counter0)
	scaler_counter0 = event->vme.header.multi_scaler_counter0;
      else
	{
	  scaler_counter0 = (uint32) -1;
	  if (event->vme.multi_scaler0._num_items)
	    scaler_counter0 = event->vme.multi_scaler0._items[0].get_event_counter();
	}

      if (event->vme.header.failure.has_multi_scaler_counter0)
	adctdc_counter0 = event->vme.header.multi_adctdc_counter0;
      else
	{
	  adctdc_counter0 = (uint32) -1;

	  for (unsigned int i = 0; i < countof(event->vme.multi_adc); i++)
	    if (event->vme.multi_adc[i]._num_items)
	      {
		adctdc_counter0 = event->vme.multi_adc[i]._items[0].get_event_counter();
		goto found_adc_tdc_counter0;
	      }

	  for (unsigned int i = 0; i < countof(event->vme.multi_tdc); i++)
	    if (event->vme.multi_tdc[i]._num_items)
	      {
		adctdc_counter0 = event->vme.multi_tdc[i]._items[0].get_event_counter();
		goto found_adc_tdc_counter0;
	      }
	  
	found_adc_tdc_counter0:
	  ;
	}

      multi_events = 1;
    }

  map_multi_events(event->vme.multi_scaler0,
		   scaler_counter0,
		   multi_events);
  
  for (unsigned int i = 0; i < countof(event->vme.multi_adc); i++)
    map_multi_events(event->vme.multi_adc[i],
		     adctdc_counter0,
		     multi_events);
  
  for (unsigned int i = 0; i < countof(event->vme.multi_tdc); i++)
    map_multi_events(event->vme.multi_tdc[i],
		     adctdc_counter0,
		     multi_events);

  return multi_events;
}

void is445_08_user_function(unpack_event *event,
			    raw_event    *raw_event,
			    cal_event    *cal_event
			    MAP_MEMBERS_PARAM)
{
  /*
  printf ("%7d: %5d %5d %5d %5d\n",
	  event->event_no,
	  raw_event->SCI[0][0].T,
	  raw_event->SCI[0][1].T,
	  raw_event->SCI[0][0].E,
	  raw_event->SCI[0][1].E);
  */
  /*
  printf ("tdc0: "); event->vme.tdc0.data.dump(); printf ("\n");
  printf ("qdc0: "); event->vme.qdc0.data.dump(); printf ("\n");
  printf ("adc0: "); event->vme.adc0.data.dump(); printf ("\n");
  printf ("scaler: "); event->vme.scaler0.data.dump(); printf ("\n");
  */

  // Since we are multi-event, we have to provide TRIGGER and
  // EVENT-numbers in the RAW level as well.  That is handled by
  // generic code.

  // We need to calculate tshort and tebis.


#if USE_IS445_MAY08_UNPACK
  _state.clear();
  
  // Take the trigger from the raw event (calculated by the generic code)
  _state.set_trigger(raw_event->trigger);

  // Loop over the unpack event, modules from this event

  ssize_t i;

  for (bitsone_iterator iter; (i = event->vme.scaler0().data._valid.next(iter)) >= 0; )
    _state.set_scaler_value((uint32) i,event->vme.scaler0().data[i]);

  // Overflow from adc and tdc are ignored (just as in the old unpacker)

  for (unsigned int adc = 0; adc < countof(event->vme.adc); adc++)
    for (bitsone_iterator iter; (i = event->vme.adc[adc]().data._valid.next(iter)) >= 0; )
      _state.set_adc(adc,(uint32) i,event->vme.adc[adc]().data[i].value);

  for (unsigned int tdc = 0; tdc < countof(event->vme.tdc); tdc++)
    for (bitsone_iterator iter; (i = event->vme.tdc[tdc]().data._valid.next(iter)) >= 0; )
      _state.set_tdc(tdc,(uint32) i,event->vme.tdc[tdc]().data[i].value);

  //_state.set_adc();
  //_state.set_tdc();

  _state.process();

#if USE_IS445_MAY08_NTUPLE
  if (_fortntu_name)
    _state.output_event();
#endif

  raw_event->TSHORT.value = _state.event.time_t2;
  raw_event->TEBIS.value  = _state.event.time_ebis;
  
#endif

  
}

void is445_08_user_init()
{
  // CALIB_PARAM(DSSSD[0].B[ 9].E,ZERO_SUPPRESS_ITEM(DSSSD[0].B, 9));
  // CALIB_PARAM(DSSSD[0].B[10].E,ZERO_SUPPRESS_ITEM(DSSSD[0].B,10));
  /*
  CALIB_PARAM( DSSSD[0].B[ 9].E ,CALIB_SLOPE_OFFSET(  2.4 , 6.7 ));
  CALIB_PARAM( DSSSD[0].B[10].E ,CALIB_SLOPE_OFFSET(  2.2 , 6.8 ));
  */
#if USE_IS445_MAY08_UNPACK && USE_IS445_MAY08_NTUPLE
  if (_fortntu_name)
    _state.open_output(_fortntu_name);
#endif
}

void is445_08_user_exit()
{
#if USE_IS445_MAY08_UNPACK && USE_IS445_MAY08_NTUPLE
  if (_fortntu_name)
    _state.close_output();
#endif
}

void is445_08_usage_command_line_options()
{
  //      "  --option          Explanation.\n"
#if USE_IS445_MAY08_UNPACK && USE_IS445_MAY08_NTUPLE
  printf ("  --fortntu=FILE    Generate fortran code style ntuple.\n");
#endif
}

bool is445_08_handle_command_line_option(const char *arg)
{
#if USE_IS445_MAY08_UNPACK && USE_IS445_MAY08_NTUPLE
  const char *post;

#define MATCH_PREFIX(prefix,post) (strncmp(arg,prefix,strlen(prefix)) == 0 && *(post = arg + strlen(prefix)) != '\0')
#define MATCH_ARG(name) (strcmp(arg,name) == 0)
  
 if (MATCH_PREFIX("--fortntu=",post)) {
   _fortntu_name = post;
   return true;
 }
#endif

 return false;
}
