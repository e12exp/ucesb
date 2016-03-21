
#include "esn_external.hh"
#include "error.hh"

#include "external_data.hh"

#include "worker_thread.hh"
#include "thread_buffer.hh"

/*---------------------------------------------------------------------------*/

EXT_MWPC::EXT_MWPC()
{
  _items_start = NULL;
  _items_end = NULL;
  _items_alloc_end = NULL;
}

void EXT_MWPC::__clean()
{
  fifo.__clean();
#if 0
  _valid.clear();
#endif
  _items_end = _items_start;
}

void EXT_MWPC::set_channels(uint16 full_address,uint16 channels)
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

      EXT_MWPC_item *alloc = (EXT_MWPC_item *)
	realloc(_items_start,sizeof (EXT_MWPC_item) * alloc_size);

      if (!alloc)
	ERROR("Memory allocation failure for MWPC_item");

      _items_end = alloc + (_items_end - _items_start);

      _items_start = alloc;
      _items_alloc_end   = _items_start + alloc_size;
#endif
    }

  EXT_MWPC_item *item = _items_end++;
  
  item->_start = full_address;
  item->_end   = (uint16) (full_address + channels);
}

#if 0
void EXT_MWPC::set_channel(int logical,int channel)
{
  uint32 prev = 0;

  if (_valid.get_set(logical))
    prev = _hits[logical];

  _hits[logical] = prev | (((uint32) 1) << channel);
}

void EXT_MWPC::set_channels(int first_logical,int first_channel,int channels)
{
  while (first_channel + channels > 32)
    {
      // The channels do not fit within this module.
      // First dump as many as fits

      int ch_fit = 32 - first_channel;

      uint32 prev = 0;
      
      if (_valid.get_set(first_logical))
	prev = _hits[first_logical];
      
      _hits[first_logical] = prev | ((((uint32) (1 << ch_fit)) - 1) << first_channel);

      //printf ("(%d/%d:%d)",first_logical,first_channel,ch_fit);

      first_logical++;
      first_channel = 0;
      channels -= ch_fit;
    }

  if (channels)
    {
      // The channels fit within this module!

      uint32 prev = 0;
      
      if (_valid.get_set(first_logical))
	prev = _hits[first_logical];
      
      _hits[first_logical] = prev | ((((uint32) (1 << channels)) - 1) << first_channel);

      //printf ("(%d/%d:%d)",first_logical,first_channel,channels);
    }
}
#endif

/* For the MWPC we will destroy the work done by the PCOS camac
 * controller in calculating widths and mean position of many wires
 * fired and instead fill an bit-array of which wires have fired.
 *
 * This is necessary in cases when there have been inverted cables or
 * other non-continous cabling, which the PCOS has been unaware of.
 *
 * A later stage in the analysis will then reassemble the information about
 * stretches of wires that fired.
 */

#define PCOS_WIDTH_IDENTIFIER 0x8000

#if __BYTE_ORDER == __LITTLE_ENDIAN
union PCOS_width 
{
  struct
  {
    uint16 width      : 4;
    uint16 unused     : 11;
    uint16 identifier : 1;
  };
  uint16 u16;
};

union PCOS_address 
{
  struct
  {
    uint16 halfwire   : 1;
    uint16 address    : 5;
    uint16 logical    : 9;
    uint16 identifier : 1;
  };
  uint16 u16;
};
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
union PCOS_width 
{
  struct
  {
    uint16 identifier : 1;
    uint16 unused     : 11;
    uint16 width      : 4;
  };
  uint16 u16;
};

union PCOS_address 
{
  struct
  {
    uint16 identifier : 1;
    uint16 logical    : 9;
    uint16 address    : 5;
    uint16 halfwire   : 1;
  };
  uint16 u16;
};
#endif

