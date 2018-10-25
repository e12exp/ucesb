#include <algorithm>
#include <map>

#include "lmd_source_multievent.hh"
#include "lmd_input.hh"
#include "config.hh"

#define SUBEVT_TYPE 100
#define SUBEVT_SUBTYPE 10000
//#define SUBEVT_PROCID 2

// Time calibration trigger type
#define T_TCAL 1

#define TS_SKEW_WARN 1

#define DT (_conf._eventbuilder_ts)

//#define WRTS_SIZE 0
#define WRTS_SIZE (uint32_t)sizeof(wrts_header)
// someone thought it a good idea to use uint32 where xe ought to have used size_t.

lmd_event *lmd_source_multievent::get_event()
{
  /*  uint64_t ts=0;
  for (auto x: events_available)
    {
      if (ts!=0 && (x)->timestamp < ts)
	printf("sort does not work!\n");
      ts=(x)->timestamp;
      }*/

  
  _TRACE("lmd_source_multievent::get_event()\n");

  if(!_conf._enable_eventbuilder)
    return lmd_source::get_event();

  uint64_t first_ts;
  uint32_t total_size = 0;
  std::map<uint32_t, uint32_t> total_proc_size;
  std::map<uint32_t, multievent_entry*> proc_header;
  std::map<uint32_t, uint32_t> proc_idx_subevent;
  uint32_t idx_subevent;

  multievent_entry *evnt;

  
  if(events_curevent.empty())
  {
    // No left overs from last buffer
    evnt = next_singleevent();
  }
  else
  {
    /*    // Left overs from last buffer available
    _TRACE("  %ld events left over\n", events_curevent.size());
    printf("foooo\n");
    evnt = events_curevent.front();
    for(unsigned int i = 1; i < events_curevent.size(); i++)
    {
      total_size += events_curevent[i]->size;

      if(total_proc_size.count(events_curevent[i]->proc_id) == 0)
      {
        total_proc_size[events_curevent[i]->proc_id] = events_curevent[i]->size;
        proc_header[events_curevent[i]->proc_id] = events_curevent[i];
      }
      else
      {
        total_proc_size[events_curevent[i]->proc_id] += events_curevent[i]->size;
      }
    }
    _TRACE("  -> Total size: %d\n", total_size);
    */
    assert(0);
  }

  if(!evnt && input_status == eof)
  {
    _TRACE("return NULL (evnt = NULL, eof)\n");
    return NULL;
  }
  else if(!evnt)
  {
    _TRACE("return _file_event (evnt = NULL, unknown_event)\n");
    return &_file_event;
  }

  first_ts = evnt->timestamp;

  //printf("before %08x:%08x %08x:%08x\n", first_ts >> 32, 0xffffffff&first_ts, febex_ts_current >> 32, 0xffffffff&febex_ts_current);
  if (first_ts+100 > febex_ts_current)
  {
    load_events();
    assert(events_curevent.size()==0);

    //update first event
    evnt = next_singleevent();
    first_ts = evnt->timestamp;

    //memset(&_file_event, 0, sizeof(_file_event));
    //printf("after  %08x:%08x %08x:%08x\n", first_ts >> 32, 0xffffffff&first_ts, febex_ts_current >> 32, 0xffffffff&febex_ts_current);
    return  get_event(); // wait for the next readout before processing
  }
  
  do
  {
    if(evnt == events_available.front())
    {
      events_available.pop_front();
      events_curevent.push_back(evnt);
    }
    total_size += evnt->size;
    _TRACE(" Total size: %d\n", total_size);

    if(total_proc_size.count(evnt->proc_id) == 0)
    {
      total_proc_size[evnt->proc_id] = evnt->size;
      proc_header[evnt->proc_id] = evnt;
    }
    else
    {
      total_proc_size[evnt->proc_id] += evnt->size;
    }

    evnt = next_singleevent();
  }
  while(evnt != NULL && evnt->timestamp >= first_ts && evnt->timestamp - first_ts <= DT);
  // ^ -- do while loop.  event building happening in there. 
  // events_curevent is the container holding the coinciding events.
  

  //        if(evnt)
  //          events_available.push_front(evnt);

  if(!evnt && input_status != eof)
  {
      // Something went wrong while loading data
    // -> Break and return input event
    _TRACE("=> return &_file_event (input event)\n");
    fprintf("error, returned input event!\n");
    return &_file_event;
  }
  
  else if(!evnt && input_status == eof && events_curevent.empty())
  {
    // BUT: If EOF is reached, return last event buffer
    // ^---- does not look like you do this here --pklenze
    _TRACE("=> return NULL (eof)\n");
    return NULL;
  }

  _TRACE(" Allocating %d bytes\n", total_size);
  _file_event.release();
  _file_event._header = input_event_header;
  _file_event._header._info.l_count = ++l_count;

  // Create 1 subevent per processor
  _file_event._nsubevents = (int)total_proc_size.size();
  _file_event._subevents = (lmd_subevent *)
    _file_event._defrag_event.allocate(_file_event._nsubevents * sizeof (lmd_subevent));

  idx_subevent = 0;
  for(std::map<uint32_t, uint32_t>::iterator pit = total_proc_size.begin(); pit != total_proc_size.end(); pit++)
  {
    _file_event._subevents[idx_subevent]._header = proc_header[pit->first]->_header;
    _file_event._subevents[idx_subevent]._data =
      (char*)_file_event._defrag_event_many.allocate(pit->second+WRTS_SIZE);
    _file_event._subevents[idx_subevent]._header._header.l_dlen = (pit->second+WRTS_SIZE)/2 + 2; 

    // Map processor ID to subevent array index
    proc_idx_subevent[pit->first] = idx_subevent++;
    
    // Reset size to 0 for reuse as offset marker
    total_proc_size[pit->first] = 0;

  }
  //printf("total size=%d\n", total_size);

  _TRACE(" -> &_subevents = %p\n", _file_event._subevents);

  total_size = 0;
  _TRACE(" Collecting events\n");

  for(multievent_queue::iterator it = events_curevent.begin(); it != events_curevent.end(); it++)
  {
    evnt = *it;
    // Map hit to correct subevent for origin processor ID
    idx_subevent = proc_idx_subevent[evnt->proc_id];
    if (!total_proc_size[evnt->proc_id])
      {
	// add WRTS header for each proc
	wrts_header wr(febex2wrts(first_ts));
	static uint64_t whirr_prev = 0;
	static uint64_t first_ts_prev = 0;
	uint64_t whirr = febex2wrts(first_ts);

	if (whirr < whirr_prev)
	  {
	  printf("wr=%08x%08x..%08x%08x fbx=%08x%08x..%08x%08x, current=%08x%08x -> %08x%08x  prev=%08x%08x -> %08x%08x\n",
		 wr_ts_last >> 32, 0xffffffff & wr_ts_last,
		 wr_ts_current >> 32, 0xffffffff & wr_ts_current,
		 febex_ts_last >> 32, 0xffffffff & febex_ts_last,
		 febex_ts_current >> 32, 0xffffffff & febex_ts_current,
		 first_ts >> 32, 0xffffffff & first_ts,
		 whirr >> 32, 0xffffffff & whirr,
		 first_ts_prev >> 32, 0xffffffff & first_ts_prev,
		 whirr_prev >> 32, 0xffffffff & whirr_prev		 
		 );
	printf("   fbx: currentRO - first = %ld\n", (int64_t)febex_ts_current-first_ts);
	printf("   fbx: first -  previous = %ld\n", (int64_t)first_ts-first_ts_prev);
	printf("   fbx: first -  previous = %ld\n", (int64_t)first_ts-first_ts_prev);
	printf("    %f\n", (double)delta_wrts/delta_febex);
	  }
	whirr_prev = whirr;
	first_ts_prev=first_ts;

	memcpy(_file_event._subevents[idx_subevent]._data, &wr, sizeof(wr));
	total_proc_size[evnt->proc_id] = WRTS_SIZE;
      }
    memcpy(_file_event._subevents[idx_subevent]._data + total_proc_size[evnt->proc_id],
	   evnt->data, evnt->size);
    total_size += evnt->size;
    total_proc_size[evnt->proc_id] += evnt->size;
    _TRACE("  Total size = %d\n", total_size);
    _TRACE("  Proc size [%d] = %d\n", evnt->proc_id, total_proc_size[evnt->proc_id]);

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
  uint32_t bufsize, channel_header;
  uint8_t sfp_id, module_id, channel_id;
  uint32_t proc_id;
  int64_t ts_normalize, ts_skew, ts_header;
  keep_buffer_wrapper *alloc;

  static uint32_t buguser=0;

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

    _TRACE("lmd_source_multievent::load_events(): Creating new buffer. Total count: %ld\n", data_alloc.size());
  }

  ts_normalize = -1;
  buguser++;
  ts_skew = 0;
  for(int i = 0; i < _file_event._nsubevents; i++)
  {
    se = &(_file_event._subevents[i]);
    _TRACE("  Subevent: Type: %d, Subtype: %d, ProcID: %d\n", se->_header._header.i_type,
        se->_header._header.i_subtype, se->_header.i_procid);

    if(se->_header._header.i_type != SUBEVT_TYPE || se->_header._header.i_subtype != SUBEVT_SUBTYPE
        /*|| se->_header.i_procid != SUBEVT_PROCID*/)
      continue;

    proc_id = se->_header.i_procid;
    _file_event.get_subevent_data_src(se, pb_start, pb_end);

    pl_start = (uint32_t*)pb_start;
    pl_end = (uint32_t*)pb_end;
    uint64_t wr_latest=0;

    pl_data=pl_start;
    // get WR timestamp
    if (pl_data < pl_end && *pl_data == 0x400) // 0x400: febex system id
      {
	if (pl_data+5 > pl_end)
	  {
	    fprintf(stderr, "White Rabbit: Short frame (0x%p to 0x%p, size %ld).\n",
		    pl_data, pl_end, pl_data-pl_end);
	    return unknown_event;
	  }
	++pl_data;
	if ( (*pl_data & 0xffff0000)!=0x03e10000)
	  {
	    fprintf(stderr, "White Rabbit: Bad magic number. (0x%x instead of 0x%x)\n",
		    *pl_data & 0xffff0000, 0x03e10000);
	    return unknown_event;

	  }
	wr_latest = *pl_data++ & 0xffff;
	wr_latest |= (*pl_data++ & 0xffff) << 16;
	wr_latest |= (uint64_t)(*pl_data++ & 0xffff) << 32;
	wr_latest |= (uint64_t)(*pl_data++ & 0xffff) << 48;
	pl_start=pl_data;
      }
    
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
      channel_header = *pl_data++;
      bufsize = *pl_data++;			

      channel_id = (uint8_t)((channel_header >> 24) & 0xff);
      module_id = (uint8_t)((channel_header >> 16) & 0xff);
      sfp_id = (uint8_t)((channel_header >> 12) & 0xf);

      // Get readout timestamp from special channel and correct possible TS skew between modules/crates/processors
      if(channel_id == 0xff)
      {
        if(bufsize != 8)
        {
          fprintf(stderr, "[ERROR] Special channel 0xff has invalid data size: %d (Processor %d, SFP %d, module %d)\n",
		  bufsize, proc_id, sfp_id, module_id);
          pl_data += bufsize/4;
        }

        if(_file_event._header._info.i_trigger == T_TCAL)
        {
          ts_header = (int64_t)((*pl_data++) & 0x00ffffff) << 32;
          ts_header |= (int64_t)*(pl_data++);

          if(ts_normalize < 0)
          {
            ts_normalize = ts_header;
            ts_skew = 0;
            _TRACE("Current base timestamp: 0x%08lx\n", ts_normalize);
	    if (wr_latest)
	      {
		update_ts_conv(wr_latest, ts_normalize);
	      }
          }
          else
          {
            ts_skew = ts_header - ts_normalize;

            if((ts_skew < -TS_SKEW_WARN || ts_skew > TS_SKEW_WARN) && ! (buguser%0x40))
            {
              fprintf(stderr, "[WARNING] Timestamp skew for processor %d, SFP %d, module %d: %ld (Trigger %d)\n",
		      proc_id, sfp_id,
		      module_id, ts_skew,
		      _file_event._header._info.i_trigger);
            }
          } 
          proc_ts_skew[256 * proc_id + module_id] = ts_skew;
	  _TRACE("processor %d, SFP %d, module %d (trigger %d, skew: %ld)\n\n",
		 proc_id, sfp_id, module_id, _file_event._header._info.i_trigger, 
		 proc_ts_skew[256*proc_id + module_id] );

        } //T_TCAL
        else
        {
          pl_data += bufsize/4;
          if(proc_ts_skew.count(256*proc_id + module_id) == 1)
            ts_skew = proc_ts_skew[256 * proc_id + module_id];
          else
            ts_skew = 0;
        }

        continue;
      } // end of special channel

      // _TRACE(" + Channel %d\n", channel);

      // Read all events within current GOSIP buffer
      for(pl_bufstart = pl_data; pl_data < pl_bufstart + bufsize/4; )
      {
        _TRACE(" ++ Event\n");
        // Check event header
        if(!(   (*pl_data & 0xffff0000) == 0xAFFE0000     // old Febex FW  
		|| (*pl_data & 0xffff0000) == 0x115A0000  // Febex FW 1.2+
		|| (*pl_data & 0xffff0000) == 0xB00B0000)) // Pulser
	  {
          // Oops... Something went wrong
	    fprintf(stderr, "[WARNING] Invalid event header: 0x%08x\n", *pl_data);
	    break;
	  }

        event_entry = new (*alloc) multievent_entry(alloc);
        event_entry->_header = se->_header;
        event_entry->channel_id = channel_id;
        event_entry->module_id = module_id;
        event_entry->sfp_id = sfp_id;
        event_entry->proc_id = proc_id;
        event_entry->timestamp = (*(pl_data + 2) | ((uint64_t)(*(pl_data + 3)) << 32)) -  ts_skew;
	uint64_t fbxts=event_entry->timestamp;
	if ((int64_t)fbxts<(int64_t)febex_ts_last-50)
	  fprintf(stderr, "found a febex ts %ld fbx ticks in the previous readout slice!\n", febex_ts_last-fbxts);
    
	if ((int64_t)fbxts>(int64_t)febex_ts_current+100)
	  fprintf(stderr, "found a febex ts %ld fbx ticks in the next readout slice!\n", -febex_ts_current+fbxts);

	
        _TRACE("skew: %ld", ts_skew);
	//       fprintf(stdout, "FOOOFOOO Timestamp skew for processor %d, SFP %d, module %d: %ld (Trigger %d)\n", proc_id, sfp_id, module_id, ts_skew, _file_event._header._info.i_trigger);

	event_entry->size = (*pl_data & 0xffff) + 8;	// Include GOSIP buffer header
        _TRACE("    Size: %d\n", event_entry->size);
        event_entry->data = (uint32_t*)alloc->allocate(event_entry->size);

        memcpy(event_entry->data, pl_bufhead, 8);
        memcpy(event_entry->data + 2, pl_data, event_entry->size - 8);
#define UPDATE_TS 1
#if UPDATE_TS
	   event_entry->data[5]=(uint32_t)((event_entry->timestamp)>>32);
	   event_entry->data[4]=(uint32_t)(event_entry->timestamp);
#endif
  
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
  //sort(events_read.begin(), events_read.end(), multievent_entry::compare);
  events_available.insert(events_available.end(), events_read.begin(), events_read.end());
  sort(events_available.begin(), events_available.end(), multievent_entry::compare);  
  // ^-- sort everything, in case an event made it in one readout for one febex but not in another
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
  assert(sizeof(wrts_header)==5*4);

  init_ts_conv();
}

