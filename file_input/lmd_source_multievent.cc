#include <algorithm>

#include "lmd_source_multievent.hh"
#include "lmd_input.hh"
#include "config.hh"

#define SUBEVT_TYPE 100
#define SUBEVT_SUBTYPE 10000
#define SUBEVT_PROCID 2

#define DT (1500/17)

lmd_event *lmd_source_multievent::get_event()
{
   _TRACE("lmd_source_multievent::get_event()\n");
 
   if(!_conf._enable_eventbuilder)
      return lmd_source::get_event();

        uint64_t first_ts;
	uint32_t total_size = 0;

        multievent_entry *evnt;
        
        if(events_curevent.empty())
        {
            // No left overs from last buffer
            evnt = next_singleevent();
        }
        else
        {
            // Left overs from last buffer available
            _TRACE("  %d events left over\n", events_curevent.size());
            evnt = events_curevent.front();
            for(unsigned int i = 1; i < events_curevent.size(); i++)
              total_size += events_curevent[i]->size;
            _TRACE("  -> Total size: %d\n", total_size);
        }

        if(!evnt)
        {
           _TRACE("return NULL (evnt = NULL)\n");
           return NULL;
        }

	first_ts = evnt->timestamp;

        do
	{
                if(evnt == events_available.front())
                {
                  events_available.pop_front();
		  events_curevent.push_back(evnt);
                }
		total_size += evnt->size;
                _TRACE(" Total size: %d\n", total_size);
		evnt = next_singleevent();
	}
	while(evnt != NULL && evnt->timestamp >= first_ts && evnt->timestamp - first_ts <= DT);

//        if(evnt)
//          events_available.push_front(evnt);

        // Something went wrong while loading data
        // -> Break and return input event
        // BUT: If EOF is reached, return last event buffer
        if(!evnt && input_status != eof)
        {
           _TRACE("=> return &_file_event (input event)\n");
           return &_file_event;
        }
        else if(!evnt && input_status == eof && events_curevent.empty())
        {
           _TRACE("=> return NULL (eof)\n");
           return NULL;
        }

        _TRACE(" Allocating %d bytes\n", total_size);
        _file_event.release();
	_file_event._header = input_event_header;
	_file_event._header._info.l_count = ++l_count;
	_file_event._subevents = (lmd_subevent *)
               _file_event._defrag_event.allocate(sizeof (lmd_subevent));
	_file_event._nsubevents = 1;
	_file_event._subevents[0]._header = events_curevent[0]->_header;
	_file_event._subevents[0]._data =
		(char*)_file_event._defrag_event_many.allocate(total_size);
	_file_event._subevents[0]._header._header.l_dlen = total_size/2 + 2;

        _TRACE(" -> &_subevents = %p\n", _file_event._subevents);

	total_size = 0;
        _TRACE(" Collecting events\n");
	for(multievent_queue::iterator it = events_curevent.begin(); it != events_curevent.end(); it++)
	{
		evnt = *it;
		memcpy(_file_event._subevents[0]._data + total_size, evnt->data, evnt->size);
		total_size += evnt->size;
                _TRACE("  Total size = %d\n", total_size);

                // Does not really delete the event, but unref it in the keep_buffer_many
                delete evnt;
	}
        events_curevent.clear();
	
	_file_event._status = LMD_EVENT_GET_10_1_INFO_ATTEMPT
		| LMD_EVENT_HAS_10_1_INFO
		| LMD_EVENT_LOCATE_SUBEVENTS_ATTEMPT;

        _TRACE("=> return multi event buffer\n");
        return &_file_event;
}

multievent_entry* lmd_source_multievent::next_singleevent()
{
	_TRACE("lmd_source_multievent::next_singleevent()\n");
	
        // No more events available? Load more!
        if(events_available.empty())
        {
		input_status = load_events();

               // Still no available? Either end of file or wrong event type => Break
               if(events_available.empty())
                  return NULL;
        }

        return events_available.front();
}

