
#ifndef __FILE_MMAP_HH__
#define __FILE_MMAP_HH__

#include "sig_mmap.hh"

#include "reclaim.hh"

#include "input_buffer.hh"
#include "thread_buffer.hh"

class file_mmap;

#ifdef USE_THREADING
struct fmm_reclaim
  : public tb_reclaim
{
  file_mmap *_mm;
  off_t      _end;
};
#endif

struct file_mmap_chunk
{
  // What parts of the file do we map
  off_t _start;
  off_t _end;
  // Where is our mapping
  char *_base;
  // Neighbour chunk
  file_mmap_chunk *_prev;
  file_mmap_chunk *_next;
  // Info for the SIGBUS catcher
  sig_mmap_info _mmap_info;
};

class file_mmap 
  : public input_buffer
{
public:
  file_mmap();
  virtual ~file_mmap();

public:
  int _fd;

public:
  size_t _page_mask;

public:
  volatile off_t _avail;
  volatile off_t _done;

  // These two members are for monitoring (only) (optimize away?)
  off_t _front; // how far have we read
  off_t _back;  // how far have we released?

  bool _last_has_eof;

public:
  file_mmap_chunk         _ends;
  /*static*/ file_mmap_chunk *_free;

public:
  void init(int fd);
  virtual void close();

public:
  virtual int map_range(off_t start,off_t end,buf_chunk chunks[2]);
  virtual void release_to(off_t end);

#ifdef USE_THREADING
  virtual void arrange_release_to(off_t end);
#endif
  
  void consistency_check();
};

#endif//__FILE_MMAP_HH
