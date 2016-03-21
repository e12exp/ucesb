
#include "structures.hh"

#include "user.hh"

#include <math.h>

#include "struct_mapping.hh"

#include "event_base.hh"

#include "sync_check.hh"

#include "corr_plot.hh"

#include "vesna5.h"

int _check_fifo_ec_match = 1;

v5_conf_t _v5_conf = { 
  NULL 
};

void esn_usage_command_line_options()
{
  //      "  --option          Explanation.\n"
  printf ("  --no-fifo-ec-check  Don't check fifo event counters.\n");
  printf ("  --cal-vdc-tdc-prefix=PATH  Prefix for millepede files.\n");
}

bool esn_handle_command_line_option(const char *arg)
{
  const char *post;

#define MATCH_PREFIX(prefix,post) (strncmp(arg,prefix,strlen(prefix)) == 0 && *(post = arg + strlen(prefix)) != '\0')
#define MATCH_ARG(name) (strcmp(arg,name) == 0)
  
  if (MATCH_ARG("--no-fifo-ec-check")) {
    _check_fifo_ec_match = 0;
    return true;
  } else if (MATCH_PREFIX("--cal-vdc-tdc-prefix=",post)) {
    _v5_conf._cal_vdc_tdc_prefix = post;
    return true;
  }

  return false;
}

sync_check<0x100> _vdc_ec_sync[2];

int esn_user_function_unpack(unpack_event *event)
{
  // check the event counters

  // Take the scaler event counter as basis (just one of them, all
  // must anyhow match)

  uint16 ec = event->esn.ev.sci.fifo.event_counter;

  if (event->esn.ev.mwpc[0].fifo.event_counter != ec ||
      event->esn.ev.mwpc[1].fifo.event_counter != ec ||
      event->esn.ev.vdc[0].fifo.event_counter != ec ||
      event->esn.ev.vdc[1].fifo.event_counter != ec)
    {

      if (_check_fifo_ec_match)
	ERROR("Fifo event counters mismatch (%04x %04x %04x %04x %04x).",
	      event->esn.ev.mwpc[0].fifo.event_counter,
	      event->esn.ev.mwpc[1].fifo.event_counter,
	      event->esn.ev.vdc[0].fifo.event_counter,
	      event->esn.ev.vdc[1].fifo.event_counter,
	      event->esn.ev.sci.fifo.event_counter);
    }
  /*
  if (!(event->esn.ev.mwpc[0].fifo.header & 0x4000) ||
      !(event->esn.ev.mwpc[1].fifo.header & 0x4000) ||
      !(event->esn.ev.vdc[0].fifo.header & 0x4000) ||
      !(event->esn.ev.vdc[1].fifo.header & 0x4000) ||
      !(event->esn.ev.sci.fifo.header & 0x4000))
    {
      ERROR("Fifo without timeout (%04x %04x %04x %04x %04x).",
	    event->esn.ev.mwpc[0].fifo.header,
	    event->esn.ev.mwpc[1].fifo.header,
	    event->esn.ev.vdc[0].fifo.header,
	    event->esn.ev.vdc[1].fifo.header,
	    event->esn.ev.sci.fifo.header);
    }
  */
  // Then we must check that the VDC event counters are in sync
  // since they may have an offset relative to the fifo counters,
  // this requires more logics...
#if 0
  event_counter_bits<16> ec_16(event->esn.ev.sci.fifo.event_counter);
      
  for (int i = 0; i < 2; i++)
    {
      if (!_vdc_ec_sync[i].check_sync(event->esn.ev.vdc[1]._sync_info,
				      ec_16))
	{
	  ERROR("Event counter mismatch in VDC[%d].",i);
	}
    }
#endif

  return 1;
}

/**************************************************************/

void esn_user_function_raw(unpack_event *event,
			   raw_event    *raw_event)
{
  // The mapping functions insert data into the hit arrays, but cannot
  // do the sorting, as they do not know when they have filled the
  // last entry.  So we do the sorting after all filling is done...

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 2; j++)
      raw_event->MWPC[i][j].H.sort_hits();  

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      raw_event->VDC[i][j].H.sort_hits();  
  //printf ("---\n");
}

/**************************************************************/

#define CORR_PICTURES 0

