#ifndef _LMD_SOURCE_MULTIEVENT_H_
#define _LMD_SOURCE_MULTIEVENT_H_

#include <boost/circular_buffer.hpp>
#include <array>
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
  std::array<int64_t, 20*4> proc_ts_skew {{}};

  lmd_event_hint event_hint;
  lmd_event_10_1_host input_event_header;

  // we have 4*16*16=1024 channels, each of which could send us up to 200 hits
  // plus some overhead (e.g *2) for overlapping readouts.
  // this works out at 4 MB
  #define CIRC_BUF_SIZE size_t(4*16*16*256*2)
  //#define CIRC_BUF_SIZE size_t(4*16*2)
  boost::circular_buffer<multievent_entry* > events_available;

  file_status_t load_events();
  
  multievent_entry* next_singleevent();
  
  uint64_t febex2wrts(uint64_t fbxts, uint8_t sfp);

  void update_ts_conv(uint64_t wrts, uint64_t fbxts, uint8_t sfp);

public:
  lmd_source_multievent() : events_available(CIRC_BUF_SIZE)
  {
    // Create a double buffer for event reading
    // - will later be extended if needed
    
    assert(sizeof(wrts_header)==5*4);
    
    //init_ts_conv();
  }


  ~lmd_source_multievent()
  {
    for (auto&& e: events_available)
      delete e;
  }


  virtual lmd_event *get_event();
};

#endif

