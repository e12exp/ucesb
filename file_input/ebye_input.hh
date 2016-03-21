
#ifndef __EBYE_INPUT_HH__
#define __EBYE_INPUT_HH__

#include "input_buffer.hh"
#include "decompress.hh"
#include "reclaim.hh"
#include "input_event.hh"
#include "hex_dump_mark.hh"

#include "typedef.hh"
#include <stdlib.h>

#ifdef USE_EBYE_INPUT

#include "ebye_event.hh"

//#define EBYE_RECORD_MAX_LENGTH     32764


struct ebye_event
  : public input_event
{
  ebye_event_header _header;
  // The data
  char      *_data;
  // If it was subdivided, and no data pointer gotten yet
  buf_chunk  _chunks[2];
  // Hmm, can this be handled more elegantly?
  bool       _swapping;

public:
  void get_data_src(ebye_data_t *&start,ebye_data_t *&end);
  void print_event(int data,hex_dump_mark_buf *unpack_fail) const;
};

#define FILE_INPUT_EVENT ebye_event

#ifndef USE_THREADING
extern FILE_INPUT_EVENT _file_event;
#endif

class ebye_source :
  public data_input_source
{
public:
  ebye_source();
  ~ebye_source() { }

public:
  void new_file();

public:
  ebye_record_header _record_header;
  bool               _swapping;
  uint32             _last_seq_no;

  // ebye_event_header  *_event_header;
  // uint32            *_event_header;
  // uint32            *_event_data;
  // uint32            *_event_data_end;

public:
  buf_chunk  _chunks[2];
  buf_chunk *_chunk_cur;
  buf_chunk *_chunk_end;

public:
  ebye_event *get_event(/*ebye_event *dest*/);

public:
  void print_file_header();
  void print_buffer_header(const ebye_record_header *header);
  void print_buffer_header() { } // TO BE KILLED

public:
  bool read_record();
  bool skip_record();
};

#endif//USE_EBYE_INPUT

#endif//__EBYE_INPUT_HH__