#if CORR_PICTURES
#define NUM_MWPC_BUCKETS 512
#define NUM_MWPC_CHUNK   32
#define NUM_MWPC_ADDR    (NUM_MWPC_BUCKETS*NUM_MWPC_CHUNK)

ppcorr<NUM_MWPC_BUCKETS,
       NUM_MWPC_CHUNK>   mwpc_corr1[2];
ppcorr<2*NUM_MWPC_BUCKETS,
       NUM_MWPC_CHUNK>   mwpc_corr2;

ppcorr<64,16>            mwpc_raw_corr[4][2];

#define NUM_VDC_BUCKETS 256
#define NUM_VDC_CHUNK   32
#define NUM_VDC_ADDR    (NUM_VDC_BUCKETS*NUM_VDC_CHUNK)

ppcorr<NUM_VDC_BUCKETS,
       NUM_VDC_CHUNK>   vdc_corr1[2];
ppcorr<2*NUM_VDC_BUCKETS,
       NUM_VDC_CHUNK>   vdc_corr2;

ppcorr<15,16>           vdc_raw_corr[2][2];
#endif

/**************************************************************/

int compare_EXT_VDC_item(const void *p1,const void *p2)
{
  int addr1 = ((const EXT_VDC_item*) p1)->_full_address;
  int addr2 = ((const EXT_VDC_item*) p2)->_full_address;

  return addr1 - addr2;
}

