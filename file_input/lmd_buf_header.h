/* From s_filhe.h
 */

#ifndef __LMD_BUF_HEADER_H__
#define __LMD_BUF_HEADER_H__

#include "typedef.hh"
#include "swapping.hh"

/*  ================= GSI Buffer header =======================  */

// (0x8000 - sizeof(s_bufhe))/2
// Above this buffer size l_free[2] is used for used words.
#define LMD_BUF_HEADER_MAX_IUSED_DLEN 16360

typedef struct
{
  sint32  l_dlen;      /*  Length of data field in words */
  sint16  i_type; 
  sint16  i_subtype; 
  sint16  i_used;      /*  Used length of data field in words */
  sint8   h_end;       /*  Fragment at begin of buffer */
  sint8   h_begin;     /*  Fragment at end of buffer */
  sint32  l_buf;       /*  Current buffer number */
  sint32  l_evt;       /*  Number of fragments */
  sint32  l_current_i; /*  Index, temporarily used */
  sint32  l_time[2]; 
  sint32  l_free[4]; 
} s_bufhe_little_endian;

typedef struct
{
  sint32  l_dlen;      /*  Length of data field in words */
  sint16  i_subtype; 
  sint16  i_type; 
  sint8   h_begin;     /*  Fragment at end of buffer */
  sint8   h_end;       /*  Fragment at begin of buffer */
  sint16  i_used;      /*  Used length of data field in words */
  sint32  l_buf;       /*  Current buffer number */
  sint32  l_evt;       /*  Number of fragments */
  sint32  l_current_i; /*  Index, temporarily used */
  sint32  l_time[2]; 
  sint32  l_free[4]; 
} s_bufhe_big_endian;

typedef s_bufhe_big_endian         s_bufhe_network_order;
#define s_bufhe_host               HOST_ENDIAN_TYPE(s_bufhe)

/* ------------------------------------------------------------------ */

#define LMD_BUF_HEADER_10_1_TYPE      10
#define LMD_BUF_HEADER_10_1_SUBTYPE   1

/* ------------------------------------------------------------------ */

#endif // __LMD_BUF_HEADER_H__
