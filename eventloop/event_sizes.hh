
#ifndef __EVENT_SIZES_HH__
#define __EVENT_SIZES_HH__

#include "lmd_input.hh"
#include "pax_input.hh"
#include "genf_input.hh"
#include "ebye_input.hh"

#include <stdint.h>

#include <map>

#ifdef USE_LMD_INPUT
struct event_size
{
  uint64_t _min;
  uint64_t _max;

  uint64_t _sum;
  uint64_t _sum_x;

  uint64_t _sum_header;
};

union subevent_ident
{
  uint                   _compare[3]; // first is dummy (_info._header.l_dlen, always 0)
  lmd_subevent_10_1_host _info;

public:
  bool operator<(const subevent_ident &rhs) const
  {
    for (size_t i = 1; i < countof(_compare); i++)
      {
	if (_compare[i] == rhs._compare[i])
	  continue;
	return _compare[i] < rhs._compare[i];
      }
    return false;
  }
};

typedef std::map<subevent_ident,event_size *> set_subevent_size;

class event_sizes
{




public:
  set_subevent_size _subev_size[17]; // 0-15, others               

  event_size ev_size[17];

public:
  void account(FILE_INPUT_EVENT *event);

public:
  void init();
  void show();


};
#endif

#ifndef USE_LMD_INPUT
class event_sizes
{
public:
  // void account(FILE_INPUT_EVENT *event);

public:
  void init() { }
  void show() { }
};
#endif

#endif//__EVENT_SIZES_HH__
