
#include "structures.hh"

#include "threshold_cros3.c"

#include "user.hh"

#include "event_base.hh"

#include "lmd_output.hh"

#include "endian.hh"

#include <algorithm>

#define PRINT_GRID               0
#define EXAMINE_THRESHOLD_CURVE  0

#define NUM_WIRE_X 144
#define NUM_WIRE_Y 112

#define NUM_GRID_X (NUM_WIRE_X/3)
#define NUM_GRID_Y (NUM_WIRE_Y/6)

#define GRID_X(x) (1+(x)*(NUM_GRID_X)/(NUM_WIRE_X))
#define GRID_Y(y) (1+(y)*(NUM_GRID_Y)/(NUM_WIRE_Y))

cros3_thr_info thr_info[2][256];

void user_function(unpack_event *event,
		   raw_event    *raw_event,
		   cal_event    *cal_event,
		   cros3_user_struct *user_event)
{
  for (int i = 0; i < 2; i++)
    {
      wire_hits &hits = event->cros3.ccb[i].data;

      if (!hits.num_hits)
	continue;

      PDC_hits &ccb = user_event->ccb[i /*ccb_id - 1*/];

      for (int j = 0; j < hits.num_hits && j < MAX_CROS3_hits; j++)
	{
	  wire_hit &unpack_hit = hits.hits[j];

	  PDC_wire_hit &hit = 
	    ccb.hits.append_item(-1);
	  
	  hit.wire  = unpack_hit.wire;
	  hit.start = unpack_hit.start;
	  hit.stop  = unpack_hit.stop;
	}


#if PRINT_GRID
      printf ("==== CCB %d: hits: %d ==================================\n",
	      i+1,hits.num_hits);

      std::sort(hits.hits,hits.hits+hits.num_hits);

      char grid[NUM_GRID_Y+1+1+1][NUM_GRID_X+1+1+2];

      memset(grid,' ',sizeof(grid));

      for (int y = 0; y < NUM_GRID_Y+3; y++)
	{
	  grid[y][0]  = '|';
	  grid[y][NUM_GRID_X+1] = '|';
	  grid[y][NUM_GRID_X+3] = 0;
	}

      for (int x = 0; x < NUM_GRID_X+3; x++)
	{
	  grid[0][x]  = '-';
	  grid[NUM_GRID_Y+1][x] = '-';
	}

      grid[           0][           0] = '+';
      grid[NUM_GRID_Y+1][           0] = '+';
      grid[           0][NUM_GRID_X+1] = '+';
      grid[NUM_GRID_Y+1][NUM_GRID_X+1] = '+';

      int xs[NUM_WIRE_X];
      int num_x = 0;
      int ys[NUM_WIRE_Y];
      int num_y = 0;

      for (int j = 0; j < hits.num_hits; j++)
	{
	  wire_hit &hit = hits.hits[j];

	  if (hit.wire < NUM_WIRE_X)
	    {
	      int y = hit.wire;
	      grid[GRID_Y(y)][NUM_GRID_X+2] = '<';
	      ys[num_y++] = y;
	    }
	  else
	    {
	      int x = hit.wire - NUM_WIRE_X;
	      grid[NUM_GRID_Y+2][GRID_X(x)] = '^';
	      xs[num_x++] = x;
	    }

	  printf ("hit %d/%3d: %3d %3d\n",
		  i+1,hit.wire,hit.start,hit.stop);
	}

      int pairs[NUM_WIRE_X * NUM_WIRE_Y];
      int num_pairs = 0;

      int xps[NUM_WIRE_X];
      int num_xp = 0;
      int yps[NUM_WIRE_Y];
      int num_yp = 0;

      for (int j = 0; j < hits.num_hits-1; j++)
	{
	  wire_hit &hit1 = hits.hits[j];
	  wire_hit &hit2 = hits.hits[j+1];

	  if (hit1.wire+1 == hit2.wire)
	    pairs[num_pairs++] = hit1.wire;
	}

      for (int j = 0; j < num_pairs; j++)
	{
	  printf("pair: %d\n",pairs[j]);
	}

     
      for (int j = 0; j < num_pairs; j++)
	{
	  int p1 = pairs[j];

	  if (p1 < NUM_WIRE_X)
	    {
	      int y = p1;
	      grid[GRID_Y(y)][NUM_GRID_X+2] = '-';

	      printf ("Y:%d\n",y);

	      yps[num_yp++] = y;
	    }
	  else
	    {
	      int x = p1 - NUM_WIRE_X;
	      grid[NUM_GRID_Y+2][GRID_X(x)] = '|';

	      printf ("X:%d\n",x);

	      xps[num_xp++] = x;
	    }
	}
	  
      for (int j = 0; j < num_y; j++)
	{
	  for (int k = 0; k < num_x; k++)
	    {
	      int x = xs[k];
	      int y = ys[j];
	      
	      grid[GRID_Y(y)][GRID_X(x)] = '+';
	      
	      printf("pfft: %d,%d\n",x,y);
	    }
	}

      for (int j = 0; j < num_yp; j++)
	{
	  for (int k = 0; k < num_xp; k++)
	    {
	      int x = xps[k];
	      int y = yps[j];
	      
	      grid[GRID_Y(y)][GRID_X(x)] = 'B';
	      
	      printf("BOOM: %d,%d\n",x,y);
	    }
	}

      for (int y = 0; y < NUM_GRID_Y+3; y++)
	printf ("%s\n",grid[y]);

#endif
      
    }

}

