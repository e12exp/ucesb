
#ifndef __WATCHER_CHANNEL_HH__
#define __WATCHER_CHANNEL_HH__

//#include "detector_requests.hh"

#include <curses.h>
#include <unistd.h>
#include <string.h>

#include <vector>
#include <string>

#include "watcher_event_info.hh"

//#define WATCH_TYPE_PHYSICS   0
//#define WATCH_TYPE_OFFSPILL  1
//#define WATCH_TYPE_TCAL      2
//#define WATCH_TYPE_OTHER     3

//#define NUM_WATCH_TYPES      4
////#define NUM_WATCHCOINC_TYPES 2 // physics and offspill must be first

#define NUM_WATCH_BINS              40
#define NUM_WATCH_STAT_RANGE_BINS   10

#define NUM_WATCH_PRESENT_CHANNELS  32


struct watcher_display_info
{
  //detector_requests* _requests;

  WINDOW* _w;
  int     _line;
  uint    _counts;

  int     _max_line;
  int     _max_width;

  short   _col_norm;
  short   _col_data[NUM_WATCH_TYPES];

  int     _show_range_stat;
};

class watcher_channel_display
{

public:
  virtual ~watcher_channel_display() { }

public:
  virtual void clear_data() = 0;

public:
  virtual void display(watcher_display_info& info/*,
	       const signal_id& id,
	       const char* te*/) = 0;
};

typedef std::vector<watcher_channel_display*> vect_watcher_channel_display;

struct watch_range
{
public:
  uint _zero;
  uint _bins[2][NUM_WATCH_BINS]; // +1 to avoid trouble from (4096/NUM_WATCH_BINS)
  uint _overflow;
};

#define WATCH_STAT_RANGE_HISTORY 32 // must be power of 2

struct watch_stat_range_bin
{
  int   _start;
  int   _num;
  float _history[WATCH_STAT_RANGE_HISTORY];

public:
  void forget();
};

struct watch_stat_range
{
  watch_stat_range_bin _bins[2][NUM_WATCH_STAT_RANGE_BINS];
};

class watcher_channel
  : public watcher_channel_display
{
public:
  virtual ~watcher_channel() { }

public:
  watcher_channel()
  {
    clear();
    memset(_range_hit,0,sizeof(_range_hit));
    _rescale = 0;
    _min = 0;
    _max = 4096;
  }
  
  void clear()
  {
    clear_data();
    memset(&_stat_range,0,sizeof(_stat_range));
  }
  
  virtual void clear_data()
  {
    memset(_data,0,sizeof(_data));
    for (int r = 0; r < 2; r++)
      for (int b = 0; b < NUM_WATCH_STAT_RANGE_BINS; b++)
	_stat_range._bins[r][b].forget();
  }

public:
  watch_range _data[NUM_WATCH_TYPES];
  watch_stat_range _stat_range;

public:
  // counters 0 when unused, when used start over at 10, so that
  // we do not switch low-auto-high range too often (just due to a few
  // counts

  char _range_hit[2];  

  std::string _name;

public:
  uint _rescale;
  uint _min;
  uint _max;

public:
  void set_rescale_min(uint min) { _min = min; _rescale = _max - _min; }
  void set_rescale_max(uint max) { _max = max; _rescale = _max - _min; }

public:
  void collect_raw(uint raw,uint type,watcher_event_info *watch_info);

public:
  virtual void display(watcher_display_info& info/*,
	       const signal_id& id,
	       const char* te*/);

};

class watcher_present_channels;

class watcher_present_channel
{
public:
  watcher_present_channel()
  {
    clear();
    _min = 0;
    _max = 4096;
  }

  virtual ~watcher_present_channel() { }
  
  void clear()
  {
    clear_data();
  }
  
  virtual void clear_data()
  {
    memset(_data,0,sizeof(_data));
  }

public:
  watcher_present_channels *_container;

public:
  uint _data[NUM_WATCH_TYPES];

public:
  uint _min;
  uint _max;

public:
  void set_cut_min(uint min) { _min = min; }
  void set_cut_max(uint max) { _max = max; }

public:
  void collect_raw(uint raw,uint type,watcher_event_info *watch_info);

public:
  void display(watcher_display_info& info);

};

class watcher_present_channels
  : public watcher_channel_display
{
public:
  virtual ~watcher_present_channels() { }

public:
  watcher_present_channels();

public:
  void clear()
  {
    clear_data();
  }
  
  virtual void clear_data();

  void event_summary();

public:
  watcher_present_channel *_channels[NUM_WATCH_PRESENT_CHANNELS];
  
public:
  bool _has[NUM_WATCH_TYPES];
  uint _data[NUM_WATCH_TYPES];

public:
  std::string _name;

public:
  virtual void display(watcher_display_info& info/*,
	       const signal_id& id,
	       const char* te*/);

};

typedef std::vector<watcher_present_channels*> vect_watcher_present_channels;

#endif//__WATCHER_CHANNEL_HH__
