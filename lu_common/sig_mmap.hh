
#ifndef __SIG_MMAP_HH__
#define __SIG_MMAP_HH__

#include <stdlib.h>
#include <sys/types.h>

struct sig_mmap_info
{
  void   *_addr;
  size_t  _length;
  int     _fd;
  off_t   _offset;

  sig_mmap_info *_next;
};

void sig_register_mmap(sig_mmap_info *info,
		       void *addr, size_t length, int fd, off_t offset);
void sig_unregister_mmap(sig_mmap_info *info,
			 void *addr, size_t length);

#endif//__SIG_MMAP_HH__
