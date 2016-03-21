#include "structures.hh"

#include "user.hh"
#include "optimise.hh"

//#include "mc_def.hh"
char *argv0_replace(const char *filename);

#include "convert_picture.hh"

#include "huffman.c"


#define PICT_SIZE 96
#define PICT_SIDE  6

#define PICT_LINE_SIZE (PICT_SIZE*PICT_SIDE+(PICT_SIDE-1))
#define PICT_ARRAY_SIZE (PICT_LINE_SIZE)*(PICT_LINE_SIZE)

char *_pict;
int   _npict = 0;

void make_rich_picture(unpack_event *event)
{
  if (_npict == 0)
    {
      memset(_pict,255,PICT_ARRAY_SIZE);
      
      for (int i = 1; i < PICT_SIDE; i++)
	{
	  for (int j = 0; j < PICT_LINE_SIZE; j++)
	    {
	      *(_pict + (i * (PICT_SIZE + 1) - 1) + j * PICT_LINE_SIZE) = 128;
	      *(_pict + (i * (PICT_SIZE + 1) - 1) * PICT_LINE_SIZE + j) = 128;
	    }
	}
    }

  int py = _npict;

  int n = 0;

  for (int i = 0; i < 12; i++)
    {
      int sector = i / 2;

      int px = sector;
      
      char *start = _pict + 
	px * (PICT_SIZE + 1) + 
	py * (PICT_SIZE + 1) * PICT_LINE_SIZE;
      
      rich_rawdata_word *d = event->rich[i].data.data._items;

      for (int j = event->rich[i].data.data._num_items; j; --j, d++)
	{
	  uint16 cpmc = d->cpmc;

	  rich_map_xy_item &xy = _rich_map._upi2xy._upif[cpmc];

	  // printf ("%5d: %2d %2d        ",cpmc,xy._x,xy._y);
 
	  *(start + xy._x + xy._y * PICT_LINE_SIZE) = 0; // 128 - (d->value >> 3);

	  n++;
	}
      
      /*
      for (int i = pads->_n; i; --i, pad++)
	{
	  int x, y;
	  
	  if (pad->_index > 4095)
	    continue;
	  
	  y = padXY[pad->_index][0] - 8;
	  x = padXY[pad->_index][1] - 8;
	  
	  *(start + x + y * PICT_LINE_SIZE) = 15*ilog2(pad->_value);
	}
      */
    }

  if (n > 60)
    {
      printf ("n: %d\n",n);
      _npict++;
    }

  // printf ("%d / %d...\n",_npict,(PICT_SIDE * PICT_SIDE));

  if (_npict == (PICT_SIDE))
    {
      char filename[256];
      static int richpict = 0;

      sprintf (filename,"rich_%03d.png",richpict++);

      convert_picture(filename,_pict,PICT_LINE_SIZE,PICT_LINE_SIZE);

      _npict = 0;
    }

}







template<typename T_value,int n>
struct masked_values
{
  uint32  mask[(n+31)/32];
  T_value values[n];

  int     num_values;
  T_value values_sorted[n];

  void clear() { memset (mask,0,sizeof(mask)); }

  uint32 set(int i,T_value value)
  {
    int shift  = i % (sizeof(uint32) * 8);
    int offset = i / (sizeof(uint32) * 8);

    uint32 bit = ((uint32) 1) << (shift);
    uint32 *m  = &mask[offset];

    values[i] = value;
    
    uint32 prev = *m;
    *m |= bit;
    return prev & bit;
  }

  void make_list()
  {
    num_values = 0;
    int base_index = 0;

    for (size_t off = 0; off < countof(mask); off++, base_index += 32)
      if (mask[off])
	{
	  // this mask knows about at least one value

	  uint32 m = mask[off];

	  int index = base_index;

	  while (m)
	    {
	      while (!(m & 0x000000ff)) { m >>= 8; index += 8; }
	      if    (!(m & 0x0000000f)) { m >>= 4; index += 4; }
	      if    (!(m & 0x00000003)) { m >>= 2; index += 2; }
	      if    (!(m & 0x00000001)) { m >>= 1; index += 1; }

	      m >>= 1;
	      values_sorted[num_values++] = values[index++];
	    }	  	  
	}

  }
};