EXT_DECL_DATA_SRC_FCN(void,EXT_MWPC::__unpack)
{
  fifo.__unpack_header(__buffer);

  //printf ("---\n");

  int left = fifo.get_wc();

  if (!left)
    goto done_unpack_mwpc; // no data

#ifdef USE_THREADING
  // Allocate enough entries for us to survive even if it sends
  // all wires in their own data entry
#define EXT_MWPC_ALLOC_ITEMS 4096 // guess
  _items_start = (EXT_MWPC_item *) 
    _wt._defrag_buffer->allocate_reclaim(EXT_MWPC_ALLOC_ITEMS * sizeof(EXT_MWPC_item));
  _items_end = _items_start;
  _items_alloc_end = _items_start + EXT_MWPC_ALLOC_ITEMS;
#endif
  
  while (left > 0)
    {
      uint16 w;

      GET_BUFFER_UINT16(w);

      //printf ("MWPC: [%04x] ",w);

      if (w & PCOS_WIDTH_IDENTIFIER)
	{
	  PCOS_width width;

	  width.u16 = w;

	  if (width.unused)
	    {
#ifdef USE_THREADING
	      // Adjust allocated size down
	      _wt._defrag_buffer->allocate_adjust_end(_items_start,
						      _items_alloc_end,_items_end);
#endif
	      ERROR("PCOS width had unused bits set. (w=%04x)",
		    width.u16);
	    }

	  PCOS_address address;

	  GET_BUFFER_UINT16(address.u16);

	  if (address.identifier)
	    {
#ifdef USE_THREADING
	      // Adjust allocated size down
	      _wt._defrag_buffer->allocate_adjust_end(_items_start,
						      _items_alloc_end,_items_end);
#endif
	      ERROR("PCOS address expected, found width word. (w=%04x,a=%04x)",
		    width.u16,address.u16);
	    }
	  /*
	  printf ("[%04x] width/address: [%3d] [%2d] %d w=%d  (%4d)",
		  address.u16,
		  address.logical,
		  address.address,
		  address.halfwire,
		  width.width,
		  (address.logical * 32 + address.address - (width.width-1) / 2));
	  */
	  if (!((address.halfwire ^ width.width) & 1))
	    {
#ifdef USE_THREADING
	      // Adjust allocated size down
	      _wt._defrag_buffer->allocate_adjust_end(_items_start,
						      _items_alloc_end,_items_end);
#endif
	      ERROR("Halfwire (%d) does not agree with width (%d).",
		    address.halfwire,width.width);
	    }
	  /*
	  if (width.width-1 > (address.address*2+address.halfwire))
	    printf (" l");
	  if ((address.address*2+address.halfwire)+width.width-1 > 63)
	    printf (" h");
	  */
	  uint16 full_address =
	    (uint16) (address.logical * 32 + address.address -
		      (width.width-1) / 2);

	  if (full_address >= 512*32)
	    {
#ifdef USE_THREADING
	      // Adjust allocated size down
	      _wt._defrag_buffer->allocate_adjust_end(_items_start,
						      _items_alloc_end,_items_end);
#endif
	      ERROR("full_address out of range (%d>%d)",full_address,512*32);
	    }

#if 0
	  set_channels(full_address / 32,full_address % 32,width.width);
#endif
	  set_channels(full_address,width.width);

	  left -= 2; // read two words
	}
      else
	{
	  PCOS_address address;

	  address.u16 = w;
	  /*
	  printf ("             address: [%3d] [%2d] %d w=1  (%4d)",
		  address.logical,
		  address.address,
		  address.halfwire,
		  (address.logical * 32 + address.address));
	  */
	  if (address.halfwire)
	    {
#ifdef USE_THREADING
	      // Adjust allocated size down
	      _wt._defrag_buffer->allocate_adjust_end(_items_start,
						      _items_alloc_end,_items_end);
#endif
	      ERROR("Halfwire cannot be set with implicit width 1.");
	    }

#if 0
	  set_channel(address.logical,address.address);
#endif
	  uint16 full_address =
	    (uint16) (address.logical * 32 + address.address);

	  set_channels(full_address,1);

	  // Now, the data seems to look like that for all cases where
	  // there is no width specifier (i.e. width = 1), then there
	  // are always two equal address words after each other.
	  // Check that, and eat the second word
	  
	  uint16 w2;
	  GET_BUFFER_UINT16(w2);

	  if (w2 != w)
	    WARNING("Second (dummy) address word mismatch (%04x!=%04x)",
		    w,w2);

	  //i--; // read 1 word
	  left -= 2; // read two words (also dummy)
	}

      //printf ("\n");
    }

#ifdef USE_THREADING
  // Adjust allocated size down
  _wt._defrag_buffer->allocate_adjust_end(_items_start,
					  _items_alloc_end,_items_end);
#endif
  
 done_unpack_mwpc:

  fifo.__unpack_footer(__buffer);
}
EXT_FORCE_IMPL_DATA_SRC_FCN(void,EXT_MWPC::__unpack);

