
#ifndef __PIPE_BUFFER_HH__
#define __PIPE_BUFFER_HH__

#include "input_buffer.hh"
#include "thread_buffer.hh"

#include "thread_block.hh"

#include <pthread.h>

class pipe_buffer_base;

#ifdef USE_THREADING
struct pbf_reclaim
  : public tb_reclaim
{
  pipe_buffer_base *_pb;
  off_t             _end;
};
#endif

class pipe_buffer_base
  : public input_buffer
{
public:
  pipe_buffer_base();
  virtual ~pipe_buffer_base();

public:
  char  *_buffer;

  size_t _size;  // must be power of 2 (for & (_size -1) to work)

  // Please note: _avail and _done must be size_t (and not e.g. off_t)
  // since otherwise might reading and writing them not be atomic,
  // which screws up our syncronisation.  As long as the arguments to
  // map_range (start,end), only move slowly compared to the range
  // of _avail and _done, there is no problem, as we do all calculations
  // with clamped differences

  // We have two choices.  Either make _avail, _done (and
  // _wakeup_avail and _wakeup_done) size_t or ssize_t.  Making them
  // size_t avoids a bunch of casts relative to _size, and would in
  // principle make sense.  However, quite some comparisons are
  // working both positive and negative, and ssize_t is safer for that
  // (as explicit casting then is not necessary at those locations.)

  // Going for size_t...

  IF_USE_PTHREAD(volatile) size_t _avail; /* Total amount of data read
					   * (available) (may wrap).
					   */
  IF_USE_PTHREAD(volatile) size_t _done;  /* Total amount of data processed
					   * (released) (may wrap).
					   */

  // This member is for monitoring (only) (optimize away?)
  size_t _front; // How far have we requested.

  IF_USE_PTHREAD(volatile) bool   _reached_eof;

#ifdef USE_PTHREAD
public:
  pthread_t _thread;
  // pthread_t _thread_consumer;

  bool _active; // has thread

public:
  volatile const thread_block *_need_reader_wakeup;
  volatile const thread_block *_need_consumer_wakeup;

  volatile size_t _wakeup_done;
  volatile size_t _wakeup_avail;

  thread_block  _block;
  thread_block *_block_reader;

public:
  static void *reader_thread(void *us);
  virtual void *reader() = 0;
#else
public:
  virtual int read_now(off_t end) = 0;
#endif

public:
  void init(unsigned char *push_magic,size_t push_magic_len,
	    size_t bufsize
#ifdef USE_PTHREAD
	    ,thread_block *block_reader,bool create_thread
#endif
	    );
  virtual void close();

  void realloc(size_t bufsize);

public:
  virtual int map_range(off_t start,off_t end,buf_chunk chunks[2]);
  virtual void release_to(off_t end);

#ifdef USE_THREADING
  virtual void arrange_release_to(off_t end);
#endif

};

class pipe_buffer
  : public pipe_buffer_base
{
public:
  pipe_buffer();
  virtual ~pipe_buffer();

public:
  int _fd;

#ifdef USE_PTHREAD
public:
  virtual void *reader();
#else
public:
  virtual int read_now(off_t end);
#endif

public:
  void init(int fd,unsigned char *push_magic,size_t push_magic_len,
	    size_t bufsize
#ifdef USE_PTHREAD
	    ,thread_block *block_reader
#endif
	    );
  virtual void close();

public:

};

#endif//__PIPE_BUFFER_HH__
