
#ifndef __PAX_EVENT_HH__
#define __PAX_EVENT_HH__

#define PAX_RECORD_MAX_LENGTH     32764

#define PAX_RECORD_TYPE_UNDEF     0
#define PAX_RECORD_TYPE_STD       0x0001
#define PAX_RECORD_TYPE_STD_SWAP  0x0100

struct pax_record_header
{
  uint16 _length;
  uint16 _type;
  uint16 _seqno;
  uint16 _off_nonfrag;
};

#define PAX_EVENT_TYPE_NORMAL_MAX  31

struct pax_event_header
{
  uint16 _length;
  uint16 _type;
};

#endif//__PAX_EVENT_HH__