/*---------------------------------------------------------------------------*/

void EXT_MWPC_map::clean_maps()
{
  _num_maps = 0;

  for (int i = 0; i < MAX_MWPC_LOOKUP_MAPS; i++)
    _maps[i] = NULL;

  _maps[MAX_MWPC_LOOKUP_MAPS] = NULL; // dummy, to allow +1 (next) lookup
}

void EXT_MWPC_map::add_map(uint16 start,sint16 channels,EXT_MWPC_RAW *dest,uint16 offset)
{
  if (_num_maps >= MAX_MWPC_MAPS)
    ERROR("Too many MWPC maps.");

  EXT_MWPC_map_item &map = _map[_num_maps++];

  map._start    = start;
  map._end      = (uint16) (start + channels);
  map._dest     = dest;
  map._offset   = offset;
  map._direction = channels;
}

/*---------------------------------------------------------------------------*/

int compare_EXT_MWPC_map_item(const void *p1,const void *p2)
{
  const EXT_MWPC_map_item *i1 = (const EXT_MWPC_map_item *) p1;
  const EXT_MWPC_map_item *i2 = (const EXT_MWPC_map_item *) p2;

  return i1->_start - i2->_start;
}

/*---------------------------------------------------------------------------*/

void EXT_MWPC_map::sort_maps()
{
  /* Make sure the maps are sorted.
   * also join any maps that are neighbours.
   */

  qsort(_map,_num_maps,sizeof (EXT_MWPC_map_item),compare_EXT_MWPC_map_item);

  /* Now that they are sorted, we can just loop over the array...
   * Join any items that are adjacent.  However, if the destination
   * is reverse order, 
   */

  /* We do not look for overlaps at the output side of the map.
   */

  for (int i = 0; i < _num_maps-1; )
    {
      EXT_MWPC_map_item &map1 = _map[i];
      EXT_MWPC_map_item &map2 = _map[i+1];

      if (map2._start < map1._end)
	{
	  ERROR("MWPC maps overlapping, %d(%d:%d) < %d(%d:%d)",
		map2._start,map2._start >> 5,map2._start & 0x1f,
		map1._end,map1._end >> 5,map1._end & 0x1f);
	}
      else if (map2._start  == map1._end && // adjacent at input
	       map2._offset == map1._offset + map1._direction && // adjacent at output
	       (map2._direction > 0) == (map1._direction > 0)) // same direction
	{
	  map1._end       = map2._end;
	  map1._direction = (uint16) (map1._direction + map2._direction);

	  memmove(&_map[i+1],&_map[i+2],_num_maps-i-2 * sizeof (EXT_MWPC_map_item));
	}
      else
	i++;
    }

  /* Make sure the outputs are not overlapping (or map is bad).
   * Hmm, that would have to go by destination array...
   */

  // Add dummy map as last item, which is guaranteed to not match
  // anyone (not included in lookup).  Used in the mapping, to
  // not have to check that we do not go outside _num_maps.
  _map[_num_maps]._start = 0x7fff;
  _map[_num_maps]._end   = 0x7fff;  

  // Setup the fast lookup tables.  Each look-up table entry points
  // to the first map that can come into question.  They are in order,
  // so any second entry should not be set)

  for (int i = 0; i < _num_maps; i++)
    {
      EXT_MWPC_map_item *map1 = &_map[i];

      int lookup_start = map1->_start >> MWPC_LOOKUP_MAP_SHIFT;
      int lookup_end   = 
	(map1->_end + MWPC_LOOKUP_MAP_CHUNK-1) >> MWPC_LOOKUP_MAP_SHIFT;

      // Only the first lookup item may already be in use
      
      if (!_maps[lookup_start])
	_maps[lookup_start] = map1;

      for (int lookup = lookup_start+1; lookup < lookup_end; lookup++)
	{
	  assert(!_maps[lookup]);
	  _maps[lookup] = map1;
	}
    }
}

