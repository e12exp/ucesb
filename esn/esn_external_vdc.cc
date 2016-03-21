
#include "esn_external.hh"
#include "error.hh"

#include "external_data.hh"

#include "worker_thread.hh"
#include "thread_buffer.hh"

/*---------------------------------------------------------------------------*/

EXT_VDC::EXT_VDC()
{
  _items_start = NULL;
  _items_end = NULL;
  _items_alloc_end = NULL;
}

void EXT_VDC::__clean()
{
  fifo.__clean();
  _sync_info.clear();
  _items_end = _items_start;
}

#define TDC3377_HEADER_IDENTIFIER 0x8000
#define TDC3377_HEADER_RES_MASK   0x0300
#define TDC3377_HEADER_EDGES      0x0400
#define TDC3377_HEADER_DW         0x4000

#define TDC3377_HEADER_RES_500ps  0x0000
#define TDC3377_HEADER_RES_1ns    0x0100
#define TDC3377_HEADER_RES_2ns    0x0200
#define TDC3377_HEADER_RES_4ns    0x0300

#if __BYTE_ORDER == __LITTLE_ENDIAN
union TDC3377_header 
{
  struct
  {
    uint16 address      : 8;
    uint16 resolution   : 2;
    uint16 both_edges   : 1;
    uint16 event_number : 3;
    uint16 double_word  : 1;
    uint16 identifier   : 1;
  };
  uint16 u16;
};

union TDC3377_data0 
{
  struct
  {
    uint16 time         : 10;
    uint16 channel      : 5;
    uint16 identifier   : 1;
  };
  uint16 u16;
};

union TDC3377_data1
{
  struct
  {
    uint16 time         : 9;
    uint16 edge         : 1;
    uint16 channel      : 5;
    uint16 identifier   : 1;
  };
  uint16 u16;
};
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
union TDC3377_header 
{
  struct
  {
    uint16 identifier   : 1;
    uint16 double_word  : 1;
    uint16 event_number : 3;
    uint16 both_edges   : 1;
    uint16 resolution   : 2;
    uint16 address      : 8;
  };
  uint16 u16;
};

union TDC3377_data0 
{
  struct
  {
    uint16 identifier   : 1;
    uint16 channel      : 5;
    uint16 time         : 10;
  };
  uint16 u16;
};

union TDC3377_data1
{
  struct
  {
    uint16 identifier   : 1;
    uint16 channel      : 5;
    uint16 edge         : 1;
    uint16 time         : 9;
  };
  uint16 u16;
};
#endif

