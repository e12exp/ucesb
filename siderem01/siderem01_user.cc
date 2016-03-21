
#include "structures.hh"

#include "user.hh"

#include "lmd_output.hh"

#include "endian.hh"

#include "sst_peak.hh"

#include "calib_param.hh"

/********************************************************/

#define FLT_CALC_PARAMS     0 // test siderem dsp pedestal calculation
#define FLT_REPACK_DATA     0 // test siderem dsp pack routines
#define SHOW_UNPACK_STRETCHES 0 // show the unpacked stretches from the filter routine
#define SHOW_PACKED_FILTER_DATA 0  // show packed filtered data
#define SIDEREM_PEAKS       0 // test calculation of peaks (with 'old style' data, 
                              // log-parabola-fit, calibrations)
#define SINGLE_EVENT_PLOTS  0 // make plots of single events
#define APAC_PEDESTALS      0 // determine pedestals with apac code
#define CALC_CLUSTER_SUMS   0 // simplistic analysis (useless :-) ) calc cluster sums
#define TEST_HUFFMAN        0 // test huffman encoding
#define CALC_INTER_ADC_DIFF 0 // check inter-adc correlations
#define TEST_CALL_SID_FLT   0 // test the DAQ call wrappers for the filter
#define REWRITE_NONCOMPRESSED 0 // write new subevent with old-style format

/********************************************************/

#define NUM_DET 20

/********************************************************
 *
 * Code to determine pedestals, and test those routines.
 *
 */

#if FLT_CALC_PARAMS

#define WARNING_FMT WARNING
#define ERROR_FMT   ERROR

typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned int    int32;

#define SIDEREM_CALC_FILTER 1
#define MAX_NUM_EVENTS      128
#define BASELINE_CHUNK      64

#include "siderem_filter.c"

flt_all_events sst_data[NUM_DET];

flt_calib  sst_calib[NUM_DET];
flt_config sst_cfg[NUM_DET];

flt_work   flt_state;
#endif

/********************************************************/

#if FLT_REPACK_DATA
#include "siderem_ext.hh"

EXT_SIDEREM sst_unpack_test_d; // diff
EXT_SIDEREM sst_unpack_test_b; // baseline
#endif

/********************************************************/

#if TEST_HUFFMAN
#include "unfinished/huffman.c"

uint32 huff_freq[NUM_DET][4096];
uint32 huff_freq_total[NUM_DET];
#endif

/********************************************************/

#if TEST_CALL_SID_FLT
#include "call_sid_flt.h"

// The .c file would normally be included just in the link, and not
// from the f_user, but in order to make it's inclusion controlled by
// the TEST_CALL_SID_FLT, we do it here
#include "call_sid_flt.c"
#endif

/********************************************************/

int ntotal_prev[NUM_DET] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
uint16 prev_trig;

void user_pedestals(int det_number, int raw_data[1024]);

void raw_user_function(unpack_event *event,
		       raw_event    *raw_event)
{
  // Do the mapping of the unpack->raw SST data

  for (int det = 0; det < NUM_DET; det++)
    {
      EXT_SST &unpack = event->sst.sst[det];
      raw_array_zero_suppress<raw_event_SST,raw_event_SST,1024> &raw = raw_event->SST[det];

      bitsone_iterator iter;
      int i;
      
      while ((i = unpack.data._valid.next(iter)) >= 0)
	{
	  DATA12 value = unpack.data._items[i];

	  raw.insert_index(-1,i).E = value;

	  /* printf ("%d %4d: %4d\n", det, i, value); */
	  /*
	  printf ("%x\n",
		  unpack.header.u32);
	  */

	}
    }
}

