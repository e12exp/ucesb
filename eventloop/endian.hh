
#ifndef __ENDIAN_HH__
#define __ENDIAN_HH__

#include "byteorder_include.h"

#ifndef __BYTE_ORDER
# if defined(_BYTE_ORDER)
#  define __BYTE_ORDER     _BYTE_ORDER
#  define __LITTLE_ENDIAN  _LITTLE_ENDIAN
#  define __BIG_ENDIAN     _BIG_ENDIAN
# elif defined(BYTE_ORDER)
#  define __BYTE_ORDER     BYTE_ORDER
#  define __LITTLE_ENDIAN  LITTLE_ENDIAN
#  define __BIG_ENDIAN     BIG_ENDIAN
# else
#  ifndef DEPSRUN
#   error __BYTE_ORDER not defined
#  endif
# endif
#endif

#endif//__ENDIAN_HH__
