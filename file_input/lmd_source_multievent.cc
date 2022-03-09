#include <algorithm>
#include <map>

#include "lmd_source_multievent.hh"
#include "lmd_input.hh"
#include <inttypes.h>

#include <time.h>

#define SUBEVT_TYPE 100
#define SUBEVT_SUBTYPE 10000
//#define SUBEVT_PROCID 2


#define TS_SKEW_RATE_WARN 1e-6
#define DEBUGDRIFT 0


//#define TS_SKEW_RATE_WARN 0 //1e-16

#define DT (_conf._eventbuilder_ts)

//#define WRTS_SIZE 0
#define WRTS_SIZE (uint32_t)sizeof(wrts_header)
// someone thought it a good idea to use uint32 where xe ought to have used size_t.

#define BUGUSER 0x10 // debug/soft error suppresion factor

uint64_t lmd_source_multievent::febex2wrts(uint64_t fbxts, uint8_t sfp) //TODO
{
  assert(sfp<4);
  static bool warned=0;
  if (!warned++ && !wr_ts_current)
    fprintf(stderr, "febex2wrts: WRTS is zero. This should never happen.\n");

  return (int64_t)wr_ts_current+(int64_t)(double((int64_t)fbxts-(int64_t)febex_ts_current[sfp])
                                          *ts_conv_slope[sfp]);
}

void lmd_source_multievent::update_ts_conv(uint64_t wrts, uint64_t fbxts, uint8_t sfp)
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



