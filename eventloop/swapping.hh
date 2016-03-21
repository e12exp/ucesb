#ifndef __SWAPPING_HH__
#define __SWAPPING_HH__

#include "byteswap_include.h"
#include "byteorder_include.h"

#include <assert.h>
#include <stdlib.h>

#include <sys/types.h>

#if BYTE_ORDER == BIG_ENDIAN
# define HOST_ENDIAN_TYPE(x)  x##_big_endian
#endif
#if BYTE_ORDER == LITTLE_ENDIAN
# define HOST_ENDIAN_TYPE(x)  x##_little_endian
#endif

inline uint32 bswap(uint32 src) { return bswap_32(src); }
inline uint16 bswap(uint16 src) { return bswap_16(src); }
inline uint8  bswap(uint8 src)  { return src; } // just to make template happy...

template<typename T>
inline void byteswap(T* ptr,size_t len)
{
  assert (len % sizeof(T) == 0);

  uint* p = (uint *) ptr;

  for (size_t i = len / sizeof(T); i; i--)
    {
      *p = bswap(*p);
      p++;
    }
}

#endif//__SWAPPING_HH__
