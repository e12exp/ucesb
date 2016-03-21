
#ifndef __SELECT_EVENT_HH__
#define __SELECT_EVENT_HH__

#include "typedef.hh"

#include "lmd_event_10_1.h"
#include "lmd_subevent_10_1.h"

#include <vector>

struct select_event_request_item
{
  int _offset;
  int _size;
  int _min;
  int _max;
};

class select_event_request
{
public:
  std::vector<select_event_request_item> _items;

public:
  int add_item(const char *cmd);

public:
  bool match(const void *ptr) const;
};

#define SELECT_FLAG_INCLUDE   0x0001
#define SELECT_FLAG_EXCLUDE   0x0002

struct select_event_requests
{
public:
  select_event_requests()
  {
    _flags = 0;
  }

public:
  int _flags;
  std::vector<select_event_request> _items;

public:
  bool has_selection();

public:
  void add_item(const select_event_request &item,bool incl,
		const char *command);

  bool match(const void *ptr) const;
  bool accept(const void *ptr) const;
};


// Class used to keep track of user request of which events, and or
// subevents that are wanted in an output stream (file or tcp)

// For the event itself, one may select on the trigger

// For the subevent, on may select on type, subtype , procid, subcrate, control

class lmd_event_out;

class select_event
{
public:
  select_event();

public:
  select_event_requests _event;
  select_event_requests _subevent;

  bool                  _omit_empty_payload;

public:
  bool has_selection();

public:
  void parse_request(const char *cmd,bool incl);



public:
  bool accept_event(const lmd_event_10_1_host *header) const;
  bool accept_subevent(const lmd_subevent_10_1_host *header) const;

  bool accept_final_event(lmd_event_out *event) const;


};

#endif//__SELECT_EVENT_HH__
