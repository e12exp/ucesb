
#ifndef __TCP_PIPE_BUFFER_HH__
#define __TCP_PIPE_BUFFER_HH__

#include "pipe_buffer.hh"

class lmd_input_tcp;

class tcp_pipe_buffer
  : public pipe_buffer_base
{
public:
  tcp_pipe_buffer();
  virtual ~tcp_pipe_buffer() { }

public:
  lmd_input_tcp *_server;

#ifdef USE_PTHREAD
public:
  virtual void *reader();
#else
public:
  virtual int read_now(off_t end);
#endif

public:
  void init(lmd_input_tcp *server,size_t bufsize
#ifdef USE_PTHREAD
	    ,thread_block *block_reader
#endif
	    );
  virtual void close();


};

#endif//__TCP_PIPE_BUFFER_HH__
