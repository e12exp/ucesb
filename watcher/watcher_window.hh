
#ifndef __WATCHER_WINDOW_HH__
#define __WATCHER_WINDOW_HH__

#include "watcher_event_info.hh"
#include "watcher_channel.hh"

#include <curses.h>
#include <time.h>

class watcher_window
{
public:
  watcher_window();

protected:
  WINDOW* mw;
  WINDOW* wtop;
  WINDOW* wscroll;

public:
  uint _time;
  uint _event_no;

  uint _counter;
  uint _type_count[NUM_WATCH_TYPES];

  int  _show_range_stat;

public:
  time_t _last_update;
  uint   _last_event_spill_on_off;

public:
  // detector_requests _requests;

  uint _display_at_mask;
  uint _display_counts; 
  uint _display_timeout;

public:
  vect_watcher_channel_display  _display_channels;
  vect_watcher_present_channels _present_channels;

public:
  void init();
  void event(watcher_event_info &info);

};

#endif//__WATCHER_WINDOW_HH__
