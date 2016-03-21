
#ifndef __PAX_INPUT_HH__
#define __PAX_INPUT_HH__

#include "input_buffer.hh"
#include "decompress.hh"
#include "input_event.hh"
#include "hex_dump_mark.hh"

#include "typedef.hh"
#include <stdlib.h>

#ifdef USE_PAX_INPUT

#include "pax_event.hh"

struct pax_event
  : public input_event
{
  pax_event_header _header;
  // The data
  char      *_data;
  // If it was subdivided, and no data pointer gotten yet
  buf_chunk  _chunks[2];
  // Hmm, can this be handled more elegantly?
  bool       _swapping;

public:
  void get_data_src(uint16 *&start,uint16 *&end);
  void print_event(int data,hex_dump_mark_buf *unpack_fail) const;
};

#define FILE_INPUT_EVENT pax_event

#ifndef USE_THREADING
extern FILE_INPUT_EVENT _file_event;
#endif

class pax_source :
  public data_input_source
{
public:
  pax_source();
  ~pax_source() { }

public:
  void new_file();

public:
  pax_record_header  _record_header;
  bool               _swapping;
  int                _last_seq_no;

  // pax_event_header  *_event_header;
  // uint16            *_event_data;
  // uint16            *_event_data_end;

public:
  buf_chunk  _chunks[2];
  buf_chunk *_chunk_cur;
  buf_chunk *_chunk_end;

public:
  pax_event *get_event(/*pax_event *dest*/);

public:
  void print_file_header();
  void print_buffer_header();

public:
  bool read_record();
  bool skip_record();
};

#endif//USE_PAX_INPUT

#endif//__PAX_INPUT_HH__
