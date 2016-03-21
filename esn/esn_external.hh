
#ifndef __ESN_EXTERNAL_HH__
#define __ESN_EXTERNAL_HH__

#include "data_src.hh"

#include "bitsone.hh"

#include "dummy_external.hh"

#include "external_data.hh"

#include "sync_check_info.hh"

/*---------------------------------------------------------------------------*/

class EXT_FIFO
{
public:
  uint16 header;
  uint16 event_counter;

public:
  int get_wc()
  {
    return (header & 0x1fff) - 1; 
  }
  
public:
  void __clean();
  EXT_DECL_DATA_SRC_FCN(void,__unpack_header);
  EXT_DECL_DATA_SRC_FCN(void,__unpack_footer);
};

/*---------------------------------------------------------------------------*/

DUMMY_EXTERNAL_MAP_STRUCT_FORW(EXT_MWPC);

struct EXT_MWPC_item
{
  uint16 _start;
  uint16 _end;
};

class EXT_MWPC
{
public:
  EXT_MWPC();

public:
  EXT_FIFO fifo;

public:
#if 0
  // We store the bits of all channels in all possible logicals
  // that has been hit
  uint32 _hits[512];
  // To cut down on the work of finding who was hit (and not have
  // to loop over 512 entries) we also have a bitmasks telling who
  // of the _hits has any non-zero values.  (I.e. _hits[i] is not
  // valid, unless _valid.get(i) says so)
  bitsone<512> _valid;
#endif
  EXT_MWPC_item *_items_start;
  EXT_MWPC_item *_items_end;
  EXT_MWPC_item *_items_alloc_end;

public:
#if 0
  void set_channel(int logical,int channel);
  void set_channels(int first_logical,int first_channel,int channels);
#endif
  void set_channels(uint16 full_address,uint16 channels);

public:
  void __clean();
  EXT_DECL_UNPACK(/*_ARG:any arguments*/);
  // Needed if it is part of a select statement (or first in subevent)
  // bool __match(__data_src &__buffer /*,any arguments*/);

public:
  DUMMY_EXTERNAL_DUMP(EXT_MWPC);
  DUMMY_EXTERNAL_SHOW_MEMBERS(EXT_MWPC);
  DUMMY_EXTERNAL_ENUMERATE_MEMBERS(EXT_MWPC);
  DUMMY_EXTERNAL_ZERO_SUPPRESS_INFO_PTRS(EXT_MWPC);
};

DUMMY_EXTERNAL_WATCHER_STRUCT(EXT_MWPC);
DUMMY_EXTERNAL_CORRELATION_STRUCT(EXT_MWPC);

/*---------------------------------------------------------------------------*/

class EXT_MWPC_RAW;

struct EXT_MWPC_map_item
{
  uint16         _start;
  uint16         _end;
  EXT_MWPC_RAW  *_dest;
  uint16         _offset;
  uint16         _direction; // _channels or -_channels for reverse order map
};

#define MAX_MWPC_MAPS 32

// Let's do it quickly by doing the maps in chunks of 16
#define MWPC_LOOKUP_MAP_SHIFT     4
#define MWPC_LOOKUP_MAP_CHUNK     (1 << MWPC_LOOKUP_MAP_SHIFT)
#define MAX_MWPC_LOOKUP_MAPS      ((512*32)/MWPC_LOOKUP_MAP_CHUNK)

class EXT_MWPC_map
{
public:
  EXT_MWPC_map_item  _map[MAX_MWPC_MAPS+1]; // +1 dummy map
  int                _num_maps;

  // There are at most 16384 wires, the PCOS modules each take in 32
  // channels of data.  Let's use a per 32-channel lookup table to
  // quickly find the correctmap.  That is 512 pointers.  
  //
  // Hmm, the cables from the detectors are in bunches of 16, so let's
  // use that instead...  (unused parts of the pointer array will
  // anyhow not be in cache)

  EXT_MWPC_map_item *_maps[MAX_MWPC_LOOKUP_MAPS+1];
// +1 map to allow lookup also in the next item, if no hit in first

  int _mwpc_buf; // for debug messages

public:
  void add_map(uint16 start,sint16 channels,EXT_MWPC_RAW *dest,uint16 offset);
  void sort_maps();
  void clean_maps();

public:
  void map_members(const struct EXT_MWPC &src MAP_MEMBERS_PARAM) const;

  DUMMY_EXTERNAL_MAP_ENUMERATE_MAP_MEMBERS(EXT_MWPC);
};

/*---------------------------------------------------------------------------*/

struct EXT_MWPC_RAW_item
{
  uint16 _start;
  uint16 _end;
};

class EXT_MWPC_RAW
{
public:
  EXT_MWPC_RAW();

public:
  EXT_MWPC_RAW_item *_items_start;
  EXT_MWPC_RAW_item *_items_end;
  EXT_MWPC_RAW_item *_items_alloc_end;

public:
  void set_hits(int first,int length,int direction);
  void sort_hits();

public:
  void __clean();

public:
  DUMMY_EXTERNAL_DUMP(EXT_MWPC_RAW);
  DUMMY_EXTERNAL_SHOW_MEMBERS(EXT_MWPC_RAW);
  DUMMY_EXTERNAL_ENUMERATE_MEMBERS(EXT_MWPC_RAW);
  DUMMY_EXTERNAL_ZERO_SUPPRESS_INFO_PTRS(EXT_MWPC_RAW);
};