#pragma GCC diagnostic ignored "-Wconversion"
EXT_DECL_DATA_SRC_FCN(void,EXT_VDC::__unpack)
{
  fifo.__unpack_header(__buffer);

  // printf ("---\n");

  TDC3377_header header;

  int address_offset = 0;

  int left = fifo.get_wc();

  if (!left)
    {
      fifo.__unpack_footer(__buffer);
      return;
    }

  // we have some data
  // the first data word must be a header
  
  uint16 w;
  
  GET_BUFFER_UINT16(w);

  if (!(w & TDC3377_HEADER_IDENTIFIER))
    ERROR("First word not a header. (%04x)",w);

  // take the event counter as the first header (other ones must match)

  header.u16 = w;
  left--;

  _sync_info.add(header.address,
		 header.event_number); // cannot fail (1) since we're first!
  /*  
  printf ("header: address=%3d res=%d edges=%d ec=%d dw=%d*\n",
	  header.address,
	  header.resolution,
	  header.both_edges,
	  header.event_number,
	  header.double_word);
  */

#ifdef USE_THREADING
  // reserve enough memory to hold all wires hit.  VSN*ch*hits
  // 256*32*16=128K items We do _anyhow_ need to check against
  // overflow, since the hits may be so unordered and broken that they
  // appear too many times, and thus can be more.  On the other hand,
  // they are limited by the PAX event size (64K data words)...  Hmm A
  // VDC has 2x256 wires or something such, with at most 3 hits per
  // wire.  That would be 512*3=1.5k items.  Let's be too generous
  // anyhow and give 4k items.  In any case, it is only reserved
  // during the execution of this routine.  Afterwards (before we
  // return), we give all unused space back!  Except in case of an
  // error...

#define EXT_VDC_ALLOC_ITEMS 4096
  _items_start = (EXT_VDC_item *) 
    _wt._defrag_buffer->allocate_reclaim(EXT_VDC_ALLOC_ITEMS * sizeof(EXT_VDC_item));
  _items_end = _items_start;
  _items_alloc_end = _items_start + EXT_VDC_ALLOC_ITEMS;
#endif

  // This is really ugly:
  goto header_after_ec_check;

  while (left--)
    {
      GET_BUFFER_UINT16(w);
      
      //printf ("VDC: [%04x] ",w);
      
      if (w & TDC3377_HEADER_IDENTIFIER)
	{
	  header.u16 = w;
	  /*	  
	  printf ("header: address=%3d res=%d edges=%d ec=%d dw=%d\n",
		  header.address,
		  header.resolution,
		  header.both_edges,
		  header.event_number,
		  header.double_word);
	  */
	  // check that res = 1 and dw = 0
	  // we also check edges = 0... (but this one could think may change)
	  /*
	  if (header.event_number != event_counter)
	    WARNING("VDC event counter mismatch within fifo (%d != %d) [%d]",
		    header.event_number,event_counter,
		    header.address);
	  */

	  if (_sync_info.add(header.address,
			     header.event_number))
	    {
#ifdef USE_THREADING
	      // Adjust allocated size down
	      _wt._defrag_buffer->allocate_adjust_end(_items_start,
						      _items_alloc_end,_items_end);
#endif
	      ERROR("Multiple headers for VDC (0x%04x)",w);
	    }

	header_after_ec_check:
	  
	  if ((w & (TDC3377_HEADER_RES_MASK | 
		    TDC3377_HEADER_EDGES |
		    TDC3377_HEADER_DW)) != TDC3377_HEADER_RES_1ns)
	    {
#ifdef USE_THREADING
	      // Adjust allocated size down
	      _wt._defrag_buffer->allocate_adjust_end(_items_start,
						      _items_alloc_end,_items_end);
#endif
	      ERROR("VDC header with unexpectec res, dw or edge (0x%04x)",w);
	    }
	  
	  // printf ("%04x : %04x\n",header.event_number);
	  
	  address_offset = header.address << 5;
	}
      else
	{
	  int time, edge, channel;
	  
	  if (header.both_edges) // TODO: is this really correct (cases switched?!?)
	    {
	      TDC3377_data0 d0;
	      
	      d0.u16  = w;
	      
	      time    = d0.time;
	      edge    = 0;
	      channel = d0.channel;
	    }
	  else
	    {
	      TDC3377_data1 d1;
	      
	      d1.u16  = w;
	      
	      time    = d1.time;
	      edge    = d1.edge;
	      channel = d1.channel;
	    }
	  
	  // printf ("data: [%2d] %d (%d)\n",channel,time,edge);
	  
	  {
	    if (_items_end >= _items_alloc_end)
	      {
#ifdef USE_THREADING
		// No need to readjust :-)
		ERROR("Too many items!");
#else
		/* We must first reallocate. */
		
		size_t current_size = _items_alloc_end - _items_start;
		size_t alloc_size = current_size ? current_size * 2 : 16;
		
		EXT_VDC_item *alloc = (EXT_VDC_item *)
		  realloc(_items_start,sizeof (EXT_VDC_item) * alloc_size);
		
		if (!alloc)
		  ERROR("Memory allocation failure for VDC_item");
		
		_items_end = alloc + (_items_end - _items_start);
		
		_items_start = alloc;
		_items_alloc_end   = _items_start + alloc_size;
#endif
	      }
	    
	    EXT_VDC_item *item  = _items_end++;
	    
	    memset(item,0,sizeof(EXT_VDC_item));
	    
	    /* Hmmm, gcc __typeof accepts the bitfield...
	     * Likely to bite back!  Good luck!
	     */
	    item->_full_address = /*(__typeof(item->_full_address))*/
	      (address_offset + channel);
	    item->_time         = /*(__typeof(item->_time))*/ time;
	    item->_edge         = /*(__typeof(item->_edge))*/ edge;
	  }
	  
	}
      
      //printf ("\n");
    }

#ifdef USE_THREADING
  // Adjust allocated size down
  _wt._defrag_buffer->allocate_adjust_end(_items_start,
					  _items_alloc_end,_items_end);
#endif

  fifo.__unpack_footer(__buffer);
}
EXT_FORCE_IMPL_DATA_SRC_FCN(void,EXT_VDC::__unpack);
#pragma GCC diagnostic warning "-Wconversion"

/*---------------------------------------------------------------------------*/

