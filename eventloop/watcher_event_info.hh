
#ifndef __WATCHER_EVENT_INFO_HH__
#define __WATCHER_EVENT_INFO_HH__

#include "control_include.hh"

#ifdef WATCHER_EVENT_INFO_INCLUDE
#include WATCHER_EVENT_INFO_INCLUDE
#endif

// If the experiment has several different trigger types to displayed
// with different colours in the watcher, it should define
// NUM_WATCH_TYPES itself with the actual number of triggers, numbered
// [0..NUM_WATCH_TYPES-1].  For each trigger, it should then set the 
// _type field of watcher_event_info appropriately.

// It should then also define WATCH_TYPE_NAMES itself to the name of
// an global array of watcher_type_info with NUM_WATCH_TYPES items,
// giving a descriptive name and what colour to use.

#ifndef NUM_WATCH_TYPES
#define NUM_WATCH_TYPES 1
#endif

#ifndef WATCH_TYPE_NAMES
#define WATCH_TYPE_NAMES dummy_watch_types
#endif

// These flags are used for the _display member to tell if the event
// is special.  Together with _display_at_mask of the class
// watcher_window they define when to display.

#define WATCHER_DISPLAY_SPILL_EOS 0x01
#define WATCHER_DISPLAY_SPILL_BOS 0x02
#define WATCHER_DISPLAY_SPILL_ON  0x04
#define WATCHER_DISPLAY_SPILL_OFF 0x08

// These are only for use with _display_at_mask, to tell that data
// should be displayed also if a certain caount has been reached, or a
// timeout expired (default on, at 10000 events and 15s respectively)

#define WATCHER_DISPLAY_COUNT     0x10
#define WATCHER_DISPLAY_TIMEOUT   0x20

// For the _info field, to tell what additional information is
// available

#define WATCHER_DISPLAY_INFO_TIME     0x01
#define WATCHER_DISPLAY_INFO_EVENT_NO 0x01
#define WATCHER_DISPLAY_INFO_RANGE    0x02

struct watcher_event_info
{
  int    _type;
  uint   _display;

  int    _info;
  uint   _time;      // if WATCHER_DISPLAY_INFO_TIME
  uint   _event_no;  // if WATCHER_DISPLAY_INFO_EVENT_NO
  double _range_loc; // if WATCHER_DISPLAY_INFO_RANGE
};

struct watcher_type_info
{
  short       _color; // ncurses colors (see watcher/watcher_window.cc)
  const char *_name;
};

#endif//__WATCHER_EVENT_INFO_HH__
