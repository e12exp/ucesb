#ifndef __RIDF_EVENT_HH__
#define __RIDF_EVENT_HH__

struct ridf_header
{
  uint32 _code;
  uint32 _address;
};

struct ridf_event_header
{
  struct ridf_header _header;
  uint32 _number;
};

struct ridf_subevent_header
{
  struct ridf_header _header;
  uint32 _id;
};

#endif // __RIDF_EVENT_HH__
