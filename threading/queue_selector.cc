
#include "queue_selector.hh"


int queue_selector::next_queue()
{
  // until things work: simple round-robin

  _current++;

  return _current;

#if 0
  if (likely(!_drained))
    {
      _current_left--;
      
      if (likely(_current_left))
	return _current;
    }

  // Go over the queues and find out who is in most need of more events.
  // If we find anyone being empty, we'll enque events into that one, without
  // further looking into the other ones

  for (int i = 0; i < _queues; i++)
    {
      // Find out the status of the queue, how many events are in the queue

      int items = queue[i]->fill();

      if (unlikely(!items))
	{
	  _bunches >>= 3; // send fewer items per queue (restart the round robin)
  
	  _current_left = _bunches;

	  // but do not try to emit more elements than the queue has free slots!
	  // (avoid unnecessary stops)

	  int free_slots = queue[i]->slots() - items - 1 /* the one given now*/;

	  _current_left = (_bunches < free_slots) ? _bunches ? free_slots;
	  return _current = i;
	}

      // Compare the number of elements in the queue to the desired
      // fill factor, i.e. a queue that we do not want to give so much
      // work (because it also has other tasks)

      // Now for the 'queue' that fills into our own thread, there
      // will never be any entries, as we would process the events
      // directly, and send them along to the next stage...

      int factor = items / fraction[i];

      if (factor > min_factor)
	continue;

      min_factor = factor;
      
      // Now that


    }
#endif

}
