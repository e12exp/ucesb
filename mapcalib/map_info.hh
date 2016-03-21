
#ifndef __MAP_INFO_HH__
#define __MAP_INFO_HH__

#include "../common/node.hh"

#include "signal_id_map.hh"

#include "file_line.hh"

struct map_info
  : public def_node
{
public:
  virtual ~map_info() { }

public:
  map_info(const file_line &loc,
	   const signal_id_info *src,const signal_id_info *dest,
	   const signal_id_info *rev_src,const signal_id_info *rev_dest)
  {
    _loc  = loc;

    _src  = src;
    _dest = dest;

    _rev_src  = rev_src;
    _rev_dest = rev_dest;
  }

public:
  const signal_id_info   *_src;
  const signal_id_info   *_dest;

  const signal_id_info   *_rev_src;
  const signal_id_info   *_rev_dest;

public:
  file_line _loc;
};

int data_type(const char *type);

#endif//__MAP_INFO_HH__