lmd_event *lmd_source_multievent::get_event()
{
  
  _TRACE("lmd_source_multievent::get_event()\n");

  if(!_conf._enable_eventbuilder)
    return lmd_source::get_event();

  multievent_entry *evnt;

  evnt = next_singleevent();
 
  if(!evnt && input_status == eof)
  {
    _TRACE("return NULL (evnt = NULL, eof)\n");
    return NULL;
  }
  else if(!evnt)
  {
    _TRACE("confused: unknown event. Bailing.\n");
    exit(1);
  }

  uint64_t whirr = evnt->wrts;
  static uint64_t whirr_prev = 0;
  if (whirr_prev>whirr)
    fprintf(stderr, "WR non-monotonous, delta=%ld, current=0x%lx, last=0x%lx, %d.%d.%d\n",
            whirr-whirr_prev, whirr, whirr_prev,
            evnt->sfp_id, evnt->module_id, evnt->channel_id);
  assert (whirr_prev<=whirr);  
  whirr_prev = whirr;
  
  _file_event.release();
  _file_event._header = input_event_header;
  _file_event._header._info.l_count = 1;
  _file_event._nsubevents = 1;
  lmd_subevent& se = *(lmd_subevent *)
    _file_event._defrag_event.allocate(sizeof (lmd_subevent));
  se._header = evnt->_header;
  se._data   =  (char*)_file_event._defrag_event_many.allocate(WRTS_SIZE+evnt->size);
  wrts_header wr(whirr);
  memcpy(se._data, &wr, sizeof(wr));
  memcpy(se._data+sizeof(wr),
         evnt->data, evnt->size);
  se._header._header.l_dlen = (evnt->size+WRTS_SIZE)/2 + 2; 
  //printf("size=%d\n", se._header._header.l_dlen);
  _file_event._subevents=&se;
  
  delete evnt;
  
  _file_event._status = LMD_EVENT_GET_10_1_INFO_ATTEMPT
    | LMD_EVENT_HAS_10_1_INFO
    | LMD_EVENT_LOCATE_SUBEVENTS_ATTEMPT;
  // fprintf(stderr, "all done!");
  return &_file_event;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
multievent_entry* lmd_source_multievent::next_singleevent()
{
  _TRACE("lmd_source_multievent::next_singleevent()\n");

  
  // No more events available? Load more!
  while( events_available.empty() || events_available.front()->wrts+200*50/3 > wr_ts_current)
  {
    input_status = load_events();
    if (input_status==eof)
      return nullptr;
  }
  
  auto res=events_available.front();
  events_available.pop_front();
  return res;
}




lmd_source_multievent::file_status_t lmd_source_multievent::load_events()  ///////////////////////////////
{
  multievent_entry *event_entry;
  lmd_subevent *se;
  char *pb_start, *pb_end;
  uint32_t *pl_start, *pl_end, *pl_bufstart, *pl_bufhead;
  uint32_t proc_id;
  int64_t ts_normalize[4]{-1,-1,-1,-1};

  static uint16_t badmodules[4][20]; //sfp, module
  memset(badmodules, 0, sizeof(badmodules));

  static uint16_t goodmodules[4][20]; //sfp, module
  memset(goodmodules, 0, sizeof(goodmodules));

  static uint16_t bankswitch_issue[4][20]; //sfp, module
  memset(bankswitch_issue, 0, sizeof(bankswitch_issue));
  
  static uint32_t buguser=0;
  bool dropall=0;
  
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



  buguser++;
  auto sorted_up_to=events_available.end();
  
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

    uint32_t* pl_data=pl_start;
    // get WR timestamp
    if (pl_data < pl_end) // WR header.
      {
	if (*pl_data != _conf._enable_eventbuilder)
	  {
	    fprintf(stderr, "DAQ WR ID = 0x%x, event-builder expected 0x%x!\n",
		*pl_data, _conf._enable_eventbuilder);
	    return unknown_event;
	  }
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

    bool found_special_ch[4][20];

    memset(found_special_ch, 0, sizeof(found_special_ch));

    int last_sfp=-1;
    int last_mod=-1;
    int last_ch=0xfe;
    
    for(pl_data = pl_start; pl_data < pl_end; ) // note: it is terrible form to change a loop variable in the loop body --pk
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
      uint32_t channel_header = *pl_data++;
      uint32_t bufsize = *pl_data++;			
      int channel_id = (uint8_t)((channel_header >> 24) & 0xff);
      int module_id = (uint8_t)((channel_header >> 16) & 0xff);
      int sfp_id = (uint8_t)((channel_header >> 12) & 0xf);
      assert(sfp_id<4);
      // check for monotony in sfp, mod, channel
      {
	if (sfp_id!=last_sfp)
	  {
	    if (sfp_id < last_sfp)
	      {
		fprintf(stderr, "SFP ids of event are non-monotonic (last %d, now %d)\n", last_sfp, sfp_id);
	      }
	    last_mod=-1;
	  }
	if (module_id!=last_mod)
	  {
	    if (module_id!=(last_mod+1)%0x100 )
	      {
		fprintf(stderr, "Module ids of event are non-monotonic (@sfp %d: last %d, now %d)\n", sfp_id, last_mod, module_id);
		//		fprintf(stderr, "Expected %hhd, LHS: %d, RHS: %d \n", last_mod+(uint8_t)1, (int)module_id, last_mod+1) );
	      }
	    last_ch=0xfe;
	  }
	if (channel_id != (last_ch + 1)%0x100 )
	  {
	    //	    fprintf(stderr, "Unexpected channel id for %d.%02d.*: expected %d, got %d.\n", sfp_id, module_id, last_ch+1, channel_id);	  
	  }

	if (channel_id > 16 && channel_id!=0xff)
	  fprintf(stderr, "Unexpected channel id for %d.%d.%d\n",sfp_id, module_id, channel_id);
	last_sfp=sfp_id;
	last_mod=module_id;
	last_ch=channel_id;
      }

      int hit_no=0;
      int64_t ts_skew=0;

      uint64_t fbxts;
      
      if(channel_id == 0xff) // note: ch 255 comes first. 
      {
        _TRACE("Found_SC sfp=%d, mod=%d, tsn=%ld\n", sfp_id, module_id, ts_normalize[sfp_id]);
        if(bufsize != 8)
        {
          fprintf(stderr, "[ERROR] Special channel 0xff has invalid data size: %d (Processor %d, SFP %d, module %d)\n",
		  bufsize, proc_id, sfp_id, module_id);
          pl_data += bufsize/4;
        }

        fbxts=*(pl_data + 1)
          | ((uint64_t)(*(pl_data) & 0x00ffffff) << 32);
        
        if(ts_normalize[sfp_id] < 0) // module 0
          {
            ts_normalize[sfp_id] = fbxts;
            ts_skew = 0;
            _TRACE("Current base timestamp for sfp %d: 0x%08lx\n", (int)sfp_id, ts_normalize[sfp_id]);
	    update_ts_conv(wr_latest, ts_normalize[sfp_id], (uint8_t)sfp_id);
	    if (!ts_normalize[sfp_id] && !dropall)
	      {
		fprintf(stderr, "DAQ restart detected. dropping first readout.\n");
		dropall=1;
	      }

            // two neighboring sfps should have the same exploder, check if their clock count is compatible.

            int other_sfp=sfp_id^0x01;
            if (ts_normalize[other_sfp]>0 &&  ! (buguser%BUGUSER) && DEBUGDRIFT)
              {
                int64_t ts_skew2 =  ts_normalize[sfp_id]-ts_normalize[other_sfp];
                double skew_rate= double(ts_skew)/double(ts_normalize[sfp_id]);
               fprintf(stderr, "[WARNING] drift between %d.0.* and %d.0.*: drift: %3ld,  rate: %e\n", 
                       sfp_id, other_sfp, ts_skew2, skew_rate);
              }
          }
        else if (std::abs(fbxts-febex_ts_last[sfp_id])<1000) // module N, BS issue
	  {
	    bankswitch_issue[sfp_id][module_id]=1;
	    ts_skew=0xdeadbeefdeadbeef;
	  }
        else // module N
          {
            ts_skew = fbxts - ts_normalize[sfp_id];
            double skew_rate= double(ts_skew)/double(ts_normalize[sfp_id]);
            
            if( std::abs(skew_rate)>=0 && std::abs(skew_rate) >= TS_SKEW_RATE_WARN  && ! (buguser%BUGUSER))
              {
                fprintf(stderr, "[WARNING] Timestamp skew for %d.%d:, drift: %3ld,  rate: %e\n",
                        sfp_id,
                        module_id, ts_skew,
                        skew_rate
                        );
                //fprintf(stderr, "%d, %d, %d\n", std::abs(ts_skew)>10 , std::abs(skew_rate) > TS_SKEW_RATE_WARN  , ! (buguser%BUGUSER));
              }
	    if (labs(ts_skew-proc_ts_skew[20*sfp_id + module_id])>10000 && proc_ts_skew[20*sfp_id + module_id])
	      {
		fprintf(stderr, "[ERROR] Large change in timestamp skew for processor %d, SFP %d, module %d: %ld -> %ld (Trigger %d). Events ignored.\n",
			proc_id, sfp_id,
			module_id, proc_ts_skew[20*sfp_id + module_id], ts_skew,
			_file_event._header._info.i_trigger);
		bankswitch_issue[sfp_id][module_id]=2;
	      }
          } 
        proc_ts_skew[20*sfp_id + module_id] = ts_skew;
        found_special_ch[sfp_id][module_id]=1;
        _TRACE("processor %d, SFP %d, module %d (trigger %d, skew: %ld)\n\n",
               proc_id, sfp_id, module_id, _file_event._header._info.i_trigger, 
               proc_ts_skew[20*sfp_id + module_id] );

        //continue; // skip special channel in output
        //TODO: keep in stream
        // end of special channel
      }
      // _TRACE(" + Channel %d\n", channel);

      // Read all events within current GOSIP buffer
      for(pl_bufstart = pl_data; pl_data < pl_bufstart + bufsize/4; )
      {
        if (channel_id != 0xff) // otherwise, keey TS read above. 
          fbxts=(*(pl_data + 2) | ((uint64_t)(*(pl_data + 3)) << 32))-ts_skew;
        
        _TRACE(" ++ Event\n");
        // Check event header
        if(!((*pl_data & 0xffff0000) == 0xAFFE0000     // old Febex FW  
             || (*pl_data & 0xffff0000) == 0x115A0000  // Febex FW 1.2+
             || (*pl_data & 0xffff0000) == 0xB00B0000 // Pulser
             || (channel_id == 0xff)))
	  {
          // Oops... Something went wrong
	    fprintf(stderr, "[WARNING] Invalid event header: 0x%08x\n", *pl_data);
	    break;
	  }

	bool good=!bankswitch_issue[sfp_id][module_id] && !dropall;
	if (good && channel_id!=0xff)
	  {
	    if ((int64_t)fbxts<(int64_t)febex_ts_last[sfp_id]-50 )
	      {
		fprintf(stderr, "found a febex ts %20ld fbx ticks (%e s) in the previous readout slice @ %01d.%02d.%02d (hit %d).\n => DROPPED EVENT. \n", febex_ts_last[sfp_id]-fbxts,
			(double)(febex_ts_last[sfp_id]-fbxts)*16.666666666e-9,
			sfp_id, module_id, (int)channel_id, hit_no);
		fprintf(stderr, "febex_ts_last = %20ld -- fbxts = %20ld @ %01d.%02d.%02d, pl_data=%p \n", febex_ts_last[sfp_id], fbxts,
			sfp_id, module_id, (int)channel_id, pl_data);
		badmodules[sfp_id][module_id]++;
		good=0;
	    }
	    else if ((int64_t)fbxts>(int64_t)febex_ts_current[sfp_id]+200 )
	      {
		fprintf(stderr, "found a febex ts %20ld fbx ticks (%e s) in the next readout slice! @ %01d.%02d.%02d (hit %d).\n => DROPPED EVENT.\n", -febex_ts_current[sfp_id]+fbxts,
			(double)(-febex_ts_current[sfp_id]+fbxts)*16.666666e-9,
			sfp_id, module_id, (int)channel_id, hit_no);
		fprintf(stderr, "abs ts is %ld, readout is %ld\n", fbxts, febex_ts_current[sfp_id]);
		badmodules[sfp_id][module_id]++;
		good=0;
	      }
	    else
	      {
		goodmodules[sfp_id][module_id]++;
	      //	    fprintf(stderr, "%d  ", (int)channel_id);
	      }
	  } // check fbx ts (unless already bs issue)

	  
	if (good)
	{
	  event_entry = new multievent_entry;
	  event_entry->_header = se->_header;
	  event_entry->channel_id = (uint8_t)channel_id;
	  event_entry->module_id = (uint8_t)module_id;
	  event_entry->sfp_id = (uint8_t)sfp_id;
	  event_entry->proc_id = proc_id;
	  event_entry->timestamp = fbxts;
	  
	  if (!found_special_ch[sfp_id][module_id])
	    {
	      fprintf(stderr, "no special channel for %d.%d.*\n", sfp_id, module_id);
	      found_special_ch[sfp_id][module_id]=1; //only bug user once
	    }
	      
	  //fprintf(stderr, "found a febex ts %ulld  @ %01d.%02d.%0d2\n", -febex_ts_current+fbxts,sfp_id, module_id, channel_id);
	  _TRACE("skew: %ld", ts_skew);
	  //       fprintf(stdout, "FOOOFOOO Timestamp skew for processor %d, SFP %d, module %d: %ulld (Trigger %d)\n", proc_id, sfp_id, module_id, ts_skew, _file_event._header._info.i_trigger);
	  
	  event_entry->size = (*pl_data & 0xffff) + 8;	// Include GOSIP buffer header
	  _TRACE("    Size: %d\n", event_entry->size);
	  event_entry->data = (uint32_t*)malloc(event_entry->size);

	  memcpy(event_entry->data, pl_bufhead, 8);
	  memcpy(event_entry->data + 2, pl_data, event_entry->size - 8);
          event_entry->wrts=febex2wrts(event_entry->timestamp, event_entry->sfp_id);
	  // Adjust size of GOSIP buffer header
	  *(event_entry->data + 1) = event_entry->size - 8;
	  if (events_available.full())
            {
              fprintf(stderr, "Oops, I could not insert into events_available because it is full. CIRC_BUF_SIZE is %ld, consider increasing that.\n", CIRC_BUF_SIZE);
              exit(1);
            }
	  events_available.push_back(event_entry);
	} // if good
	hit_no++;
        if (channel_id != 0xff)
          pl_data += (*pl_data & 0xffff)/4;
        else
          pl_data += bufsize/4;

        // fprintf(stderr, "Length for %d.%d.%d: %d \n",sfp_id, module_id, channel_id,  (*pl_data & 0xffff)/4);
      }
    }
  }

  for (int sfp=0; sfp<4; sfp++)
    for (int m=0; m<20; m++)
      if (bankswitch_issue[sfp][m])
	fprintf(stderr, "%s for %d.%02d.*\n", (bankswitch_issue[sfp][m]==1)?"BANKSWITCH ISSUE":"TS JUMP", sfp, m);	
      else if (badmodules[sfp][m])
	fprintf(stderr, "summary for %02d.%02d: %03d good, %03d bad\n", (int)sfp,(int)m, (int)goodmodules[sfp][m], (int)badmodules[sfp][m]);

  if (dropall) // after daq restart
    {
      fprintf(stderr, "skipping to next event.\n");
      return load_events();
    }
  /*
  printf("%p, %p ", &(*sorted_up_to), &(*events_available.end()));
  exit(0);
  if(sorted_up_to==events_available.end())
    return unknown_event;
  */

#if 0
  //this does not seem to work with circular buffers:
  sort(sorted_up_to, events_available.end(), multievent_entry::compare);
  std::inplace_merge(events_available.begin(), sorted_up_to, events_available.end(), multievent_entry::compare);
#else
  sort(events_available.begin(), events_available.end(), multievent_entry::compare);
  /*  fprintf(stderr, "\n\n\nafter read:\n");
  for (auto&& e: events_available)
    fprintf(stderr, "wrts=0x%lx\n", e->wrts);
  fprintf(stderr, "\n\n\n");
  */
#endif
  
  // ^-- sort everything, in case an event made it in one readout for one febex but not in another
  return ready;
}

bool multievent_entry::compare(const multievent_entry *e1, const multievent_entry *e2)
{
  return (e1->wrts < e2->wrts);
}