void esn_user_function(unpack_event *event,
		       raw_event    *raw_event,
		       cal_event    *cal_event)
{
  /*
  printf ("+++\n");

  for (int m = 0; m < 2; m++)
    {
      EXT_MWPC *mwpc = &event->esn.ev.mwpc[m];

      for (EXT_MWPC_item *item = mwpc->_items_start;
	   item != mwpc->_items_end; item++)
	{
	  printf ("%d: %04d..%04d\n",m,item->_start,item->_end);

	}

    }
  
  printf ("---\n");
  */
#if CORR_PICTURES
  for (int m = 0; m < 2; m++)
    {
      EXT_MWPC *mwpc = &event->esn.ev.mwpc[m];
      
      for (EXT_MWPC_item *item1 = mwpc->_items_start;
	   item1 < mwpc->_items_end; item1++)
	{
	  mwpc_corr1[m].add_range_self(item1->_start,item1->_end);
	  
	  for (int i1 = item1->_start; i1 < item1->_end; i1++)
	    for (EXT_MWPC_item *item2 = item1 + 1;
		 item2 < mwpc->_items_end; item2++)
	      mwpc_corr1[m].add_range(item1->_start,item1->_end,
				      item2->_start,item2->_end);
	}
    }
  
  for (int m1 = 0; m1 < 2; m1++)
    {
      EXT_MWPC *mwpc1 = &event->esn.ev.mwpc[m1];
      int m1_off = m1*NUM_MWPC_ADDR;
      
      for (EXT_MWPC_item *item1 = mwpc1->_items_start;
	   item1 < mwpc1->_items_end; item1++)
	{
	  mwpc_corr2.add_range_self(m1_off+item1->_start,m1_off+item1->_end);
	  
	  for (int i1 = item1->_start; i1 < item1->_end; i1++)
	    {
	      for (EXT_MWPC_item *item2 = item1 + 1;
		   item2 < mwpc1->_items_end; item2++)
		mwpc_corr2.add_range(m1_off+item1->_start,m1_off+item1->_end,
				     m1_off+item2->_start,m1_off+item2->_end);
	      
	      if (m1 == 0)
		{
		  EXT_MWPC *mwpc2 = &event->esn.ev.mwpc[1];
		  int m2_off = 1*NUM_MWPC_ADDR;
		  
		  for (EXT_MWPC_item *item2 = mwpc2->_items_start;
		       item2 < mwpc2->_items_end; item2++)
		    mwpc_corr2.add_range(m1_off+item1->_start,m1_off+item1->_end,
					 m2_off+item2->_start,m2_off+item2->_end);
		}
	    }
	}
    }
  

  for (int m = 0; m < 2; m++)
    {
      EXT_VDC *vdc = &event->esn.ev.vdc[m];
      
      // In order for the plots to bi traingular, we must sort the entries...

      qsort(vdc->_items_start,
	    vdc->_items_end-vdc->_items_start,sizeof(EXT_VDC_item),
	    compare_EXT_VDC_item);

      for (EXT_VDC_item *item1 = vdc->_items_start;
	   item1 < vdc->_items_end; item1++)
	{
	  vdc_corr1[m].add_self(item1->_full_address);

	  for (EXT_VDC_item *item2 = item1 + 1;
	       item2 < vdc->_items_end; item2++)
	    vdc_corr1[m].add(item1->_full_address,item2->_full_address);
	}
    }

  for (int m1 = 0; m1 < 2; m1++)
    {
      EXT_VDC *vdc1 = &event->esn.ev.vdc[m1];
      int m1_off = m1*NUM_VDC_ADDR;
      
      for (EXT_VDC_item *item1 = vdc1->_items_start;
	   item1 < vdc1->_items_end; item1++)
	{
	  vdc_corr2.add_self(m1_off+item1->_full_address);

	  for (EXT_VDC_item *item2 = item1 + 1;
	       item2 < vdc1->_items_end; item2++)
	    vdc_corr2.add(m1_off+item1->_full_address,m1_off+item2->_full_address);

	  if (m1 == 0)
	    {
	      EXT_VDC *vdc2 = &event->esn.ev.vdc[1];
	      int m2_off = 1*NUM_VDC_ADDR;
	      
	      for (EXT_VDC_item *item2 = vdc2->_items_start;
		   item2 < vdc2->_items_end; item2++)
		vdc_corr2.add(m1_off+item1->_full_address,
			      m2_off+item2->_full_address);
	    }
	}
    }

  for (int mwpc = 0; mwpc < 4; mwpc++)
    for (int plane = 0; plane < 2; plane++)
      {
	EXT_MWPC_RAW *pmwpc = &raw_event->MWPC[mwpc][plane].H;
	
	for (EXT_MWPC_RAW_item *item1 = pmwpc->_items_start;
	   item1 < pmwpc->_items_end; item1++)
	  {
	    mwpc_raw_corr[mwpc][plane].add_range_self(item1->_start,item1->_end);
	    
	    for (EXT_MWPC_RAW_item *item2 = item1 + 1;
		 item2 < pmwpc->_items_end; item2++)
	      mwpc_raw_corr[mwpc][plane].add_range(item1->_start,item1->_end,
						   item2->_start,item2->_end);
	  }
      }

  for (int vdc = 0; vdc < 2; vdc++)
    for (int plane = 0; plane < 2; plane++)
      {
	EXT_VDC_RAW *pvdc = &raw_event->VDC[vdc][plane].H;
	
	for (EXT_VDC_RAW_item *item1 = pvdc->_items_start;
	   item1 < pvdc->_items_end; item1++)
	  {
	    vdc_raw_corr[vdc][plane].add_self(item1->_wire);
	    
	    for (EXT_VDC_RAW_item *item2 = item1 + 1;
		 item2 < pvdc->_items_end; item2++)
	      vdc_raw_corr[vdc][plane].add(item1->_wire,item2->_wire);
	  }
      }
#endif



  /*
  for (int d = 0; d < 4; d++)
    for (int c = 0; c < 4; c++)
      event->
  */
#ifdef USE_VESNA5

  v5_event_t v5_event;

  for (int vdc = 0; vdc < 2; vdc++)
    for (int plane = 0; plane < 2; plane++)
      {
	EXT_VDC_RAW *pvdc = &raw_event->VDC[vdc][plane].H;

	v5_event._vdc_nhits[vdc][plane] = 0;

	vdc_hit *d = v5_event._vdc_hit[vdc][plane];

	EXT_VDC_RAW_item *end = pvdc->_items_end;
	if (end > pvdc->_items_start + countof(v5_event._vdc_hit[vdc][plane]))
	  end = pvdc->_items_start + countof(v5_event._vdc_hit[vdc][plane]);
	
	for (EXT_VDC_RAW_item *item = pvdc->_items_start;
	   item < end; item++)
	  {
	    d->_wire = item->_wire;
	    d->_time = item->_time;

	    d++;
	  }

	v5_event._vdc_nhits[vdc][plane] = d - v5_event._vdc_hit[vdc][plane];
      }

  for (int mwpc = 0; mwpc < 4; mwpc++)
    for (int plane = 0; plane < 2; plane++)
      v5_event._mwpc_nhits[mwpc][plane] = 0;

  vesna5(&v5_event);
#endif

// #define VDC_PLOT
#undef VDC_PLOT

#ifdef VDC_PLOT

#define WIDTH   (240)
#define HEIGHT  (64)
#define MAXT     512

#define PHEIGHT (4 * HEIGHT + 3)

  size_t dim = WIDTH * 3 * PHEIGHT;

  char *pict = (char *) malloc(sizeof(char) * dim);

  memset(pict,0,dim);

  for (int vdc = 0; vdc < 2; vdc++)
    for (int plane = 0; plane < 2; plane++)
      {
	EXT_VDC_RAW *pvdc = &raw_event->VDC[vdc][plane].H;

	for (EXT_VDC_RAW_item *item = pvdc->_items_start;
	   item < pvdc->_items_end; item++)
	  {
	    int w = item->_wire;
	    int t = item->_time;

	    if (w < 0      ) w = 0;
	    if (w >= WIDTH ) w = WIDTH;
	    if (t < 0      ) t = 0;
	    if (t >= MAXT  ) t = MAXT;

	    size_t off = (vdc * 2 + plane) * 3 * WIDTH * (HEIGHT + 1);

	    char *pix = pict + off + 3 * (((t * HEIGHT) / MAXT) * WIDTH + w);
	    
	    *(pix  ) = 0;
	    *(pix+1) = 255;
	    *(pix+2) = 255;
	  }
      }

  {
    char filename[256];

    static int no = 0;
    
    sprintf (filename,"vdc_n%d.ppm",no++);
        
    FILE *fid = fopen(filename,"wb");
    
    fprintf (fid,"P6\n");
    fprintf (fid,"%d %d\n",WIDTH,PHEIGHT);
    fprintf (fid,"255\n");
    
    fwrite(pict,sizeof(char),dim,fid);
    
    fclose(fid);
    
    free(pict);
  }
#endif//VDC_PLOT
}