void EXT_VDC_map::add_map(int start,EXT_VDC_RAW *dest,int wire)
{
  int i = start / MAX_VDC_MAP_CHUNK;

  EXT_VDC_map_item *map = &_map[i];

  map->_dest = dest;
  map->_wire = wire;

  // printf ("add map %4d:%3d (%3d)\n",start,i,wire);
}

/*---------------------------------------------------------------------------*/

void EXT_VDC_map::clean_maps()
{
  for (int i = 0; i < MAX_VDC_MAPS; i++)
    {
      _map[i]._dest = NULL;
      _map[i]._wire = 0;
    }
}

/*---------------------------------------------------------------------------*/

EXT_VDC_RAW::EXT_VDC_RAW()
{
  _items_start = NULL;
  _items_end = NULL;
  _items_alloc_end = NULL;
}

/*---------------------------------------------------------------------------*/

void EXT_VDC_RAW::__clean()
{
  _items_end = _items_start;
}

/*---------------------------------------------------------------------------*/

int compare_EXT_VDC_RAW_item(const void *p1,const void *p2)
{
  int wire1 = ((const EXT_VDC_RAW_item*) p1)->_wire;
  int wire2 = ((const EXT_VDC_RAW_item*) p2)->_wire;

  int diff = wire1 - wire2;

  if (diff)
    return diff;

  int time1 = ((const EXT_VDC_RAW_item*) p1)->_time;
  int time2 = ((const EXT_VDC_RAW_item*) p2)->_time;

  return time1 - time2;
}

void EXT_VDC_RAW::sort_hits()
{
  qsort(_items_start,
	_items_end-_items_start,sizeof(EXT_VDC_RAW_item),
	compare_EXT_VDC_RAW_item);
}

/*---------------------------------------------------------------------------*/

#pragma GCC diagnostic ignored "-Wconversion"
void EXT_VDC_RAW::set_hit(int wire,const EXT_VDC_item *src)
{
  if (_items_end >= _items_alloc_end)
    {
      /* We must first reallocate. */

      size_t current_size = _items_alloc_end - _items_start;
      size_t alloc_size = current_size ? current_size * 2 : 16;

      EXT_VDC_RAW_item *alloc = (EXT_VDC_RAW_item *) 
	realloc(_items_start,sizeof (EXT_VDC_RAW_item) * alloc_size);

      if (!alloc)
	ERROR("Memory allocation failure for VDC_RAW_item");

      _items_end = alloc + (_items_end - _items_start);

      _items_start = alloc;
      _items_alloc_end   = _items_start + alloc_size;
    }

  EXT_VDC_RAW_item *item = _items_end++;

  memset(item,0,sizeof(EXT_VDC_RAW_item));

  /* typecasts to make compiler happy, casting down to the smallest
   * unsigned size larger than the target type.
   */
  item->_wire = /*(__typeof(item->_wire))*/ (wire);
  item->_time = src->_time;
  item->_edge = src->_edge;
}
#pragma GCC diagnostic warning "-Wconversion"

/*---------------------------------------------------------------------------*/

void EXT_VDC_map::map_members(const struct EXT_VDC &src MAP_MEMBERS_PARAM) const
{
  // size_t items = _items_end - _items_start;

  /* Make sure we have enough memory for all the items.  For the
   * unlikely cases where some wire does not get mapped we do not care
   * about having allocated just a bit too much memory.
   */




  /* Loop over the hits in the modules, and map them to detectors.
   */

  for (EXT_VDC_item *item = src._items_start; item != src._items_end; ++item)
    {
      // Find the corresponding map

      int i = item->_full_address / MAX_VDC_MAP_CHUNK;

      const EXT_VDC_map_item *m = &_map[i];

      int offset = item->_full_address % MAX_VDC_MAP_CHUNK;

      EXT_VDC_RAW *vdc = m->_dest;

      int wire = m->_wire + offset;

      if (!vdc)
	{
	  WARNING("Data for unmapped VDC wire (%d=%d,%d)",
		  item->_full_address,
		  i,offset);
	}
      else
	{
	  //printf ("fa:%4d i:%3d off:%2d w:%3d -> %3d\n",item->_full_address,i,offset,m->_wire,wire);
	  //fflush(stdout);

	  if (wire >= 0)
	    vdc->set_hit(wire,item);
	  else
	    vdc->set_hit(-(wire),item);
	}
    }

  /* Now, one should loop over the detectors, and make sure that the
   * hits are sorted...
   *
   * These calls however need to be done per detector, and not per
   * module!
   */
}

/*---------------------------------------------------------------------------*/
