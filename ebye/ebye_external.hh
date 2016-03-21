
#ifndef __HACKY_EXTERNAL_HH__
#define __HACKY_EXTERNAL_HH__

/*---------------------------------------------------------------------------*/

#include "external_data.hh"

/*---------------------------------------------------------------------------*/

class EXT_EBYE_DATA
{
public:
  EXT_EBYE_DATA();

public:
  int _group_item[0x100][0x40];
  int _group_data[0x100];
  int _ext_group_data[0x10000];

public:
  void __clean();
  EXT_DECL_UNPACK(/*_ARG:any arguments*/);

public:
  DUMMY_EXTERNAL_DUMP(EXT_EBYE_DATA);
  DUMMY_EXTERNAL_SHOW_MEMBERS(EXT_EBYE_DATA);
  DUMMY_EXTERNAL_ENUMERATE_MEMBERS(EXT_EBYE_DATA);
  DUMMY_EXTERNAL_ZERO_SUPPRESS_INFO_PTRS(EXT_EBYE_DATA);
};

DUMMY_EXTERNAL_MAP_STRUCT(EXT_EBYE_DATA);
DUMMY_EXTERNAL_WATCHER_STRUCT(EXT_EBYE_DATA);
DUMMY_EXTERNAL_CORRELATION_STRUCT(EXT_EBYE_DATA);

/*---------------------------------------------------------------------------*/

#endif//__CROS3_EXTERNAL_HH__