bool siderem01_copy_output_file_event(lmd_event_out *event_out,
				      FILE_INPUT_EVENT *file_event,
				      unpack_event *event,
				      const select_event *sel,
				      bool combine_event)
{
  event_out->copy(file_event,sel,combine_event);

#if REWRITE_NONCOMPRESSED
  // Then add a subevent for the rewritten data

  size_t data_length = 0;
  // This must be global...  Or at least, the memory for it must be
  // available when the data is written to the output
  static uint32_t data_rewrite[NUM_DET*(1024+1)];

  for (int det = 0; det < NUM_DET; det++)
    {
      EXT_SST &unpack = event->sst.sst[det];
      bitsone_iterator iter;
      int i;
      
      while ((i = unpack.data._valid.next(iter)) >= 0)
	{
	  DATA12 value = unpack.data._items[i];

	  if (i == 0)
	    {
	      uint32_t header = unpack.header.u32 & 0xfffff000;
	      header |= 1024;

	      data_rewrite[data_length++] = header;
	    }

	  uint32_t d = value.value;

	  uint32_t adc = 0;
	  if (i >= 640)
	    adc = 2;
	  else if (i >= 320)
	    adc = 1;

	  d |= adc << 12;
	  d |= (i - (320 * adc)) << 16; // channel (within adc)
	  d |= 0xf << 28;

	  data_rewrite[data_length++] = d;

	  /* printf ("%d %4d: %4d\n", det, i, value); */
	}
    }

  data_length *= sizeof (uint32_t);

  if (data_length) // only write the subevent if there is some payload
    {
      // First a subevent header
      
      // This must be global...  Or at least, the memory for it must be
      // available when the data is written to the output
      static lmd_subevent_10_1_host _header_add;
      
      _header_add._header.l_dlen = SUBEVENT_DLEN_FROM_DATA_LENGTH(data_length);
      _header_add._header.i_type    = 82;
      _header_add._header.i_subtype = 8210;
#if defined(__i386__) || defined(__x86_64__) || (__BYTE_ORDER == __LITTLE_ENDIAN)
      // choose the X86 marker, little endian
      _header_add.i_procid          = 9;  // hmm, this is a marker of the producer
#elif defined(__powerpc__) || (__BYTE_ORDER == __BIG_ENDIAN)
      // choose the RIO2 marker, big endian
      _header_add.i_procid          = 10; // hmm, this is a marker of the producer
#else
#error Do not know what to use as procid marker
#endif
      _header_add.h_subcrate        = 0;
      _header_add.h_control         = 5;  
      
      // obey command line selection
      if (sel->accept_subevent(&_header_add))
        {
	  // add_chunk just remembers the pointers.  The data is not
          // copied until the actual buffer formatting is performed
          // (after we've returned).
          event_out->add_chunk(&_header_add,sizeof(_header_add),false);

	  event_out->add_chunk(data_rewrite,data_length,false);
        }
    }
#endif
  return true;
}


