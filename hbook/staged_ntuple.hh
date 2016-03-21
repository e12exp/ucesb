
#ifndef __STAGED_NTUPLE_HH__
#define __STAGED_NTUPLE_HH__

#include "hbook.hh"
#include "writing_ntuple.hh"

#include "external_writer.hh"

#include <vector>

struct staged_ntuple_named_str
{
  std::string _id;
  std::string _str;
};

struct fill_raw_info;

typedef void(*fill_raw_fnc)(fill_raw_info *);

struct fill_raw_info
{
  uint32_t      _words;
  uint32_t     *_ptr;
  fill_raw_fnc  _callback;
  void         *_extra;
};

class staged_ntuple
{
public:
  staged_ntuple();
  ~staged_ntuple();

public:
  external_writer  *_ext;
  reader_src_external _ext_r; // in progress (reading)

public:
  indexed_item  _global_array;  // rwn & cwn
  index_item   *_index_item;    // cwn
  uint32_t      _entries_index; // cwn
  array_item   *_array_item;    // cwn
  uint32_t      _entries_array; // cwn
  /*index2_item  *_index2_item;    // cwn
    int           _entries_index2; // cwn*/
  array2_item  *_array2_item;    // cwn
  uint32_t      _entries_array2; // cwn
  
  size_t        _array_entries;
  
  uint _ntuple_type;

  char   *_id;     // of ntuple (root: name)
  char   *_title;  // of ntuple
  char   *_ftitle; // of file (hbook: file top)

  std::vector<staged_ntuple_named_str> _named_strs;

  int  _struct_server_port;

  int  _timeslice;
  int  _timeslice_subdir;
  int  _autosave;

public:
  void open(const char *filename);
  void stage(vect_ntuple_items &list,int hid,void *base,
	     uint sort_u32_words = 0, uint max_raw_words = 0);
  void event(void *base,uint *sort_u32 = NULL,
	     fill_raw_info *fill_raw = NULL);
  bool get_event();
  void unpack_event(void *base);
  void close();
};

#endif//__STAGED_NTUPLE_HH__
