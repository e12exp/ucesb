
#ifndef __EVENT_READER_HH__
#define __EVENT_READER_HH__

#include "worker_thread.hh"

#include "lmd_input.hh"
#include "pax_input.hh"
#include "genf_input.hh"
#include "ebye_input.hh"
#include "ridf_input.hh"

class data_input_source;

class event_reader :
  public worker_thread
{
public:
  virtual ~event_reader();

public:
  // The record/event reader
#ifdef USE_LMD_INPUT
  lmd_source _reader;
#endif
#ifdef USE_PAX_INPUT
  pax_source _reader;
#endif
#ifdef USE_GENF_INPUT
  genf_source _reader;
#endif
#ifdef USE_EBYE_INPUT
  ebye_source _reader;
#endif
#ifdef USE_RIDF_INPUT
  ridf_source _reader;
#endif

public:
  virtual void *worker();

public:
  void process_file(data_input_source *source);

  void wait_for_unpack_queue_slot();


};

#endif//__EVENT_READER_HH__