uint32 freqdiff_sector[0x400];
uint32 freqdiff_module[0x400];




#define PADS_PER_SECTOR     4712
#define PADS_PER_MODULE       64
#define MODULES_PER_SECTOR    75

void compress_rich_sector(RICH_SUBEV *subev,
			  int sector)
{
  masked_values<uint16,PADS_PER_SECTOR> v_sector;
  masked_values<uint16,PADS_PER_MODULE> v_module[128];

  v_sector.clear();
  for (int j = 0; j < 128; j++)
    v_module[j].clear();

  for (int i = 0; i < 2; i++)
    {
      rich_rawdata_word *d = subev[i].data.data._items;
      
      for (int j = subev[i].data.data._num_items; j; --j, d++)
	{
	  if ((int) d->sector != sector)
	    ERROR("Sector (%d) mismath from subevent (%d).",d->sector,sector);
	  if (d->dummy != 0)
	    WARNING("Upper unused bits non-zero.");

	  uint16 cpmc = d->cpmc; 

	  rich_map_reindex_item &reindex = _rich_map._upi2index._upif[cpmc];

	  if (reindex._index_sector == (uint16) -1)
	    ERROR("Non-existing/connected channel.");
	  /*
	  printf ("%3d : %2d | %4d\n",
		  d->cpm,reindex._index_module,
		  reindex._index_sector);
	  */
	  if (v_module[d->cpm].set(reindex._index_module,d->value))
	    WARNING("Pad seen twice in module (%3d:reindex %d).",
		    d->cpm,reindex._index_module);
	  if (v_sector.set(reindex._index_sector,d->value))
	    WARNING("Pad seen twice in sector (reindex %d).",
		    reindex._index_sector);

	  // printf ("(%d)",d->value);
	}
    }

  // ok, so now we have 'sorted' all the data.  both within module,
  // and within the entire sector.  make the compacted list of values
  // for each module

  v_sector.make_list();

  for (int j = 0; j < 128; j++)
    v_module[j].make_list();

  // So, we now have the data in a mask, and an accompanying list with
  // the values themselves

  /*
  printf ("%d :",v_sector.num_values);
  for (int i = 0; i < v_sector.num_values; i++)
    printf (" %d",v_sector.values_sorted[i]);
  printf ("\n");
  for (int i = 0; i < countof(v_sector.mask); i++)
    printf (" %08x",v_sector.mask[i]);
  printf ("\n");
  */

  uint16 lastval = 0;
  for (int i = 0; i < v_sector.num_values; i++)
    {
      uint16 val = v_sector.values_sorted[i];
      freqdiff_sector[(val - lastval) & 0x3ff]++;
      lastval = val;
    }

  for (int j = 0; j < 128; j++)
    {
      uint16 lastval = 0;
      for (int i = 0; i < v_module[j].num_values; i++)
	{
	  uint16 val = v_module[j].values_sorted[i];
	  freqdiff_module[(val - lastval) & 0x3ff]++;
	  lastval = val;
	}
    }


}





void compress_rich(unpack_event *event)
{
  for (int i = 0; i < 12; i += 2)
    compress_rich_sector(event->rich+i,i/2);
}







void user_function(unpack_event *event,
		   raw_event    *raw_event,
		   cal_event    *cal_event)
{

  // make_rich_picture(event);

  compress_rich(event);


}

void user_init()
{
  _pict = (char*) malloc(PICT_ARRAY_SIZE);

  char *filename;

  filename = argv0_replace("xy2upi.dat");
  _rich_map.setup(filename);
  free (filename);

  /*
  for (int i = 0; i < 12; i++)
    {
      rich[i]._fixed.u32     = 0;
      rich[i]._fixed._sector = i / 2;
    }  
  */

  memset(freqdiff_sector,0,sizeof(freqdiff_sector));
  memset(freqdiff_module,0,sizeof(freqdiff_module));
}

void user_exit()
{
  free(_pict);

  optimize_huff(freqdiff_sector,0x400);
  optimize_huff(freqdiff_module,0x400);

  optimize_huff_length(freqdiff_sector,0x400,15);
  optimize_huff_length(freqdiff_module,0x400,15);
};

