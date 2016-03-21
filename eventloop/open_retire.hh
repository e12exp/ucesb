
#ifndef __OPEN_RETIRE_HH__
#define __OPEN_RETIRE_HH__

#include "worker_thread.hh"
#include "config.hh"
#include "decompress.hh"

class open_retire :
  public worker_thread
{
public:
  open_retire();
  virtual ~open_retire();

public:
  config_input_vect::iterator _input_iter;


public:
  void init();


public:
  bool open_file(int wakeup_this_file,
		 thread_block *block_reader);
  void close_file(data_input_source *source);
  void send_flush(int info);

public:
  virtual void *worker();



};

#endif//__OPEN_RETIRE_HH__