void user_function(unpack_event *event,
		   raw_event    *raw_event,
		   cal_event    *cal_event,
		   USER_STRUCT  *user_event)
{
  raw_event->PREV.TRIG = prev_trig;
  prev_trig = event->trigger;

  for (int det = 0; det < NUM_DET; det++)
    {
      int raw[1024]; 
      int n = 0;

      if (event->sst.sst[det].header.local_trigger == 2)
	{
	  {
	    bitsone_iterator iter;
	    int i;
	    
	    while ((i = raw_event->SST[det]._valid.next(iter)) >= 0)
	      {
		raw[i] = raw_event->SST[det]._items[i].E.value;
		n++;
	      }  
	  }
#if FLT_CALC_PARAMS
	  if (n == 1024)
	    {
	      flt_all_events *aevd = &sst_data[det];
	      
	      flt_all_events_x *aevdx = &aevd->x;
	      flt_one_event_x  *evdx  = &aevdx->ev[aevdx->events++ % MAX_NUM_EVENTS];
	      	      
	      for (int i = 0; i < 640; i++)
		evdx->data[i] = raw[i];
	      
	      flt_all_events_y *aevdy = &aevd->y;
	      flt_one_event_y  *evdy  = &aevdy->ev[aevdy->events++ % MAX_NUM_EVENTS];
	      	      
	      for (int i = 0; i < 384; i++)
		evdy->data[i] = raw[i+640];
	      
	      if (aevdx->events >= MAX_NUM_EVENTS &&
		  aevdy->events >= MAX_NUM_EVENTS)
		{
		  char prefix_x[64];
		  char prefix_y[64];

		  sprintf (prefix_x,"SST%dx: ",det);
		  sprintf (prefix_y,"SST%dy: ",det);

		  printf ("Calibrate %s %s\n",prefix_x,prefix_y);
		  
		  flt_calc_params_x(&sst_data[det].x,
				    &sst_calib[det].x,
				    &sst_cfg[det].x,4.0,
				    stdout/*NULL*/,prefix_x/*NULL*/);
		  flt_calc_params_y(&sst_data[det].y,
				    &sst_calib[det].y,
				    &sst_cfg[det].y,4.0,
				    stdout/*NULL*/,prefix_y/*NULL*/);
		  
		  aevdx->events = 0;
		  aevdy->events = 0;

		}
	      



	    }
#endif //FLT_CALC_PARAMS

#if CALC_INTER_ADC_DIFF
	  // See if there are any correletions between values that were
	  // digitised at the same time

	  if (n == 1024 && sst_cfg[det].x.sizeof_config)
	    {
	      float diff[3][320];

	      double a, b, c;

	      for (int i = 0; i < 320; i++)
		{
		  printf (""/*"%d/%4d: %4d %5.1f %4d %5.1f %4d %5.1f\n"*/,det,i,
			  raw[i    ],a=raw[i    ]-sst_calib[det].x.pedsigma2[i    ].ped,
			  raw[i+320],b=raw[i+320]-sst_calib[det].x.pedsigma2[i+320].ped,
			  raw[i+640],c=raw[i+640]-sst_calib[det].y.pedsigma2[i    ].ped);

		  diff[0][i] = a-b;
		  diff[1][i] = b-c;
		  diff[2][i] = c-a;
		}

	      flt_ped_sigma pedsigma[3];

	      calc_mean_sigma(diff[0],320,1.0,&pedsigma[0]);
	      calc_mean_sigma(diff[1],320,1.0,&pedsigma[1]);
	      calc_mean_sigma(diff[2],320,1.0,&pedsigma[2]);

	      printf ("diff:%d: %6.1f %6.1f  ",det,pedsigma[0].ped,sqrt(pedsigma[0].var));
	      printf ("%6.1f %6.1f  ",pedsigma[1].ped,sqrt(pedsigma[1].var));
	      printf ("%6.1f %6.1f\n",pedsigma[2].ped,sqrt(pedsigma[2].var));
	    }
#endif
	}
	  
      // printf ("%d:%d  ",det,n);

      // Now, search for normal events, where the 'broken' routine defaulted to
      // store raw data without telling us

      int n_chunk[16];
      
      memset(n_chunk,0,sizeof(n_chunk));

      {
	bitsone_iterator iter;
	int i;
	
	while ((i = raw_event->SST[det]._valid.next(iter)) >= 0)
	  {
	    raw[i] = raw_event->SST[det]._items[i].E.value;
	    n_chunk[i / 64]++;
	  }  
      }   
    
      int ntotal = 0;
      int nfull = 0;
      int mfull = 0;
      
      for (int i = 0; i < 16; i++)
	{
	  ntotal += n_chunk[i];
	  if (n_chunk[i] >= 64)
	    {
	      nfull++;
	      mfull |= 1 << i;
	    }
	}
      /*
      printf ("%2d: total %4d (%d) (trig: %d)\n",det,ntotal,nfull,
	      event->sst.sst[det].header.local_trigger);
      */
/*
      if ((nfull && event->sst.sst[det].header.local_trigger != 2) ||
	  (nfull != 16 && event->sst.sst[det].header.local_trigger == 2))
	{
	  printf ("%2d: total %4d (full %2d) (trig: %d) prev %4d\n",
		  det,ntotal,nfull,
		  event->sst.sst[det].header.local_trigger,
		  ntotal_prev[det]);

	}
*/
      /**/

      ntotal_prev[det] = ntotal;
      
#if FLT_REPACK_DATA
      if (/*event->sst.sst[det].header.local_trigger == 1 &&
	   */sst_cfg[det].x.sizeof_config)
	{
	  // printf ("%d %d %d\n",det,n,event->sst.sst[det].header.local_trigger);

	  if (nfull >= 5)
	    {
	      printf ("  %d: ",det);
	      for (int i = 0; i < 16; i++)
		printf ("%2d ",n_chunk[i]);
	      for (int i = 0; i < 16; i++)
		printf ("%c",n_chunk[i] >= 64 ? 'x' : ' ');
	      // printf("\n");
	      printf("  ");

	      uint32 dest_d_array[2048];
	      uint32 *dest_d = dest_d_array;

	      uint32 dest_b_array[2048];
	      uint32 *dest_b = dest_b_array;

	      if ((mfull & 0x03ff) == 0x03ff)
		{
		  uint32 src[640];

		  for (int i = 0; i < 640; i++)
		    src[i] = raw[i];

		  dest_d = flt_zero_suppress_x(&sst_cfg[det].x,&flt_state.x,src,dest_d);
		  //dest_d = flt_zero_suppress_diff_x(&sst_cfg[det].x,&flt_state.x,src,dest_d);
		  //dest_b = flt_zero_suppress_baseline_x(&sst_cfg[det].x,&flt_state.x,src,dest_b);
		}
	      if ((mfull & 0xfc00) == 0xfc00)
		{
		  uint32 src[384];

		  for (int i = 0; i < 384; i++)
		    src[i] = raw[i+640];

		  dest_d = flt_zero_suppress_y(&sst_cfg[det].y,&flt_state.y,src,dest_d);
		  //dest_d = flt_zero_suppress_diff_y(&sst_cfg[det].y,&flt_state.y,src,dest_d);
		  //dest_b = flt_zero_suppress_baseline_y(&sst_cfg[det].y,&flt_state.y,src,dest_b);
		}
	      /*
	      printf ("%d %d %04x => %d\n",
		      det,event->sst.sst[det].header.local_trigger,mfull,
		      dest-dest_array);
	      */
#if SHOW_PACKED_FILTER_DATA
	      for (uint32 *p = dest_array; p < dest; p++)
		printf (" %08x ",*p);
	      printf ("\n");
#endif
	      
	      sst_unpack_test_d.__clean();
	      __data_src<0,0> buffer_d(dest_d_array,dest_d);
	      sst_unpack_test_d.__unpack(buffer_d);

	      sst_unpack_test_b.__clean();
	      __data_src<0,0> buffer_b(dest_b_array,dest_b);
	      sst_unpack_test_b.__unpack(buffer_b);

	      //
#if SHOW_UNPACK_STRETCHES
	      for (int s = 0; s < sst_unpack_test._x._num_stretches; s++)
		{
		  EXT_SIDEREM_stretch &stretch = sst_unpack_test._x._stretches[s];

		  uint32 *d = stretch._data;

		  for (int i = stretch._start; i < stretch._end; i++)
		    {
		      uint32 val = *(d++);

		      printf ("x %4d: %4d %6.1f\n",i,val,val - sst_calib[det].x.pedsigma2[i].ped);
		    }
		  printf ("\n");
		}

	      for (int s = 0; s < sst_unpack_test._y._num_stretches; s++)
		{
		  EXT_SIDEREM_stretch &stretch = sst_unpack_test._y._stretches[s];

		  uint32 *d = stretch._data;

		  for (int i = stretch._start; i < stretch._end; i++)
		    {
		      uint32 val = *(d++);

		      printf ("y %4d: %4d %6.1f\n",i,val,val - sst_calib[det].y.pedsigma2[i].ped);
		    }
		  printf ("\n");
		}
#endif
	      printf ("%d/%d: %4d %4d",
		      event->sst.sst[det].header.local_trigger,
		      det,(dest_d-dest_d_array)*4,(dest_b-dest_b_array)*4);
	      if ((mfull & 0x03ff) == 0x03ff)
		printf (" x%d/%d",
			sst_unpack_test_d._x._num_stretches,
			sst_unpack_test_b._x._num_stretches);
	      else
		printf (" x-/x");
	      if ((mfull & 0xfc00) == 0xfc00)
		printf (" y%d/%d",
			sst_unpack_test_d._y._num_stretches,
			sst_unpack_test_b._y._num_stretches);
	      else
		printf (" y-/-");
	      printf ("\n");

	    }
	}
#endif

#if TEST_HUFFMAN
      if (mfull)
	{
	  // calculate frequencies of the differences

	  for (int i = 0; i < 16; )
	    {
	      if (n_chunk[i] >= 64)
		{
		  int start = i*64;
		  int end   = start + 64;
		  
		  while (++i < 16 && n_chunk[i] >= 64)
		    end += 64;		

		  huffman_count_freq(huff_freq[det],raw+start,raw+end);
		  huff_freq_total[det] += end-start;



		  ///printf ("======================================================\n");

		  uint32 pack[1024];		  
		  uint32* pack_end = encode_stream((uint32*)raw+start,
						   (uint32*)raw+end,pack);
		  
		  ///printf ("------------------------------------------------------\n");
		  
		  uint32 unpack[1024];  
		  decode_stream(pack,pack_end,unpack,unpack+(end-start));

		  // printf ("%d -> %d\n",end-start,(pack_end-pack)*sizeof(uint32));

		  for (int k = 0; k < end-start; k++)
		    if (unpack[k] != raw[start+k])
		      printf ("Mismatch!!!\n");
		  

		}
	      i++;
	    }
	}
#endif

            
      // printf ("\n");
      
      
      // Do some rudimentary 'cluster finding'.
      // We declare any stretch of data that is above 0 to be a cluster...
      
      if (!nfull)
	{
#if SIDEREM_PEAKS
	  sid_data data_raw;
	  sid_data data_cal;
	  
	  data_raw.n = 0;
	  data_cal.n = 0;
	  
	  {
	    bitsone_iterator iter;
	    int i;
	    
	    while ((i = raw_event->SST[det]._valid.next(iter)) >= 0)
	      {
		data_raw.data[data_raw.n].strip = i;
		data_raw.data[data_raw.n].value = raw_event->SST[det]._items[i].E.value;
		data_raw.n++;	    
	      }  
	  }
	  
	  {
	    int i;
	    
	    for (int i = 0; i < cal_event->SST[det]._num_items; i++)
	      {
		data_cal.data[data_cal.n].strip = cal_event->SST[det]._items[i]._index;
		data_cal.data[data_cal.n].value = cal_event->SST[det]._items[i]._item.E;
		data_cal.n++;	    
	      }  
	  }
	  
	  siderem_peaks(det,data_raw,user_event->PEAK_RAW[det]);
	  siderem_peaks(det,data_cal,user_event->PEAK[det]);
#endif

#if SINGLE_EVENT_PLOTS
	  {
	    bitsone_iterator iter;
	    int i;
	    
	    char pict[101+101][640+10+10+2];
	    
	    memset(pict,255,sizeof(pict));
	    
	    for (int i = 1; i < 101; i += 2)
	      {
		pict[i][10] = 128;
		pict[i][10+1+640] = 128;
		
		pict[i+101][(640-384)/2+10] = 128;
		pict[i+101][(640-384)/2+10+1+384] = 128;
	      }
	    
	    for (int i = 0; i < 640; i += 2)
	      {
		pict[96-50][10+1+i] = 192;
		pict[96   ][10+1+i] = 128;
	      }
	    for (int i = 0; i < 384; i += 2)
	      {
		pict[101+96-50][(640-384)/2+10+1+i] = 192;
		pict[101+96   ][(640-384)/2+10+1+i] = 128;
	      }
	    
	    while ((i = raw_event->SST[det]._valid.next(iter)) >= 0)
	      {
		int value = raw_event->SST[det]._items[i].E.value;
		int x, y;
		
		if (i < 640)
		  {
		    x = 10+1+i;
		    y = 0;
		  }
		else
		  {
		    x = (640-384)/2+10+1+i-640;
		    y = 101;
		  }
		
		if (value > 90)
		  {
		    pict[y+5][x] = 0;
		    pict[y+1][x] = 0;
		  }
		else
		  {
		    pict[y+96-value][x] = 0;
		    pict[y+95-value][x] = 0;
		    }
		
		}  
	    
	    static int fileno[4] = { 0,0,0,0 };
	    char filename[64];

	    sprintf(filename,"sid%d_%04d.pgm",det,fileno[det]++);
	    FILE *fid = fopen(filename,"wt");

	    fprintf (fid,"P5\n%d %d\n%d\n",10+10+1+1+640,101*2,255);

	    fwrite(pict,1,(10+10+1+1+640)*101*2,fid);

	    fclose(fid);
	  }
#endif
#if CALC_CLUSTER_SUMS
	  {
	    bitsone_iterator iter;
	    int i;
	    int start_i;
	    int last_i = -2;
	    float sum = 0;
	    int clusters = 0;
	    
	    if ((i = raw_event->SST[det]._valid.next(iter)) >= 0)
	      {	  
		start_i = i;
		last_i  = i;
		sum     = raw_event->SST[det]._items[i].E.value;
		
		while ((i = raw_event->SST[det]._valid.next(iter)) >= 0)
		  {
		    if (last_i == i-1)
		      sum += raw_event->SST[det]._items[i].E.value;
		    else
		      {
			// We have a cluster (start_i -- last_i, sum)

			if (sum > 15)
			  {
			    hit_event_CLUSTER_one &item =
			      user_event->CLUSTER[det].hits.do_insert_index(clusters++);
			    
			    item.start = start_i;
			    item.end   = last_i;
			    item.sum   = sum + (double) random() / RAND_MAX;
			  }
			
			start_i = i;
			sum     = raw_event->SST[det]._items[i].E.value;
		      }
		    last_i = i;
		  }  
		
		// We have a cluster (start_i -- last_i, sum)
		
		if (sum > 15)
		  {
		    hit_event_CLUSTER_one &item =
		      user_event->CLUSTER[det].hits.do_insert_index(clusters++);
		
		    item.start = start_i;
		    item.end   = last_i;
		    item.sum   = sum + (double) random() / RAND_MAX;
		  }
	      }
	  }
#endif
	}
    }
  
#if APAC_PEDESTALS
  for (int det=0; det<NUM_DET; det++)
    {
      bitsone_iterator iter;
      int i;
      int n = 0;
      int last_i = -1;
      
      while ((i = raw_event->SST[det]._valid.next(iter)) >= 0)
	{
	  if (i != last_i + 1 && last_i != -1)    printf (" .");
	  //printf (" %d",raw_event->SST[det]._items[i].E);
	  last_i = i;
	  n++;
	} 
      //printf ("  (stat :  %d)\n\n",n);
   
      //TRIGGER 1 zero suppression
      //if (event->sst.sst[det].header.local_trigger == 1)  
      //	{
      //	  
      //	}//end of TRIGGER 1

      //TRIGGER 3 which correspond to the trigger 2 in local trigger, no zero suppression : calculation of the pedestal,sigma
      if (event->sst.sst[det].header.local_trigger == 2)
	{  
	  int raw[1024]; 
	  bitsone_iterator iter;
	  int i;
	  int n = 0;
	  
	  while ((i = raw_event->SST[det]._valid.next(iter)) >= 0)
	    {
	      raw[i] = raw_event->SST[det]._items[i].E.value;
	    }  
	  //printf("event %7d: trigger = %d  local trigger = %d\n",
	  // event->event_no,event->trigger,
		 //event->sst1.header.local_event_counter,
	  // event->sst.sst[det].header.local_trigger);
	  
	  user_pedestals(det,raw);
	}  
    }
#endif  

#if TEST_CALL_SID_FLT
  /* We can only call the wrapper routines if the data is either
   * complete (or empty)
   */
  
  for (int det=0; det<NUM_DET; det++)
    if (event->sst.sst[det].header.count != 0 && 
	event->sst.sst[det].header.count != 1024)
      goto cannot_call_wrapper;

  printf ("call wrapper: ... ");

  // Now, we'd have liked to get our hands on the original buffer, but
  // this is a very messy thing...

  {
    uint32 buf[1025*4];
    uint32 *ptr = buf;

    uint32 dest[1025*4];
    uint32 *end;

    for (int det=0; det<NUM_DET; det++)
      {
	*(ptr++) = event->sst.sst[det].header.u32;

	if (event->sst.sst[det].header.count)
	  {
	    for (int i = 0; i < 320; i++)
	      *(ptr++) = 0xf0000000 | (i << 16) | (0 << 12) | 
		(raw_event->SST[det]._items[i].E.value);
	    for (int i = 0; i < 320; i++)
	      *(ptr++) = 0xf0000000 | (i << 16) | (1 << 12) | 
		(raw_event->SST[det]._items[i+320].E.value);
	    for (int i = 0; i < 384; i++)
	      *(ptr++) = 0xf0000000 | (i << 16) | (2 << 12) | 
		(raw_event->SST[det]._items[i+640].E.value);
	  }
      }
    end = call_sid_flt_event_data(buf,ptr,dest,0);

    printf ("%d -> %d\n",ptr-buf,end-dest);
  }

 cannot_call_wrapper:
  ;
#endif
}

