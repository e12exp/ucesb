#ifndef __BYTESWAP_INCLUDE_H__
#define __BYTESWAP_INCLUDE_H__

#ifndef ACC_DEF_RUN
# include "gen/acc_auto_def/byteswap_include.h"
#endif

#if ACC_DEF_BYTESWAP_INCLUDE_byteswap_h
# include "byteswap.h"
#endif
#if ACC_DEF_BYTESWAP_INCLUDE_endian_h
# include "endian.h"
#endif
#if 0 /* so far unused variants - kept for start to search... */
#if ACC_DEF_BYTESWAP_INCLUDE_sys_endian_h
# include "sys/endian.h"
#endif
#if ACC_DEF_BYTESWAP_INCLUDE_machine_endian_h
# include "machine/endian.h"
#endif
#if ACC_DEF_BYTESWAP_INCLUDE_machine_endian_h
# include "machine/byte_order.h"
#endif
#if ACC_DEF_BYTESWAP_INCLUDE_machine_endian_h
# include "endian.h"
# include "byteswap.h"
#endif
#if ACC_DEF_BYTESWAP_INCLUDE_machine_bswap_h
#include <sys/types.h>
#include <machine/bswap.h>
#endif
#if ACC_DEF_BYTESWAP_INCLUDE_sys_types_h
/* OpenBSD ?? */
#include <sys/types.h>
#endif
#endif
#if ACC_DEF_BYTESWAP_INCLUDE_mybyteswap
/* All else failed, we do it ourselves.  Non-optimal! */
#define bswap_16(x)				\
  (unsigned short int) ((((x) >> 8) & 0x00ff) |	\
			(((x) << 8) & 0xff00))
#define bswap_32(x)				\
  ((((x) >> 24) & 0x000000ff) |			\
   (((x) >>  8) & 0x0000ff00) |			\
   (((x) <<  8) & 0x00ff0000) |			\
   (((x) << 24) & 0xff000000))
#endif

#ifndef bswap_16
#if 0 /* so far unused variants - kept for start to search... */
/* may need to move inte separate rules if functions and not macros */
# if defined(bswap16) /* || defined(__NetBSD__) */
#  define bswap_16(x) bswap16(x)
#  define bswap_32(x) bswap32(x)
# endif
# if defined(swap16) /* || defined(__OpenBSD__) */
#  define bswap_16(x) swap16(x)
#  define bswap_32(x) swap32(x)
# endif
# if defined(__DARWIN_OSSwapInt16) /* defined(__APPLE__) */
#  define bswap_16(x) __DARWIN_OSSwapInt16(x)
#  define bswap_32(x) __DARWIN_OSSwapInt32(x)
# endif
#endif
#endif

#ifdef ACC_DEF_RUN
#ifndef bswap_16
# error bswap_16 not defined
#endif
#ifndef bswap_32
# error bswap_32 not defined
#endif
void acc_test_func()
{
  (void) bswap_16(1);
  (void) bswap_32(1);
}
#endif

#endif/*__BYTESWAP_INCLUDE_H__*/
