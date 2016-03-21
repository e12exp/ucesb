
#ifndef __QUEUE_SELECTOR_HH__
#define __QUEUE_SELECTOR_HH__

class queue_selector
{

public:
  int _current;      // The queue that we currenty want to issue events to
  int _current_left; // Events to issue before queue swap

  int _bunches;      // Number of events to throw into each queue

public:
  int next_queue();


};

#endif//__QUEUE_SELECTOR_HH__
