#ifndef __EXT_FILE_PROTO_H__
#define __EXT_FILE_PROTO_H__

#include <stdint.h>
#include <stdlib.h>

/* To get at EXT_DATA_ITEM_TYPE_... */
#include "ext_data_client.h"

#define EXTERNAL_WRITER_MIN_SHARED_SIZE  4096

#define EXTERNAL_WRITER_CMD_SHM_WORK     0x6e // more data in circ buffer

#define EXTERNAL_WRITER_RESPONSE_ERROR        1
#define EXTERNAL_WRITER_RESPONSE_SHM_ERROR    2
#define EXTERNAL_WRITER_RESPONSE_WORK         3
#define EXTERNAL_WRITER_RESPONSE_WORK_RD      4
#define EXTERNAL_WRITER_RESPONSE_DONE         0x76 // magic :-)

#define EXTERNAL_WRITER_BUF_OPEN_FILE     0x01
#define EXTERNAL_WRITER_BUF_BOOK_NTUPLE   0x02
#define EXTERNAL_WRITER_BUF_ALLOC_ARRAY   0x03
#define EXTERNAL_WRITER_BUF_CREATE_BRANCH 0x04
#define EXTERNAL_WRITER_BUF_RESIZE        0x05 // only larger!
#define EXTERNAL_WRITER_BUF_ARRAY_OFFSETS 0x06
#define EXTERNAL_WRITER_BUF_NAMED_STRING  0x07
#define EXTERNAL_WRITER_BUF_SETUP_DONE    0x08
#define EXTERNAL_WRITER_BUF_SETUP_DONE_RD 0x09 // reverse flow => reader
#define EXTERNAL_WRITER_BUF_SETUP_DONE_WR 0x0a // from struct_writer
// Items before here are init_chunks for the struct writer (always sent).
#define EXTERNAL_WRITER_BUF_NTUPLE_FILL   0x0b
#define EXTERNAL_WRITER_BUF_KEEP_ALIVE    0x0c
#define EXTERNAL_WRITER_BUF_EAT_LIN_SPACE 0x0d
#define EXTERNAL_WRITER_BUF_DONE          0x0e
#define EXTERNAL_WRITER_BUF_ABORT         0x0f
#define EXTERNAL_WRITER_BUF_HIST_H1I      0x10 // open, resize, hist, done

#define EXTERNAL_WRITER_FLAG_TYPE_INT32   EXT_DATA_ITEM_TYPE_INT32
#define EXTERNAL_WRITER_FLAG_TYPE_UINT32  EXT_DATA_ITEM_TYPE_UINT32
#define EXTERNAL_WRITER_FLAG_TYPE_FLOAT32 EXT_DATA_ITEM_TYPE_FLOAT32
#define EXTERNAL_WRITER_FLAG_TYPE_MASK    EXT_DATA_ITEM_TYPE_MASK
#define EXTERNAL_WRITER_FLAG_HAS_LIMIT    EXT_DATA_ITEM_HAS_LIMIT

#define EXTERNAL_WRITER_MAGIC (0x57e65c73u +32) // change with protocol version

#define EXTERNAL_WRITER_DEFAULT_PORT      56577

/* The (maximum) size of the NTUPLE_FILL message is declared as a
 * macro, so that it can be used by certain 'hacked' output buffer
 * child classes.
 */
#define EXTERNAL_WRITER_SIZE_NTUPLE_FILL(max_size,sort_u32_words, \
					 has_raw,raw_words)		\
  ((uint32_t) sizeof(external_writer_buf_header) +			\
   (1 + sort_u32_words +						\
    (has_raw ? (1 + raw_words) : 0)) * (uint32_t) sizeof(uint32_t) +\
   max_size)

struct external_writer_buf_header
{
  uint32_t _request;
  uint32_t _length;
};

struct external_writer_buf_string
{
  uint32_t _length;
  char     _string[];
};

struct external_writer_portmap_msg
{
  uint32_t _magic;
  uint16_t _port;
};

#endif/*__EXT_FILE_PROTO_H__*/
