
#include "structures.hh"

#include "user.hh"

#include <curses.h> // needed for the COLOR_ below

watcher_type_info mwpclab_watch_types[NUM_WATCH_TYPES] = 
  { 
    { COLOR_GREEN,   "Physics" }, 
    { COLOR_YELLOW,  "Tcal" }, 
    { COLOR_MAGENTA, "TcalClock" }, 
    { COLOR_RED,     "Scaler" }, 
  };

// Number of seconds from 1900 to 1970, 17 leap years...
#define SECONDS_1970 ((365*70+17)*24*3600ul)

void mwpclab_watcher_event_info(watcher_event_info *info,
				unpack_event *event)
{
  info->_type = MWPCLAB_WATCH_TYPE_PHYSICS;

  if (event->ev.trig.value == 1)
    info->_type = MWPCLAB_WATCH_TYPE_PHYSICS;
  else if (event->ev.trig.value == 2)
    info->_type = MWPCLAB_WATCH_TYPE_TCAL;
  else if (event->ev.trig.value == 4)
    {
      info->_type = MWPCLAB_WATCH_TYPE_TCAL_CLOCK;

      if (event->ev.clock.before_s)
	{
	  info->_info |= WATCHER_DISPLAY_INFO_TIME;

	  // With flags & 1, we're using the NTP epoch...

	  if (event->ev.clock.flags & 1)
	    info->_time = (int) (event->ev.clock.before_s - SECONDS_1970);
	  else
	    info->_time = event->ev.clock.before_s;
	}
    }
  else if (event->ev.trig.value == 8)
    {
      info->_type = MWPCLAB_WATCH_TYPE_SCALER;
      info->_display |= WATCHER_DISPLAY_SPILL_EOS;
    }
    


}

void user_function(unpack_event *event,
		   raw_event    *raw_event,
		   cal_event    *cal_event)
{

}