DUMMY_EXTERNAL_MAP_STRUCT(EXT_MWPC_RAW);
DUMMY_EXTERNAL_CALIB_MAP_STRUCT(EXT_MWPC_RAW);
DUMMY_EXTERNAL_WATCHER_STRUCT(EXT_MWPC_RAW);
DUMMY_EXTERNAL_CORRELATION_STRUCT(EXT_MWPC_RAW);

/*---------------------------------------------------------------------------*/

DUMMY_EXTERNAL_MAP_STRUCT_FORW(EXT_VDC);

struct EXT_VDC_item
{
  uint32 _full_address : 13; // address:8 channel:5

  uint32 _edge         : 1;
  uint32 _time         : 10;
};

class EXT_VDC
{
public:
  EXT_VDC();

public:
  EXT_FIFO fifo;
  sync_check_info<0x100,3> _sync_info;

public:
  // A bitmask telling what TDC has seen any data at all
  // bitsone<256> _valid;
  EXT_VDC_item *_items_start;
  EXT_VDC_item *_items_end;
  EXT_VDC_item *_items_alloc_end;

public:
  void __clean();
  EXT_DECL_UNPACK(/*_ARG:any arguments*/);
  // Needed if it is part of a select statement
  // bool __match(__data_src &__buffer /*,any arguments*/);

public:
  DUMMY_EXTERNAL_DUMP(EXT_VDC);
  DUMMY_EXTERNAL_SHOW_MEMBERS(EXT_VDC);
  DUMMY_EXTERNAL_ENUMERATE_MEMBERS(EXT_VDC);
  DUMMY_EXTERNAL_ZERO_SUPPRESS_INFO_PTRS(EXT_VDC);
};

DUMMY_EXTERNAL_WATCHER_STRUCT(EXT_VDC);
DUMMY_EXTERNAL_CORRELATION_STRUCT(EXT_VDC);

/*---------------------------------------------------------------------------*/

class EXT_VDC_RAW;

struct EXT_VDC_map_item
{
  EXT_VDC_RAW   *_dest;
  int            _wire; // negative for inverse wire ordering
};

// Let's do it quickly by doing the maps in chunks of 16
#define MAX_VDC_MAPS      512
#define MAX_VDC_MAP_CHUNK 16

class EXT_VDC_map
{
public:
  EXT_VDC_map_item  _map[MAX_VDC_MAPS];

public:
  void add_map(int start,EXT_VDC_RAW *dest,int wire);
  void clean_maps();

public:
  void map_members(const struct EXT_VDC &src MAP_MEMBERS_PARAM) const;

  DUMMY_EXTERNAL_MAP_ENUMERATE_MAP_MEMBERS(EXT_VDC);
};

/*---------------------------------------------------------------------------*/

struct EXT_VDC_RAW_item
{
  uint32 _wire         : 13;

  uint32 _edge         : 1;
  uint32 _time         : 10;
};

class EXT_VDC_RAW
{
public:
  EXT_VDC_RAW();

public:
  EXT_VDC_RAW_item *_items_start;
  EXT_VDC_RAW_item *_items_end;
  EXT_VDC_RAW_item *_items_alloc_end;

public:
  void set_hit(int wire,const EXT_VDC_item *src);
  void sort_hits();

public:
  void __clean();

public:
  DUMMY_EXTERNAL_DUMP(EXT_VDC_RAW);
  DUMMY_EXTERNAL_SHOW_MEMBERS(EXT_VDC_RAW);
  DUMMY_EXTERNAL_ENUMERATE_MEMBERS(EXT_VDC_RAW);
  DUMMY_EXTERNAL_ZERO_SUPPRESS_INFO_PTRS(EXT_VDC_RAW);
};

DUMMY_EXTERNAL_MAP_STRUCT(EXT_VDC_RAW);
DUMMY_EXTERNAL_CALIB_MAP_STRUCT(EXT_VDC_RAW);
DUMMY_EXTERNAL_WATCHER_STRUCT(EXT_VDC_RAW);
DUMMY_EXTERNAL_CORRELATION_STRUCT(EXT_VDC_RAW);

/*---------------------------------------------------------------------------*/

DUMMY_EXTERNAL_MAP_STRUCT_FORW(EXT_SCI);

class EXT_SCI
{
public:
  EXT_FIFO fifo;

public:
  void __clean();
  EXT_DECL_UNPACK(/*_ARG:any arguments*/);
  // Needed if it is part of a select statement
  // bool __match(__data_src &__buffer /*,any arguments*/);

public:
  DUMMY_EXTERNAL_DUMP(EXT_SCI);
  DUMMY_EXTERNAL_SHOW_MEMBERS(EXT_SCI);
  DUMMY_EXTERNAL_ENUMERATE_MEMBERS(EXT_SCI);
  DUMMY_EXTERNAL_ZERO_SUPPRESS_INFO_PTRS(EXT_SCI);
};

DUMMY_EXTERNAL_MAP_STRUCT(EXT_SCI);
DUMMY_EXTERNAL_WATCHER_STRUCT(EXT_SCI);
DUMMY_EXTERNAL_CORRELATION_STRUCT(EXT_SCI);

/*---------------------------------------------------------------------------*/

#endif//__ESN_EXTERNAL_HH__