int _huffman_initialized = 0;

void user_init()
{
#if FLT_CALC_PARAMS
  memset(sst_data,0,sizeof(sst_data));
  memset(sst_calib,0,sizeof(sst_calib));
  memset(sst_cfg,0,sizeof(sst_cfg));
#endif

#if TEST_HUFFMAN
  memset(huff_freq,0,sizeof(huff_freq));
  memset(huff_freq_total,0,sizeof(huff_freq_total));

  // show_calib_map();

  if (!_huffman_initialized)
    {
      setup_huffman();
      setup_unpack_tables();
      _huffman_initialized = 1;
    }
  // print_huffman();
#endif

#if TEST_CALL_SID_FLT
  // test multiple calls...
  for (int i = 0; i < 10; i++)
    {
      call_sid_flt_init();
      call_sid_flt_init_detector(3,0,1);
      call_sid_flt_init_detector(3,0,2);
      call_sid_flt_init_detector(3,0,3);
      call_sid_flt_init_detector(3,0,4);

      call_sid_flt_init_detector(5,0,1);
      call_sid_flt_init_detector(5,0,2);
      call_sid_flt_init_detector(5,0,3);
      call_sid_flt_init_detector(5,0,4);

      call_sid_flt_init_detector(5,1,1);
      call_sid_flt_init_detector(5,1,2);
      call_sid_flt_init_detector(5,1,3);
      call_sid_flt_init_detector(5,1,4);
    }
#endif
}

void user_exit()
{
#if TEST_HUFFMAN
  for (int det = 0; det < NUM_DET; det++)
    {
      printf ("========== %d ==========\n",det);

      optimize_huff(huff_freq[det],4096);
      
    }

  uint freq[4096];
  for (int i = 0; i < 4096; i++)
    {
      freq[i] = 0;
      for (int det = 0; det < 4; det++)
	freq[i] += huff_freq[det][i];
    }

  printf ("========== total ==========\n");
  
  optimize_huff(freq,4096);
#endif
      

}

// Data files: /d/rising03/s271/data
// tsmcli frsraw run150

/*

trigger 1  = physics with zero suppresion
trigger 2  = cosmics with zero suppression
trigger 3  = clock no zero suppression 
trigger 4  = time calibrator no readout correspond to the synchronisation
trigger 12 = begin of spill only headers, no readout
trigger 13 = en of spill    only headers, no readout

*/
