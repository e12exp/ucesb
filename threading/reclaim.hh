
#ifndef __RECLAIM_HH__
#define __RECLAIM_HH__

#include "typedef.hh"

#define RECLAIM_THREAD_BUFFER_ITEM   0x0001
#define RECLAIM_MMAP_RELEASE_TO      0x0002 // implies RECLAIM_THREAD_BUFFER_ITEM
#define RECLAIM_PBUF_RELEASE_TO      0x0003 // implies RECLAIM_THREAD_BUFFER_ITEM
#define RECLAIM_MESSAGE              0x0004 // implies RECLAIM_THREAD_BUFFER_ITEM
#define RECLAIM_FORMAT_BUFFER_HEADER 0x0005 // implies RECLAIM_THREAD_BUFFER_ITEM

struct reclaim_item
{
  uint32        _type;
  reclaim_item *_next;
};

void reclaim_items(reclaim_item *first);
void print_format_reclaim_items(reclaim_item *first);

#endif//__RECLAIM_HH__
