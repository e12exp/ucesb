
#include "structures.hh"

#include "user.hh"

#include "land_triggers.hh"

#include <curses.h> // needed for the COLOR_ below

watcher_type_info land_watch_types[NUM_WATCH_TYPES] = 
  { 
    { COLOR_GREEN,   "Physics" }, 
    { COLOR_MAGENTA, "Offspill" }, 
    { COLOR_YELLOW,  "Laser" }, 
    { COLOR_RED,     "Other" }, 
  };

void land_watcher_event_info(watcher_event_info *info,
			     unpack_event *event)
{
  info->_type = LAND_WATCH_TYPE_OTHER;

  if (event->trigger == LAND_EVENT_PHYSICS)
    {
      if (event->camac.tpat & 0x0200)
	info->_type = LAND_WATCH_TYPE_LASER;
      else if (event->camac.tpat & 0x0800)
	info->_type = LAND_WATCH_TYPE_OTHER;
      else if (event->camac.tpat & 0x8000)
	info->_type = LAND_WATCH_TYPE_OFFSPILL;
      else
	info->_type = LAND_WATCH_TYPE_PHYSICS;
    }
#ifdef EXPSPEC_TRIGGER_OFFSPILL
  else if (event->trigger == LAND_EVENT_CLOCK)
    {
      info->_type = LAND_WATCH_TYPE_OTHER;
    }
#endif

  /* One can also override the _time and _event_no variables, altough
   * for LMD files formats this is filled out before (from the buffer
   * headers), so only needed if we do not like the information
   * contained in there.  Hmm _time from buffer not implemented yet...
   */

  // Since we have a time stamp (on some events), we'd rather use that
  // than the buffer timestamp, since the buffer timestamp is the time
  // of packaging, and not of event occuring
  
  // clean it for events not having it
  info->_info &= ~WATCHER_DISPLAY_INFO_TIME; 
  /*
  if (event->camac_scalers.timestamp)
    {
      info->_time = event->camac_scalers.timestamp;
      info->_info |= WATCHER_DISPLAY_INFO_TIME;
    }
  */  
}

bool land_correlation_event_info(unpack_event *event)
{
  if (event->trigger == LAND_EVENT_PHYSICS
#ifdef EXPSPEC_TRIGGER_OFFSPILL
      || event->trigger == LAND_EVENT_OFFSPILL
#endif
      )
    return true;

  return false;
}