void esn_user_init()
{
#ifdef USE_VESNA5
  vesna5_init();
#endif

  for (int i = 0; i < 2; i++)
    _vdc_ec_sync[i].reset();

#if CORR_PICTURES
  mwpc_corr1[0].clear();
  mwpc_corr1[1].clear();
  mwpc_corr2.clear();

  vdc_corr1[0].clear();
  vdc_corr1[1].clear();
  vdc_corr2.clear();

  for (int vdc = 0; vdc < 2; vdc++)
    for (int plane = 0; plane < 2; plane++)
      vdc_raw_corr[vdc][plane].clear();

  for (int mwpc = 0; mwpc < 4; mwpc++)
    for (int plane = 0; plane < 2; plane++)
      mwpc_raw_corr[mwpc][plane].clear();
#endif

  for (int i = 0; i < 2; i++)
    the_unpack_event_map.esn.ev.vdc[i].clean_maps();

#define SET_VDC_MAP(vdc_buf,start,vdc_n,vdc_p,first) \
  the_unpack_event_map.esn.ev.vdc[(vdc_buf)].add_map((start),&_static_event._raw.VDC[(vdc_n)][(vdc_p)].H,(first))

#define VDC1_U_FIRST 0
#define VDC1_V_FIRST 0
#define VDC2_U_FIRST 0
#define VDC2_V_FIRST 0

  for (int i = 0; i < 7; i++)
    {
      SET_VDC_MAP(0,          i*32     , 0,0,-(   15+i*32 + VDC1_U_FIRST));
      SET_VDC_MAP(0,          i*32 + 16, 0,0, (16+   i*32 + VDC1_U_FIRST));

      SET_VDC_MAP(0,(0x40<<5)+i*32     , 0,1,-(   15+i*32 + VDC1_V_FIRST));
      SET_VDC_MAP(0,(0x40<<5)+i*32 + 16, 0,1,-(16+15+i*32 + VDC1_V_FIRST));

      SET_VDC_MAP(1,(0x80<<5)+i*32     , 1,0,-(   15+i*32 + VDC2_U_FIRST));
      SET_VDC_MAP(1,(0x80<<5)+i*32 + 16, 1,0, (16+   i*32 + VDC2_U_FIRST));

      SET_VDC_MAP(1,(0xc0<<5)+i*32     , 1,1,-(   15+i*32 + VDC2_V_FIRST));
      SET_VDC_MAP(1,(0xc0<<5)+i*32 + 16, 1,1,-(16+15+i*32 + VDC2_V_FIRST));

    }

  // 0 97  -> 124
  // 0 128 -> 132
  // 0 260 (5 counts...)

  // 1 32  -> 43
  // 1 64  -> 70
  // 1 128 -> 138

  for (int i = 0; i < 2; i++)
    {
      the_unpack_event_map.esn.ev.mwpc[i].clean_maps();
      the_unpack_event_map.esn.ev.mwpc[i]._mwpc_buf = i;
    }
 
#define SET_MWPC_MAP_FORW(mwpc_buf,start,end,mwpc_n,mwpc_p,offset) \
  the_unpack_event_map.esn.ev.mwpc[(mwpc_buf)].add_map((start),(end)-(start),&_static_event._raw.MWPC[(mwpc_n)][(mwpc_p)].H,(offset))

  SET_MWPC_MAP_FORW(0,  96*32   , 111*32+16, 3, 0, 1*16);
  SET_MWPC_MAP_FORW(0, 111*32+16, 125*32   , 1, 0, 1*16);
  SET_MWPC_MAP_FORW(0, 128*32   , 133*32   , 0, 1, 0*16);

  SET_MWPC_MAP_FORW(1,  32*32   ,  44*32   , 3, 1, 0*16);
  SET_MWPC_MAP_FORW(1,  64*32   ,  71*32   , 1, 1, 1*16);
  SET_MWPC_MAP_FORW(1, 128*32   , 138*32+16, 0, 0, 1*16);

  for (int i = 0; i < 2; i++)
    the_unpack_event_map.esn.ev.mwpc[i].sort_maps();
  
}