void user_init()
{
  memset(&thr_info,0,sizeof(thr_info));
}

void user_exit()
{
#if EXAMINE_THRESHOLD_CURVE
  for (int i = 0; i < 2; i++)
    {
      cros3_threshold_stat *measurement = &(_static_event._unpack.cros3.ccb[i].trc);

      for (int ch = 0; ch < 256; ch++)
	{
	  for (int th = 0; th < 256; th++)
	    if (measurement->data[ch].data[th].max_counts)
	      goto good_file;

          continue;
          good_file:

	  printf ("THRESHOLD-CURVE %d/%3d:\n",i,ch);

	  cros3_examine_threshold_curve(measurement,
					&measurement->data[ch],
					&thr_info[i][ch]);

	  cros3_thr_info_t &info = thr_info[i][ch];

	  printf ("THR-EXAMINE: %d/%03d:",i,ch);
	  
	  if (info._info & CROS3_THR_INFO_OK)
	    printf (" m=%7.2f s=%7.2f",info._mean,info._sigma);
	  else
	    printf (" m=      - s=      -");


	  printf (" (i=%04x",info._info);

#define PRINT_CROS3_INFO_MASK_ITEM(info,item) if((info) & CROS3_THR_INFO_##item) printf (" %s",#item);

	  PRINT_CROS3_INFO_MASK_ITEM(info._info,OK           );
	  PRINT_CROS3_INFO_MASK_ITEM(info._info,NOISE_SEEN   );
	  PRINT_CROS3_INFO_MASK_ITEM(info._info,SILENCE_SEEN );
	  PRINT_CROS3_INFO_MASK_ITEM(info._info,NO_NOISE     );
	  PRINT_CROS3_INFO_MASK_ITEM(info._info,LOW_MISS     );
	  PRINT_CROS3_INFO_MASK_ITEM(info._info,NO_SILENCE   );
	  PRINT_CROS3_INFO_MASK_ITEM(info._info,HIGH_MISS    );
	  PRINT_CROS3_INFO_MASK_ITEM(info._info,NO_DATA      );
	  PRINT_CROS3_INFO_MASK_ITEM(info._info,TOO_LITTLE_DATA);
	  PRINT_CROS3_INFO_MASK_ITEM(info._info,INTERP_FAIL  );
	  PRINT_CROS3_INFO_MASK_ITEM(info._info,RINGING      );

	  if (info._info & CROS3_THR_INFO_RINGING)
	    printf (" rf=%7.5f",info._ring_y0);

	  printf (")");

	  printf ("\n");
	}
    }
#endif
}

bool cros3_copy_output_file_event(lmd_event_out *event_out,
				  FILE_INPUT_EVENT *file_event,
				  unpack_event *event,
				  const select_event *sel,
				  combine_event)
{
  event_out->copy(file_event,sel,combine_event);

  // Then add a subevent for the rewritten data

  size_t data_length = 0;

  for (int i = 0; i < 2; i++)
    {
      size_t length = ((char *) event->cros3.ccb[i]._dest_end) -
        ((char *) event->cros3.ccb[i]._dest_buffer);

      data_length += length;
    }

  if (data_length) // only write the subevent if there is some payload
    {
      // First a subevent header
      
      // This must be global...  Or at least, the memory for it must be
      // available when the data is written to the output
      static lmd_subevent_10_1_host _header_add;
      
      _header_add._header.l_dlen = SUBEVENT_DLEN_FROM_DATA_LENGTH(data_length);
      _header_add._header.i_type    = 85;
      _header_add._header.i_subtype = 8510;
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
	  // add_chunk just remembers the pointers.  The data is not copied until
	  // the actual buffer formatting is performed (after we've returned).
	  event_out->add_chunk(&_header_add,sizeof(_header_add),false);
	  
	  for (int i = 0; i < 2; i++)
	    {
	      size_t length = ((char *) event->cros3.ccb[i]._dest_end) -
		((char *) event->cros3.ccb[i]._dest_buffer);
	      
	      event_out->add_chunk(event->cros3.ccb[i]._dest_buffer,length,false);
	    }
	}
    }

  return true;
}

// Data files: /u/kurz/cros3/cros3_thres_32dig.lmd

