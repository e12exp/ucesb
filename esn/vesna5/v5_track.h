#ifndef __VESNA5_TRACK_H__
#define __VESNA5_TRACK_H__

/* This file contain definition of data structures used to
 * handle all the tracks known, their parameters, constituents, etc...
 */

/******************************************************************/

/* A R track is a straight line in one chamber.
 * A Q track is two R tracks combined to make a 'V' in one chamber.
 * A H track is two Q tracks combined from the two chambers of same
 *     orientation (both X or both U).
 * A F track is two H tracks combined from the two kinds of
 *     chamber orientations.
 */

/******************************************************************/

/* Information collected to make a linear fit.
 */

typedef struct
{
  double _s;   /* sum */
  double _t;
  double _w;
  double _wt;
  double _ww;
} linear_fit;

/******************************************************************/

typedef struct
{
  linear_fit _rise;
  linear_fit _fall;
} q_track_fit;

/*******************************************************************
 *
 * A pattern explaining the contributing signals to an R track.
 * 
 * It's simply a bit-pattern.  If signal # s contributes, then
 * bit (s%32) is set in element [s/32]  (assuming int has 32 bits).
 */

#define R_TRACK_USED_SIZE ((MAX_VDC_HITS+(sizeof (unsigned int)*8)-1)/(sizeof (unsigned int)*8))

typedef struct
{
  int          _sigs;  /* Number of signals. */
  unsigned int _used[R_TRACK_USED_SIZE]; /* Bit pattern of used VDC signals */
} r_track;

/*******************************************************************
 *
 * The fit parameters of a Q track.
 *
 * The parameters are the location (wire and time) of the knee of the
 * 'V', and the slope.  
 */

typedef struct
{
  double _w0, _t0, _s;
} q_track_param;

/*******************************************************************
 *
 * A Q track consist of it's raw track information, the peak
 * signal(???), the two fits for the two slopes, and the combined fit.
 */

typedef struct
{
  r_track       _r_sigs;
  int           _peak;
  q_track_fit   _fit;
  q_track_param _param;

} q_track;

/******************************************************************/

/* FIXME: fix cuts and victimization of bad tracks so that this can be
 * decreased.  Around 10 would be nice.
 *
 * The general idea is anyhow to have it static, to avoid
 * reallocations.
 */

#define MAX_Q_TRACKS 800

/* This structure is used to hold all Q tracks from one chamber.
 * So the ESN detector has four of it.
 */

typedef struct
{
  q_track _track[MAX_Q_TRACKS];
  int     _ntracks;
} q_tracks;

/******************************************************************/

extern q_tracks tracks[2][2];

extern v5_event_t* _v5_event;

/*******************************************************************
 *
 * Prototypes.
 */

void 
find_Q_tracks(vdc_hit* hit,int nhits,q_tracks* tracks);

void
display_event(v5_event_t* event);

void
display_event_chamber(v5_event_t* event,int vdc,int plane);

/******************************************************************/

void 
find_Q_tracks2(vdc_hit* hit,int nhits,q_tracks* tracks);

/******************************************************************/

#endif /* __VESNA5_TRACK_H__ */
