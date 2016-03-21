#ifndef __VESNA5_H__
#define __VESNA5_H__

/* This file contain definition of data structures and function which
 * interface to the world outside of VESNA.
 */

/******************************************************************/

typedef unsigned short word;

typedef struct
{
  word _wire;
  word _time;
} vdc_hit;

typedef struct
{
  word _wire;
  word _width;
} mwpc_hit;

#define MAX_VDC_HITS  128
#define MAX_MWPC_HITS 128

/* If MAX_VDC_HITS >= 255, then this must be changed to short or int. */
//typedef unsigned char sig_t; 

typedef struct
{
  /* The lists of hits are to be sorted according to wire and hit time,
   * for easy finding.  This is a job for the event unpacker!, if you do
   * not want us to change the event structure.  For now we'll do it!
   * (before making Q track fits) (He knows if it should be qsorted, or
   * if the data usually appears sorted from the raw data stream and can
   * be sorted upon insertation).  Also, no doublets allowed, even if
   * present in raw data.
   */

  vdc_hit  _vdc_hit   [2][2][MAX_VDC_HITS];
  int      _vdc_nhits [2][2];

  mwpc_hit _mwpc_hit  [4][2][MAX_MWPC_HITS];
  int      _mwpc_nhits[4][2];

} v5_event_t;

/******************************************************************/

typedef struct
{
  const char *_cal_vdc_tdc_prefix;

} v5_conf_t;

/******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

  void vesna5(v5_event_t* event);

  void vesna5_init();

  void vesna5_exit();

#ifdef __cplusplus
} /* end of extern "C" */
#endif

/******************************************************************/

#endif /* __VESNA5_H__ */