/*---------------------------------------------------------------------------*/

void EXT_MWPC_map::map_members(const struct EXT_MWPC &src MAP_MEMBERS_PARAM) const
{
  // Loop through all the hits that we have in the modules,
  // and map them to detectors
#if 0
  bitsone_iterator iter;
  int i;

  while ((i = src._valid.next(iter)) >= 0)
    {
      // _hits[i] has some bit one

      printf ("%d ",i);
    }
  printf ("\n");
#endif

  /* Loop over the hits in the modules, and map them to detectors.
   */

  for (EXT_MWPC_item *item = src._items_start; item != src._items_end; ++item)
    {
      int full_address = item->_start;
      int end_address  = item->_end;

      /* Find the mapping item that maps the first channel in our bunch
       */
      
      int lookup = full_address >> MWPC_LOOKUP_MAP_SHIFT;
      
      const EXT_MWPC_map_item *map_item = _maps[lookup];
      
      if (UNLIKELY(!map_item))
	{
	  // There is no map for this wire...  Since the end may
	  // never be further away than 16 wires from the start,
	  // we must never look more than into also the next item.
	  // (to handle the end of the data that may have a map)
	  
	  // Do that also...
	  
	  map_item = _maps[lookup+1];
	  
	  if (!map_item)
	    {
	      // No map there either.  Forget it!
	      /*
	      WARNING("Data for unmapped MPWC wires (%d: %5d(%3d:%3d)-%5d(%3d:%3d)) n.",
		      map._mwpc_buf,
		      full_address,full_address >> 5,full_address & 0x1f,
		      end_address, end_address  >> 5,end_address  & 0x1f);
	      */
	      continue;
	    }
	}
      
      // We may have gotten a too early map...
      
      
      while (UNLIKELY(map_item->_end < full_address))
	map_item++;
      
      // The map will no be starting either before (at or
      // somewhere with us) us, or it starts after us (in which
      // case we have no map)
      
    try_next_map:
      if (map_item->_start >= end_address)
	{
	  // got a map completely after us	      
	  /*
	  WARNING("Data for unmapped MPWC wires (%d: %5d(%3d:%3d)-%5d(%3d:%3d)) a.",
		  map._mwpc_buf,
		  full_address,full_address >> 5,full_address & 0x1f,
		  end_address, end_address  >> 5,end_address  & 0x1f);
	  */
	  continue;
	}
      
      // The map is somewhere within our data?
      
      if (map_item->_start > full_address)
	{
	  /*
	  WARNING("Data for unmapped MPWC wires (%d: %5d(%3d:%3d)-%5d(%3d:%3d)) s.",
		  map._mwpc_buf,
		  full_address,full_address >> 5,full_address & 0x1f,
		  item->_start,item->_start >> 5,item->_start & 0x1f);
	  */
	  // We need to skip some channels that are unmapped
	  
	  // skip = map_item->_start - full_address;
	  full_address  = map_item->_start;
	}
      
      // Now we are aligned.  Map as many channels as are both
      // fired, and in the map full_address is now the starting
      // address
      
      if (UNLIKELY(map_item->_end < end_address))
	{
	  // We are limited by the end of the map, and not what fired.  We'll
	  // need to look up the next map.  
	  
	  map_item->_dest->set_hits(map_item->_offset + full_address - map_item->_start,
				map_item->_end - full_address,
				map_item->_direction);
	  
	  full_address = map_item->_end; // cut away until after this map
	  
	  // See if the next map can help
	  
	  map_item++;
	  goto try_next_map;
	}	  
      else
	{
	  // No further mapping will be needed.  I.e. we go to end_address
	  
	  map_item->_dest->set_hits(map_item->_offset + full_address - map_item->_start,
				end_address - full_address,
				map_item->_direction);
	}
    }

  /* Now, one should loop over the detectors, and make sure that the
   * hits are properly joined together...
   *
   * These calls however need to be done per detector, and not per
   * module!  So they are done after the mapping...
   */
}

