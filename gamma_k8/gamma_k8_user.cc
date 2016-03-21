
#include "structures.hh"

#include "user.hh"

#include "multi_chunk_fcn.hh"

/*
uint32 AD413A_4CH::get_event_counter() const
{
  return 0;
}

uint32 AD413A_4CH::get_event_counter_offset(uint32 start) const
{
  return 0;
}
*/

int gamma_k8_user_function_multi(unpack_event *event)
{
#define MODULE event->ad413a.multi_adc

  for (unsigned int i = 0; i < MODULE._num_items; i++)
    {
      MODULE._item_event[i] = i;
    }

  MODULE.assign_events(MODULE._num_items);

  return MODULE._num_items;
}
