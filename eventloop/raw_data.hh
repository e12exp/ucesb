
#ifndef __RAW_DATA_HH__
#define __RAW_DATA_HH__

#include "typedef.hh"
#include "error.hh"

#include "bitsone.hh"

#include "../common/signal_id.hh"

#include "enumerate.hh"
#include "pretty_dump.hh"

#include "zero_suppress_map.hh"

#include <assert.h>
#include <string.h>

//template<typename T>
//class calib_map;

template<typename T>
class data_map;

struct rawdata64
{
public:
  uint64  value;

public:
  void show_members(const signal_id &id,const char *unit) const;

  void enumerate_members(const signal_id &id,
			 const enumerate_info &info,
			 enumerate_fcn callback,void *extra) const
  { 
    callback(id,enumerate_info(info,this,ENUM_TYPE_DATA64),extra);
  }

  void dump()
  {
    printf ("0x%016llx",value);
  }

  void dump(const signal_id &id,pretty_dump_info &pdi) const;

  void __clean()
  {
    uint64* pthis = (uint64 *) (this);
    *pthis = 0;
  }

  void zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
  {
    insert_zero_suppress_info_ptrs(this,used_info);
  }

  operator uint64&() { return value; }
  operator const uint64&() const { return value; }

public:
  //uint32 *get_dest_info() { return &value; }

public:
  void map_members(const data_map<rawdata64> &map MAP_MEMBERS_PARAM) const;
};

struct rawdata32
{
public:
  uint32  value;

public:
  void show_members(const signal_id &id,const char *unit) const;

  void enumerate_members(const signal_id &id,
			 const enumerate_info &info,
			 enumerate_fcn callback,void *extra) const
  { 
    callback(id,enumerate_info(info,this,ENUM_TYPE_DATA32),extra);
  }

  void dump()
  {
    printf ("0x%08x",value);
  }

  void dump(const signal_id &id,pretty_dump_info &pdi) const;

  void __clean()
  {
    uint32* pthis = (uint32 *) (this);
    *pthis = 0;
  }

  void zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
  {
    insert_zero_suppress_info_ptrs(this,used_info);
  }

  operator uint32&() { return value; }
  operator const uint32&() const { return value; }

public:
  //uint32 *get_dest_info() { return &value; }

public:
  void map_members(const data_map<rawdata32> &map MAP_MEMBERS_PARAM) const;
};

struct rawdata24
{
public:
#if __BYTE_ORDER == __LITTLE_ENDIAN
  uint32  value    : 24;
  uint32  dummy1   : 8;
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
  uint32  dummy1   : 8;
  uint32  value    : 24;
#endif

public:
  void show_members(const signal_id &id,const char *unit) const;

  void enumerate_members(const signal_id &id,
			 const enumerate_info &info,
			 enumerate_fcn callback,void *extra) const
  { 
    callback(id,enumerate_info(info,this,ENUM_TYPE_DATA24,0,0x00ffffff),extra);
  }

  void dump()
  {
    printf ("0x%08x",value);
  }

  void dump(const signal_id &id,pretty_dump_info &pdi) const;

  void __clean()
  {
    uint32* pthis = (uint32 *) (this);
    *pthis = 0;
  }

  void zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
  {
    insert_zero_suppress_info_ptrs(this,used_info);
  }

public:
  //uint32 *get_dest_info() { return &value; }

public:
  void map_members(const data_map<rawdata24> &map MAP_MEMBERS_PARAM) const;
};

struct rawdata16
{
public:
  uint16  value;

public:
  void show_members(const signal_id &id,const char *unit) const;

  void enumerate_members(const signal_id &id,
			 const enumerate_info &info,
			 enumerate_fcn callback,void *extra) const
  { 
    callback(id,enumerate_info(info,this,ENUM_TYPE_DATA16,0,0x0000ffff),extra);
  }

  void dump()
  {
    printf ("0x%04x",value);
  }

  void dump(const signal_id &id,pretty_dump_info &pdi) const;

  void __clean()
  {
    uint16* pthis = (uint16 *) (this);
    *pthis = 0;
  }

  void zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
  {
    insert_zero_suppress_info_ptrs(this,used_info);
  }

public:
  //uint32 *get_dest_info() { return &value; }

public:
  void map_members(const data_map<rawdata16> &map MAP_MEMBERS_PARAM) const;
};

struct rawdata12
{
public:
#if __BYTE_ORDER == __LITTLE_ENDIAN
  uint16  value    : 12;
  uint16  range    : 1;
  uint16  dummy1   : 2;
  uint16  overflow : 1;
  // uint16  dummy2   : 16;
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
  // uint16  dummy2   : 16;
  uint16  overflow : 1;
  uint16  dummy1   : 2;
  uint16  range    : 1;
  uint16  value    : 12;
#endif

public:
  void show_members(const signal_id &id,const char *unit) const;

  void enumerate_members(const signal_id &id,
			 const enumerate_info &info,
			 enumerate_fcn callback,void *extra) const
  { 
    // We claim to be 12 bits, but actually have 16, due to
    // overrange and range bits... :-(
    callback(id,enumerate_info(info,this,ENUM_TYPE_DATA12,0,0x0000ffff),extra);
  }

  void dump()
  {
    printf ("0x%03x%c%c",
	    value,
	    overflow ? 'O' : ' ',
	    range ? 'R'    : ' ');
  }

  void dump(const signal_id &id,pretty_dump_info &pdi) const;

  void __clean()
  {
    uint16* pthis = (uint16 *) (this);
    *pthis = 0;
  }

  void zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
  {
    insert_zero_suppress_info_ptrs(this,used_info);
  }

public:
  //uint32 *get_dest_info() { return &value; }

public:
  void map_members(const data_map<rawdata12> &map MAP_MEMBERS_PARAM) const;
};

struct rawdata8
{
public:
  uint8   value;

public:
  void show_members(const signal_id &id,const char *unit) const;

  void enumerate_members(const signal_id &id,
			 const enumerate_info &info,
			 enumerate_fcn callback,void *extra) const
  { 
    callback(id,enumerate_info(info,this,ENUM_TYPE_DATA8,0,0x000000ff),extra);
  }

  void dump()
  {
    printf ("0x%02x",value);
  }

  void dump(const signal_id &id,pretty_dump_info &pdi) const;

  void __clean()
  {
    uint8* pthis = (uint8 *) (this);
    *pthis = 0;
  }

  void zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
  {
    insert_zero_suppress_info_ptrs(this,used_info);
  }

public:
  //uint32 *get_dest_info() { return &value; }

public:
  void map_members(const data_map<rawdata8> &map MAP_MEMBERS_PARAM) const;
};

#define DATA64           rawdata64
#define DATA32           rawdata32
#define DATA24           rawdata24
#define DATA16           rawdata16
#define DATA12           rawdata12
#define DATA12_OVERFLOW  rawdata12
#define DATA12_RANGE     rawdata12
#define DATA8            rawdata8

#endif//__RAW_DATA_HH__
