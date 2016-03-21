
#ifndef __TSTAMP_ALIGNMENT_HH__
#define __TSTAMP_ALIGNMENT_HH__

#include "lmd_input.hh"

#include <stdint.h>

#include <map>
#include <vector>

#ifdef USE_LMD_INPUT
struct titris_subevent
{
    uint                   _titris_id;
    lmd_subevent_10_1_host _subev;
};

union titris_subevent_ident
{
  uint                   _compare[4]; // second is dummy (_info._header.l_dlen, always 0)
  titris_subevent        _info;

public:
  bool operator<(const titris_subevent_ident &rhs) const
  {
    for (size_t i = 0; i < 1; i++)
      {
	if (_compare[i] == rhs._compare[i])
	  continue;
	return _compare[i] < rhs._compare[i];
      }
    for (size_t i = 2; i < countof(_compare); i++)
      {
	if (_compare[i] == rhs._compare[i])
	  continue;
	return _compare[i] < rhs._compare[i];
      }
    return false;
  }
};

typedef std::map<titris_subevent_ident,size_t> set_titris_subevent_index;

struct ts_a_hist
{
public:
  ts_a_hist();

public:
  uint     _bins[2][32]; // [0] = passed, [1] = future

  void add(int i,uint64_t val);
  void clear();
};

typedef std::vector<ts_a_hist> vect_ts_a_hist;

struct tstamp_alignment_histo
{
public:
  uint64_t _prev_stamp;

  vect_ts_a_hist _hists;

public:
  void init_clear();
  void add_histos(size_t n);
};

typedef std::vector<tstamp_alignment_histo *> vect_tstamp_alignment_histo;

#endif

#ifdef USE_LMD_INPUT
class tstamp_alignment
{

public:
  set_titris_subevent_index    _map_index;
  vect_tstamp_alignment_histo  _vect_histo;

public:
  size_t get_index(const lmd_subevent *subevent_info,
		   uint titris_branch_id);
  void account(size_t index, uint64_t stamp);

public:
  void init();
  void show();

};
#endif


#endif/*__TSTAMP_ALIGNMENT_HH__*/
