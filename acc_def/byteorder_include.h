#ifndef __BYTEORDER_INCLUDE_H__
#define __BYTEORDER_INCLUDE_H__

#ifndef ACC_DEF_RUN
# include "gen/acc_auto_def/byteorder_include.h"
#endif

#if ACC_DEF_BYTEORDER_INCLUDE_byteorder_h
# include "byteorder.h"
#endif
#if ACC_DEF_BYTEORDER_INCLUDE_bsd_bsd_byteorder_h
# include "bsd/bsd_byteorder.h"
#endif
#if ACC_DEF_BYTEORDER_INCLUDE_endian_h
# include "endian.h"
#endif
#if ACC_DEF_BYTEORDER_INCLUDE_sys_endian_h
# include "sys/endian.h"
#endif
#if ACC_DEF_BYTEORDER_INCLUDE_machine_endian_h
# include "machine/endian.h"
#endif
#if 0 /* so far unused variants - kept for start to search... */
#if ACC_DEF_BYTEORDER_INCLUDE_machine_byte_order_h
# include "machine/byte_order.h"
#endif
#if ACC_DEF_BYTEORDER_INCLUDE_sys_types_h_stdint_h
/* OpenBSD ?? */
#include <sys/types.h>
#include <stdint.h>
#endif
#endif

#ifndef BYTE_ORDER
# if defined(__BYTE_ORDER)
#  define BYTE_ORDER     __BYTE_ORDER
#  define LITTLE_ENDIAN  __LITTLE_ENDIAN
#  define BIG_ENDIAN     __BIG_ENDIAN
# elif defined(_BYTE_ORDER)
#  define BYTE_ORDER     _BYTE_ORDER
#  define LITTLE_ENDIAN  _LITTLE_ENDIAN
#  define BIG_ENDIAN     _BIG_ENDIAN
# endif
#endif

#ifdef ACC_DEF_RUN
#ifndef BYTE_ORDER
# error BYTE_ORDER not defined
#endif
void acc_test_func()
{
}
#endif

#endif/*__BYTEORDER_INCLUDE_H__*/
