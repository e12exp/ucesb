
#ifndef __EVENT_PROCESSOR_HH__
#define __EVENT_PROCESSOR_HH__

#include "worker_thread.hh"
#include "data_queues.hh"
#include "event_loop.hh"

class event_processor :
  public worker_thread
{
public:
  virtual ~event_processor();




public:
  processor_thread_data_queues _queues;

public:
#if defined(USE_LMD_INPUT) || defined(USE_HLD_INPUT) || defined(USE_RIDF_INPUT)
  ucesb_event_loop::source_event_hint_t _hints;
#endif

public:
  virtual void *worker();

public:
  void wait_for_output_queue_slot();
  void wait_for_input_queue_item();

public:
  void init(int index);

};

#endif//__EVENT_PROCESSOR_HH__
