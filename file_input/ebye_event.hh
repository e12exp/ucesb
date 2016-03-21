
#ifndef __EBYE_EVENT_HH__
#define __EBYE_EVENT_HH__

#define EBYE_RECORD_ID "EBYEDATA"

struct ebye_record_header
{
  uint8  _id[8];       // EBYEDATA
  uint32 _sequence;
  uint16 _tape;        // 1
  uint16 _stream;      // 1..4
  uint16 _endian_tape; // native 1 by tape writer
  uint16 _endian_data; // native 1 by data writer
  uint32 _data_length; // in bytes, header not included
};

struct ebye_event_header
{
#ifdef USE_EBYE_INPUT_16
  uint16 _start_end_token;
  uint16 _length;
#endif
#ifdef USE_EBYE_INPUT_32
  uint32 _start_end_token_length;
#endif
};

#ifdef USE_EBYE_INPUT_16
typedef uint16 ebye_data_t;
#endif
#ifdef USE_EBYE_INPUT_32
typedef uint32 ebye_data_t;
#endif

#endif//__EBYE_EVENT_HH__
