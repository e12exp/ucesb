#ifndef __WR_STAMP_HH__
#define __WR_STAMP_HH__

/* TODO: confirm this! */
#define WR_STAMP_EBID_ERROR           0x00010000
#define WR_STAMP_EBID_BRANCH_ID_MASK  0x00000f00
#define WR_STAMP_EBID_UNUSED		\
  (0xffffffff ^				\
   WR_STAMP_EBID_ERROR ^		\
   WR_STAMP_EBID_BRANCH_ID_MASK)
#define WR_STAMP_DATA_TIME_MASK       0x0000ffff
#define WR_STAMP_DATA_ID_MASK         0xffff0000
#define WR_STAMP_DATA_0_16_ID         0x03e10000
#define WR_STAMP_DATA_1_16_ID         0x04e10000
#define WR_STAMP_DATA_2_16_ID         0x05e10000
#define WR_STAMP_DATA_3_16_ID         0x06e10000

#endif//__WR_STAMP_HH__
