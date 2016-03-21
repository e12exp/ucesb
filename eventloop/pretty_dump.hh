
#ifndef __PRETTY_DUMP_HH__
#define __PRETTY_DUMP_HH__

#include "../common/signal_id.hh"

struct pretty_dump_info
{
  bool _dump_nan;

};

void pretty_dump(const signal_id &id,const char *buf,
		 pretty_dump_info &pdi);

#endif//__PRETTY_DUMP_HH__