lmd_source_multievent::file_status_t lmd_source_multievent::load_events()
{
	multievent_entry *event_entry;
	lmd_subevent *se;
	char *pb_start, *pb_end;
	uint32_t *pl_start, *pl_end, *pl_data, *pl_bufstart, *pl_bufhead;
	uint32_t bufsize, channel;
        keep_buffer_wrapper *alloc;
	
	_TRACE("lmd_source_multievent::load_events()\n");

           _file_event.release();
	   // We need new input events
	   if(!lmd_source::get_event())
		   return eof;
	   
	   _file_event.get_10_1_info();
	   if(_file_event._header._header.i_type != 10 || _file_event._header._header.i_subtype != 1)
	   {
		   // printf("Discard:\n");
		   // _file_event.print_event(0, NULL);
                   _TRACE("=> return unknown_event\n");
		   return unknown_event;
	   }

	input_event_header = _file_event._header;
	_file_event.locate_subevents(&event_hint);
	
#if _ENABLE_TRACE
	   _file_event.print_event(0, NULL);
#endif
	
	if(_file_event._nsubevents == 0)
        {
                _TRACE("-> return unknown_events (_nsubevents = 0)\n");
		return unknown_event;
        }

        if(curbuf == data_alloc.end())
           curbuf = data_alloc.begin();
        alloc = *curbuf;

        // Check if current buffer is free or if it still contains some events in the queue
        // -> If so, we need one more buffer
        if(!alloc->available())
        {
           alloc = new keep_buffer_wrapper();
           curbuf = data_alloc.insert(curbuf, alloc);

           _TRACE("lmd_source_multievent::load_events(): Creating new buffer. Total count: %d\n", data_alloc.size());
        }

	for(int i = 0; i < _file_event._nsubevents; i++)
	{
		se = &(_file_event._subevents[i]);
		_TRACE("  Subevent: Type: %d, Subtype: %d, ProcID: %d\n", se->_header._header.i_type,
			se->_header._header.i_subtype, se->_header.i_procid);
		
		if(se->_header._header.i_type != SUBEVT_TYPE || se->_header._header.i_subtype != SUBEVT_SUBTYPE
				|| se->_header.i_procid != SUBEVT_PROCID)
			continue;
		
		_file_event.get_subevent_data_src(se, pb_start, pb_end);
		
		pl_start = (uint32_t*)pb_start;
		pl_end = (uint32_t*)pb_end;
		
		for(pl_data = pl_start; pl_data < pl_end; )
		{
			// Skip DMA alignment words
			while((*pl_data & 0xfff00000) == 0xadd00000)
				++pl_data;
			
			// GOSIP Header size
			if((*pl_data & 0xff) != 0x34)
                        {
                           // Oopsy
                           fprintf(stderr, "[WARNING] Invalid GOSIP submemory header: 0x%08x\n", *pl_data);
                           break;
                        }
			
			pl_bufhead = pl_data;
			channel = (*pl_data++ >> 16) & 0xffff;
			bufsize = *pl_data++;			
			
			// Skip FEBEX special channel
			if((channel & 0xff00) == 0xff00)
			{
				pl_data += bufsize/4;
				continue;
			}
			
			_TRACE(" + Channel %d\n", channel);
			
			// Read all events within current GOSIP buffer
			for(pl_bufstart = pl_data; pl_data < pl_bufstart + bufsize/4; )
			{
				_TRACE(" ++ Event\n");
				// Check event header
                                //                             old Febex FW                              Febex FW 1.2+                             Pulser
				if(!((*pl_data & 0xffff0000) == 0xAFFE0000  || (*pl_data & 0xffff0000) == 0x115A0000 || (*pl_data & 0xffff0000) == 0xB00B0000))
                                {
                                   // Oops... Something went wrong
                                   fprintf(stderr, "[WARNING] Invalid event header: 0x%08x\n", *pl_data);
                                   break;
                                }
			
                                event_entry = new (*alloc) multievent_entry(alloc);
				event_entry->_header = se->_header;
				event_entry->channel = channel;
				event_entry->timestamp = *(pl_data + 2) | ((uint64_t)(*(pl_data + 3)) << 32);
				
				event_entry->size = (*pl_data & 0xffff) + 8;	// Include GOSIP buffer header
				_TRACE("    Size: %d\n", event_entry->size);
				event_entry->data = (uint32_t*)alloc->allocate(event_entry->size);
				
				memcpy(event_entry->data, pl_bufhead, 8);
				memcpy(event_entry->data + 2, pl_data, event_entry->size - 8);
				
				// Adjust size of GOSIP buffer header
				*(event_entry->data + 1) = event_entry->size - 8;
				
				events_read.push_back(event_entry);
				pl_data += (event_entry->size - 8)/4;
			}
		}
	}

        if(events_read.empty())
           return unknown_event;

        curbuf++;

        // Sort current buffer by timestamp and add to available events
        sort(events_read.begin(), events_read.end(), multievent_entry::compare);
        events_available.insert(events_available.end(), events_read.begin(), events_read.end());
        events_read.clear();

	return ready;
}

bool multievent_entry::compare(const multievent_entry *e1, const multievent_entry *e2)
{
   return (e1->timestamp < e2->timestamp);
}

void* multievent_entry::operator new(size_t bytes, keep_buffer_wrapper &alloc)
{
   return alloc.allocate(bytes);
}

void multievent_entry::operator delete(void *ptr)
{
   _TRACE("delete %p\n", ptr);
   multievent_entry *e = static_cast<multievent_entry*>(ptr);
   e->data_alloc->release();
}

lmd_source_multievent::lmd_source_multievent() : l_count(0)
{
   // Create a double buffer for event reading
   // - will later be extended if needed

   data_alloc.push_back(new keep_buffer_wrapper());
   data_alloc.push_back(new keep_buffer_wrapper());

   curbuf = data_alloc.begin();
}

