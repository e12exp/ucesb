
#ifndef __GENF_INPUT_HH__
#define __GENF_INPUT_HH__

#include "input_buffer.hh"
#include "decompress.hh"
#include "reclaim.hh"
#include "input_event.hh"
#include "hex_dump_mark.hh"

#include "typedef.hh"
#include <stdlib.h>

#ifdef USE_GENF_INPUT

// The genf data format does not really have records.
// Treat every event as it's own record

struct genf_record_header
{
  uint16 _length;
};

typedef genf_record_header genf_event_header;

struct genf_event
  : public input_event
{
  genf_event_header _header;
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

#define FILE_INPUT_EVENT genf_event

#ifndef USE_THREADING
extern FILE_INPUT_EVENT _file_event;
#endif

class genf_source :
  public data_input_source
{
public:
  genf_source();
  ~genf_source() { }

public:
  void new_file();

public:
  int _fd;

  genf_record_header  _record_header;
  bool                _swapping;

public:
  buf_chunk  _chunks[2];
  buf_chunk *_chunk_cur;
  buf_chunk *_chunk_end;

public:
  genf_event *get_event(/*genf_event *dest*/);

public:
  void print_file_header();
  void print_buffer_header();

public:
  bool read_record();
  bool skip_record();
};

#endif//USE_GENF_INPUT

#endif//__GENF_INPUT_HH__

