#ifndef __MULTI_CHUNK_FCN_HH__
#define __MULTI_CHUNK_FCN_HH__

template<typename T,typename T_map>
inline void multi_chunks<T,T_map>::assign_events(uint32 events)
{
  if (_alloc_events < events)
    {
      // We need to reallocate

      uint32 new_size = _alloc_events ? _alloc_events * 2 : 8;
      while (new_size < events)
	new_size *= 2;

      int *new_event_index = new int[new_size];

      if (!new_event_index)
	{
	  ERROR("Memory allocation failure, "
		"could not allocate %d event index entries for multi-event.",
		_alloc_items);
	}

      _alloc_events = new_size;

      delete[] _event_index;
      _event_index = new_event_index;
    }

  // First set the entire (used) array to empty data (implicit zero-suppression)

  for (unsigned int i = 0; i < events; i++)
    _event_index[i] = -1;

  // Then set the entries that we have

  for (unsigned int item = 0; item < _num_items; item++)
    {
      int event = _item_event[item];

      if (event < 0 || (unsigned int) event >= events)
	ERROR("Invalid event number (%d >= %d) for item (%d).",
	      event,events,item);
      
      if (_event_index[event] != -1)
	{
	  // One could think about implementing merge functions to fix this!
	  ERROR("Several module chunks of data for the same event.");
	}

      _event_index[event] = item;
    }
  /*
  for (int i = 0; i < events; i++)
    printf ("%d  ",_event_index[i]);
  printf ("\n");
  */
  _num_events = events;
}

template<typename T,typename T_map>
void map_multi_events(multi_chunks<T,T_map> &module,
		      uint32 counter_start,
		      uint32 events)
{
  // Loop over all events for this module, and set the indices
  // in the multi_chunk array to tell to what multi subevent each chunk
  // belongs

  for (unsigned int i = 0; i < module._num_items; i++)
    {
      T &item = module._items[i];

      //int local_counter = item. header.count;
      //int offset = (local_counter - counter_start) & 0x0000ffff;
      unsigned int offset = item.get_event_counter_offset(counter_start);

      if (offset >= events)
	ERROR("Local event counter (0x%08x) "
	      "outside allowed range [0x%08x,0x%08x)",
	      item.get_event_counter(),counter_start,counter_start+events);

      // event counter valid

      module._item_event[i] = offset;

      //printf ("%d:%d  ",i,offset);
    }
  //printf ("\n");

  module.assign_events(events);
}

template<typename T,typename T_map>
void map_continuous_multi_events(multi_chunks<T,T_map> &module,
				uint32 counter_start,
				uint32 events)
{
  if (module._num_items > events)
    ERROR("Cannot map - number of items (%d) "
	  "different from number of events (%d).",
	  module._num_items,events);
  
  for (unsigned int i = 0; i < module._num_items; i++)
    {
      T &item = module._items[i];

      if (!item.good_event_counter_offset(counter_start + i))
	ERROR("Local event counter (0x%08x) "
	      "gives bad offset (expect 0x%08x (masked)).",
	      item.get_event_counter(),counter_start + i);	

      module._item_event[i] = i;
    }

  module.assign_events(events);
}

#endif//__MULTI_CHUNK_FCN_HH__
