
#ifndef __INPUT_EVENT_HH__
#define __INPUT_EVENT_HH__

#include "thread_buffer.hh"

struct input_event
{

#ifndef USE_THREADING
public:
  keep_buffer_single _defrag_event;

public:
  void release()
  {
    // _defrag_event.release();
  }
#endif

};

#endif//__INPUT_EVENT_HH__