/*---------------------------------------------------------------------------*/

EXT_MWPC_RAW::EXT_MWPC_RAW()
{
  _items_start = NULL;
  _items_end = NULL;
  _items_alloc_end = NULL;
}

/*---------------------------------------------------------------------------*/

void EXT_MWPC_RAW::__clean()
{
  _items_end = _items_start;
}

/*---------------------------------------------------------------------------*/

void EXT_MWPC_RAW::set_hits(int first,
			    int length,
			    int direction)
{
  if (_items_end >= _items_alloc_end)
    {
      /* We must first reallocate. */

      size_t current_size = _items_alloc_end - _items_start;
      size_t alloc_size = current_size ? current_size * 2 : 16;

      EXT_MWPC_RAW_item *alloc = (EXT_MWPC_RAW_item *) 
	realloc(_items_start,sizeof (EXT_MWPC_RAW_item) * alloc_size);

      if (!alloc)
	ERROR("Memory allocation failure for MWPC_HIT_item");

      _items_end = alloc + (_items_end - _items_start);

      _items_start = alloc;
      _items_alloc_end   = _items_start + alloc_size;
    }

  EXT_MWPC_RAW_item *item = _items_end++;

  if (direction > 0)
    {
      item->_start = (uint16) (first);
      item->_end   = (uint16) (first + length);
    }
  else
    {
      item->_start = (uint16) (first - length + 1);
      item->_end   = (uint16) (first + 1);
    }
}

/*---------------------------------------------------------------------------*/

int compare_EXT_MWPC_RAW_item(const void *p1,const void *p2)
{
  int start1 = ((const EXT_MWPC_RAW_item*) p1)->_start;
  int start2 = ((const EXT_MWPC_RAW_item*) p2)->_start;

  return start1 - start2;
}

void EXT_MWPC_RAW::sort_hits()
{
  // The sorting and combing of the hits is done in order to
  // regularize the hits.  The most important is the combination of
  // the hits, since this affect the number of 'particles' that are
  // expected to have hit the detectors.  But this requires the
  // sorting...

  qsort(_items_start,
	_items_end-_items_start,sizeof(EXT_MWPC_RAW_item),
	compare_EXT_MWPC_RAW_item);

  // Now coalesce any hits that are adjacent...  As this should
  // generally not happen, we use two loops, first that just checks,
  // if it finds anything, we go to the slower loop that actually
  // bunches things togther

  // printf ("%d hits\n",_items_end-_items_start);

  sint16 last_end = -1;

  for (EXT_MWPC_RAW_item *item = _items_start; item < _items_end; item++)
    {
      if (UNLIKELY(item->_start <= last_end))
	goto coalesce;
      last_end = item->_end;
    }

  return; // no adjacent hits

 coalesce:
  /*
  printf ("Coalescing...\n");
  for (EXT_MWPC_RAW_item *item = _items_start; item < _items_end; item++)
    printf ("  %d,%d",item->_start,item->_end);
  printf ("\n");
  */
  last_end = -1;

  EXT_MWPC_RAW_item *dest = _items_start;

  for (EXT_MWPC_RAW_item *src  = _items_start; src < _items_end; src++)
    {
      if (UNLIKELY(src->_start <= last_end))
	{
	  // item belongs to previous

	  (dest-1)->_end = src->_end;
	}
      else
	{
	  // Item does not belong to previous

	  *(dest++) = *src;
	}
      last_end = src->_end;
    }
  _items_end = dest;

  /*
  for (EXT_MWPC_RAW_item *item = _items_start; item < _items_end; item++)
    printf ("  %d,%d",item->_start,item->_end);
  printf ("\n");
  */
}

/*---------------------------------------------------------------------------*/
