#ifndef __RIDF_INPUT_HH__
#define __RIDF_INPUT_HH__

#ifdef USE_RIDF_INPUT

#include "input_buffer.hh"
#include "decompress.hh"
#include "input_event.hh"
#include "hex_dump_mark.hh"

#include "ridf_event.hh"

#define RIDF_EVENT_LOCATE_SUBEVENTS_ATTEMPT  0x0004

// See https://ribf.riken.jp/RIBFDAQ/, especially
// https://ribf.riken.jp/RIBFDAQ/index.php?plugin=attach&refer=DAQ%2FManual%2FDataformat&openfile=dataformat_101112e.pdf

struct ridf_subevent
{
  struct ridf_subevent_header _header;

  char *_data;
  buf_chunk *_frag;
  size_t _offset;
};

// Keeps track of maximum memory-size of subevents in an event.
struct ridf_event_hint
{
  ridf_event_hint()
  {
    _max_subevents = 1;
    _max_subevents_since_decrease = 0;
    _hist_req_realloc = 0;
  }

public:
  int _max_subevents;
  int _max_subevents_since_decrease;
  uint _hist_req_realloc;
};

struct ridf_event
  : public input_event
{
  ridf_event_header _header;

  int        _status;

  ridf_subevent *_subevents;
  int           _nsubevents;

  buf_chunk *_chunk_end;
  buf_chunk *_chunk_cur;
  size_t     _offset_cur;
  
#ifndef USE_THREADING
public:
  keep_buffer_many _defrag_event_many;

public:
  void release()
  {
    input_event::release();
    _defrag_event_many.release();
  }
#endif

  // As opposed to LMD events, this must not necessarily be the last
  // members, as we at most have two chunks

  buf_chunk  _chunks[2];

public:
  void locate_subevents(ridf_event_hint *hints);
  void get_subevent_data_src(ridf_subevent *subevent_info,
			     char *&start, char *&end);
  void print_event(int data, hex_dump_mark_buf *unpack_fail) const;
};

#define FILE_INPUT_EVENT ridf_event

#ifndef USE_THREADING
extern FILE_INPUT_EVENT _file_event;
#endif

class ridf_source :
  public data_input_source
{
public:
  ridf_source();
  ~ridf_source() { }
  ridf_event *get_event();

public:
  void new_file();
  bool _is_new_file;

public:
  void print_buffer_header(ridf_header const *);
};

#endif // USE_RIDF_INPUT

#endif // __RIDF_INPUT_HH__