void esn_user_exit()
{
#ifdef USE_VESNA5
  vesna5_exit();
#endif

#if CORR_PICTURES
  mwpc_corr1[0].picture("mwpc0.png");
  mwpc_corr1[1].picture("mwpc1.png");
  mwpc_corr2.picture("mwpc.png");

  vdc_corr1[0].picture("vdc0.png");
  vdc_corr1[1].picture("vdc1.png");
  vdc_corr2.picture("vdc.png");

  for (int vdc = 0; vdc < 2; vdc++)
    for (int plane = 0; plane < 2; plane++)
      {
	char filename[256];
	
	sprintf (filename,"vdc_%d_%d.png",vdc,plane);

	vdc_raw_corr[vdc][plane].picture(filename);
      }

  for (int mwpc = 0; mwpc < 4; mwpc++)
    for (int plane = 0; plane < 2; plane++)
      {
	char filename[256];
	
	sprintf (filename,"mwpc_%d_%d.png",mwpc,plane);

	mwpc_raw_corr[mwpc][plane].picture(filename);
      }

  /*
  for (int m = 0; m < 2; m++)
    {
      for (int addr = 0; addr < NUM_MWPC_ADDR; addr += 32)
	{
	  double cnt = 0;

	  for (int c = 0; c < 32; c++)
	    cnt += mwpc_corr1[m]._count[addr+c];

	  if (cnt)
	    printf ("%d: %3d : %8.0f\n",
		    m,addr >> 5,cnt);
	}
    }
  */
  /*
  for (int m = 0; m < 2; m++)
    {
      for (int addr = 0; addr < NUM_MWPC_ADDR; addr++)
	{
	  if (mwpc_corr1[m]._count[addr])
	    printf ("%d: 0x%04x=%5d : %3d/%2d : %8d\n",
		    m,addr,addr,addr >> 5,addr & 0x1f,
		    mwpc_corr1[m]._count[addr]);
	}
    }
  */
#endif
  /*
  for (int m = 0; m < 2; m++)
    {
      for (int i = 0; i < NUM_MWPC_ADDR; i++)
	{
	  printf ("w %d %4d : %d\n",m,i,mwpc_count[m][i]);
	}
	  
      // before we make the picture of the correlations, we need to
      // which buckets are in use at all
      
      int used[NUM_MWPC_BUCKETS];
      
      memset(used,0,sizeof(used));
      
      for (int i = 0; i < NUM_MWPC_ADDR; i++)
	{
	  for (int b = 0; b < NUM_MWPC_BUCKETS; b++)
	    if (mwpc_corr[m][i]._ptr_offset[b] != -1)
	      used[b]++;
	}
      
      int num_used = 0;

      for (int b = 0; b < NUM_MWPC_BUCKETS; b++)
	{
	  if (used[b])
	    num_used++;

	  printf ("b %d %3d : %d\n",m,b,used[b]);
	}
      
      // The we need to allocate space for a picture, large enough...

      int dim = num_used * NUM_MWPC_CHUNK + NUM_MWPC_BUCKETS+1;

      int off[NUM_MWPC_BUCKETS];

      int next_off = 1;

      for (int b = 0; b < NUM_MWPC_BUCKETS; b++)
	{
	  if (used[b])
	    {
	      off[b] = next_off;
	      next_off += NUM_MWPC_CHUNK;
	    }
	  else
	    off[b] = 0;

	  next_off++;
	}

      printf ("dim %d  next %d\n",dim,next_off);

      char *pict = (char *) malloc(sizeof(char) * dim * dim);

      memset(pict,255,sizeof(char) * dim * dim);

      // Then we need to dump the information onto the picture

      for (int b1 = 0; b1 < NUM_MWPC_BUCKETS; b1++)
	if (off[b1])
	  {
	    fprintf(stderr,"MWPC corr %d , %d/%d    \r",m,off[b1],dim);
	    fflush(stderr);
	    
	    for (int c1 = 0; c1 < NUM_MWPC_CHUNK; c1++)
	      {
		int y = off[b1] + c1;
		
		pcorr<NUM_MWPC_BUCKETS,
		  NUM_MWPC_CHUNK> *corr = &mwpc_corr[m][b1*NUM_MWPC_CHUNK+c1];
	      	
		int cnt1 = mwpc_count[m][b1*NUM_MWPC_CHUNK+c1];
		
		for (int b2 = 0; b2 < NUM_MWPC_BUCKETS; b2++)
		  {
		    if (corr->_ptr_offset[b2] != -1)
		      {
			int *data = corr->_ptr + corr->_ptr_offset[b2] * NUM_MWPC_CHUNK;
			
			// now loop over the data in the chunk and eject it
			
			for (int c2 = 0; c2 < NUM_MWPC_CHUNK; c2++)
			  {
			    int x = off[b2] + c2;
			    
			    int corr = data[c2];
			    int cnt2 = mwpc_count[m][b2*NUM_MWPC_CHUNK+c2];
			    
			    double frac;
			    
			    //if (corr > cnt1 ||
			    //  corr > cnt2)
			    //  printf("correlation larger than count... (%d %d %d))\n",corr,cnt1,cnt2);
			    
			    if (corr && cnt1 && cnt2)
			      {
				frac = (double) corr / sqrt(cnt1 * cnt2);

				// frac will always be <= 1.0, since corr < cnt1 and corr < cnt2
				
				printf ("%d (%3d %2d) (%3d %2d) f %10.6f  %6.2f  (%5d / %5d , %5d)\n",
					m,b1,c1,b2,c2,
					frac,
					log(frac),
					corr,cnt1,cnt2);

				double fraclog = log(frac);

				if (fraclog > -10.0)
				  {
				    pict[y * dim + x] = (char) ((-fraclog / 10) * 250);
				  }
			      }
			  }
		      }		  
		  }
	      }
	  }

      char filename[256];

      sprintf (filename,"mwpc%d.pgm",m);
      
      FILE *fid = fopen(filename,"wb");

      fprintf (fid,"P5\n");
      fprintf (fid,"%d %d\n",dim,dim);
      fprintf (fid,"255\n");

      fwrite(pict,sizeof(char),dim*dim,fid);

      fclose(fid);

      // Then dump the picture (as pgm - highly inefficient, but
      // gets the job done...)


      //

      free(pict);
    }
*/
}
