#ifndef _LMD_SOURCE_MULTIEVENT_H_
#define _LMD_SOURCE_MULTIEVENT_H_

#include <list>
#include <map>
#include <boost/circular_buffer.hpp>
#include <vector>
#include <deque>
#include <stdint.h>

struct lmd_source_multievent;
struct lmd_event_multievent;

#include "config.hh"
#include "lmd_input.hh"
#include "thread_buffer.hh"

#include <math.h>

#define _ENABLE_TRACE 0
#if _ENABLE_TRACE
#define _TRACE(...) fprintf(stderr, __VA_ARGS__)
#else
#define _TRACE(...)
#endif

struct multievent_entry
{
  lmd_subevent_10_1_host _header; 
  uint64_t timestamp;
  uint64_t wrts;
  uint8_t channel_id;
  uint8_t module_id;
  uint8_t sfp_id;
  uint32_t proc_id;
  uint32_t *data;
  uint32_t size;	
	multievent_entry() : data(NULL) {}
	~multievent_entry()
	{
	       _TRACE("multievent_entry::dtor()\n");

               if(data != NULL)
               {
                 free(data);
               }
	}

        static bool compare(const multievent_entry *e1, const multievent_entry *e2);

};

typedef std::deque< multievent_entry* > multievent_queue;

class lmd_source_multievent : public lmd_source
{
protected:
  enum file_status_t { ready, eof, unknown_event };
  
  uint64_t febex_ts_current[4]{}; // at readout
  uint64_t wr_ts_current{0};
  uint64_t febex_ts_last[4]{};    // at readout
  uint64_t wr_ts_last{0};
  double ts_conv_slope[4]{50./3,50./3,50./3,50./3}; 
  file_status_t input_status;

  // Timestamp skew per processor
  std::map<uint32_t, int64_t> proc_ts_skew;

  lmd_event_hint event_hint;
  lmd_event_10_1_host input_event_header;
  sint32 l_count;

  // we have 4*16*16=1024 channels, each of which could send us up to 200 hits
  // plus some overhead (e.g *2) for overlapping readouts.
  // this works out at 4 MB
  #define CIRC_BUF_SIZE size_t(4*16*16*256*2)
  //#define CIRC_BUF_SIZE size_t(4*16*2)
  boost::circular_buffer<multievent_entry* > events_available;
  std::vector<multievent_entry* > events_curevent;

  file_status_t load_events();
  
  multievent_entry* next_singleevent();
  
public:
  uint64_t febex2wrts(uint64_t fbxts, uint8_t sfp) //TODO
  {
    assert(sfp<4);
    static bool warned=0;
    if (!warned++ && !wr_ts_current)
      fprintf(stderr, "febex2wrts: WRTS is zero. This should never happen.\n");

    return (int64_t)wr_ts_current+(int64_t)(double((int64_t)fbxts-(int64_t)febex_ts_current[sfp])
            *ts_conv_slope[sfp]);
  }


  void update_ts_conv(uint64_t wrts, uint64_t fbxts, uint8_t sfp)
  {
    _TRACE("update_ts_conv(wrts=%ld, fbxts=%ld, sfp=%d)\n", wrts, fbxts, (int)sfp);
    assert(sfp<4);
    febex_ts_last[sfp]=febex_ts_current[sfp];
    if (wrts && wrts!=wr_ts_current)
    {    
       wr_ts_last=wr_ts_current;
       wr_ts_current=wrts;
    }
    febex_ts_current[sfp]=fbxts;

    static uint16_t warned=0;
    if (!wrts)
      {
	wr_ts_current=50*fbxts/3;
	if (!warned++)
	  fprintf(stdout,
		  "**************************************************\n"
		  "update_ts_conv: Either epoch is currently a multiple of 2^64 ns\n"
		  " (happy anniversary)\n"
		  " or your febex data did not include WRTS data (e.g. from PEXARIA). \n"
		  "I will fake a WRTS like timestamp based on the febex ts.\n"
		  "THIS WILL BE UNMERGEABLE WITH ANYTHING ELSE.\n"
		  "*************************************************\n\n");
      }
    if (!wr_ts_last || int64_t(febex_ts_current[sfp]) - int64_t(febex_ts_last[sfp]) <= 0) 
      {
	// one point interpolation using the standard febex ts rate of 50/3 ns
        ts_conv_slope[sfp]=50./3;
      }
    else
      {
	double delta_febex =  double(febex_ts_current[sfp] - febex_ts_last[sfp]);
	double delta_wrts  =  double(wr_ts_current - wr_ts_last);
	assert(febex_ts_current[sfp] > febex_ts_last[sfp]);
	assert(wr_ts_current > wr_ts_last);
        ts_conv_slope[sfp]=delta_wrts/delta_febex;
      }
  }
  
  lmd_source_multievent(); 

  virtual lmd_event *get_event();
};

struct wrts_header
{
  uint32_t system_id;
  uint32_t lower16;
  uint32_t midlower16;
  uint32_t midupper16;
  uint32_t upper16;
public:
  wrts_header(uint64_t ts):
    system_id(_conf._enable_eventbuilder),
    lower16(   0x03e10000 | (0xffff & (uint32_t)(ts    ))),
    midlower16(0x04e10000 | (0xffff & (uint32_t)(ts>>16))),
    midupper16(0x05e10000 | (0xffff & (uint32_t)(ts>>32))),
    upper16(   0x06e10000 | (0xffff & (uint32_t)(ts>>48)))
  {}
};

#endif

