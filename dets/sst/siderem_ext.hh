
#ifndef __SIDEREM_EXTERNAL_HH__
#define __SIDEREM_EXTERNAL_HH__

#include "data_src.hh"
#include "dummy_external.hh"
#include "external_data.hh"

/*---------------------------------------------------------------------------*/

struct EXT_SIDEREM_stretch
{
  int     _start;
  int     _end;
  // int _offset;
  // instead of having an offset, we store a direct pointer into the array
  // provided by the enclosing data structure (_data)
  uint32* _data;
};

template<int n>
struct EXT_SIDEREM_plane
{
public:
  int                 _num_stretches;
  EXT_SIDEREM_stretch _stretches[n];
  uint32              _data[n];
  uint32             *_next_data; // only used while unpacking!

  // The number of stretches are way too many, they could be (n+1)/2
  // if we are assured that adjacent strectches are always merged
  // together, and could be (n+1)/(m+1) if we would know that each
  // stretch contain at least m data values.  This way we are at least
  // safe...!

  // The data values are not indexed according to the strips, but
  // indexed with the _offset from the stretch.  This has the advantage
  // of not trashing more cache than necessary between events

public:
  void __clean() { _num_stretches = 0; }
  EXT_DECL_UNPACK_ARG(int start,int end);

  void setup_unpack() { _next_data = &_data[0]; }
};

class EXT_SIDEREM
{
public:
  EXT_SIDEREM() { }

public:
  EXT_SIDEREM_plane<640> _x;
  EXT_SIDEREM_plane<384> _y;

public:
  void __clean();
  EXT_DECL_UNPACK(/*_ARG:any arguments*/);

};

/*---------------------------------------------------------------------------*/

DUMMY_EXTERNAL_WATCHER_STRUCT(EXT_SIDEREM);

#endif//__SIDEREM_EXTERNAL_HH__
