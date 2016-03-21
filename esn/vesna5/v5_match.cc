#include "vesna5.h"
#include "v5_track.h"
#include "v5_defines.h"
#include "v5_match.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SQR(x) ((x)*(x))

template<typename T>
int compare_values(const void* a,const void* b)
{
  if (*((const T*) a) < *((const T*) b))
    return -1;
  return *((const T*) a) > *((const T*) b);
}



/******************************************************************/

/* Match Q tracks together to protons.  For each proton, we should have
 * two X tracks and two U tracks, and they should all match.
 *
 * We can reconstruct a proton with only one X and one U track, since we
 * also have the slopes, which seem to project to the other chambers
 * with an accuracy of about one wire.
 */

/* Essentially, from the lists of all tracks we have in the chambers,
 * one could start by finding which ones at all can match, and which are
 * ruled out from the beginning.  This way most of the matching will be
 * direct.  The there might be some ties left to break.
 *
 * X1 - X2  should match time, each slope should match the wire difference.
 *          correction.  The time difference depends on the velocity, so
 *          is not a detector constant.  For constant conditions, I've
 *          measured also a 0.1 ns time difference dependance per wire
 *          difference in x1 and x2 (also applies for u1 and u2).
 *          However, if adjusted, it most certainly can be used to figure
 *          out if two tracks belongs together, as it is 5 times more
 *          accurate than the accelerator repetition rate, hmm, cyclotron frequency.
 *
 * Xn - Un  should be within visible window x vs u (MIN_OVERLAP_X_MINUS_U,
 *          MAX_OVERLAP_X_MINUS_U)  (+/- 25 wires).  If not, they cannot
 *          be one track.  They should also match time-wise.
 *
 * Planning for MWPC-matching:
 *
 * From X tracks we immediately have X trajectory.  This can be matched
 * against hits in the MWPCs (before polarimeter, if present).
 */




/* We keep it simple, so perhaps not finding a perfect solution.  But
 * at least we will not be so greedy that we do take one solution as
 * soon as we find it and do not consider later indexed solutions.
 *
 * A problem for matching algorithms seems to be that we might have
 * situations where we do not want to (do hot have) all four needed
 * items for one track, but anyhow wants to make it.
 *
 */







/******************************************************************/

/* Calibration parameters.
 */

typedef struct
{
  double    _min_s;
  double    _max_s;
} slope_limit;

typedef struct
{
  double    _offset;
  double    _per_wire_diff;
  double    _var;
} dxdt_wire_diff;

typedef struct
{
  double    _offset;
  double    _per_wire_diff;
  double    _var;
} dt_wire_diff;

typedef struct
{
  double    _dt;  /* t_u - t_x  */
  double    _var; /* resolution */
} xu_timing;

typedef struct
{
  slope_limit    _s_limit[2/*VDC*/][2/*PLANE*/];
  dxdt_wire_diff _dxdtdw[2/*VDC*/][2/*PLANE*/];
  dt_wire_diff   _dtdw[2/*PLANE*/];
  xu_timing      _xu_dt[2/*VDC*/];
} v5_calib;

v5_calib _calib;


#define PLX  0
#define PLU  1

#define VDC0 0
#define VDC1 1

#define PLANE_DT_WIRE_DIFF(pl,dt0,dtdw,std) { \
  _calib._dtdw[(pl)]._offset = (dt0); \
  _calib._dtdw[(pl)]._per_wire_diff = (dtdw); \
  _calib._dtdw[(pl)]._var = SQR(std); \
}
#define SLOPE_LIMIT(pl,vdc,dxdt_min,dxdt_max) { \
  _calib._s_limit[(vdc)][(pl)]._min_s = (dxdt_min); \
  _calib._s_limit[(vdc)][(pl)]._max_s = (dxdt_max); \
}
#define SLOPE_WIRE_DIFF(pl,vdc,offset,pwd,std) { \
  _calib._dxdtdw[(vdc)][(pl)]._offset = (offset); \
  _calib._dxdtdw[(vdc)][(pl)]._per_wire_diff = (pwd); \
  _calib._dxdtdw[(vdc)][(pl)]._var = SQR(std); \
}
#define XU_TIMING(vdc,dt,std) { \
  _calib._xu_dt[(vdc)]._dt = (dt); \
  _calib._xu_dt[(vdc)]._var = SQR(std); \
}

void
match_init()
{
  PLANE_DT_WIRE_DIFF(PLX,  -1.68,  0.08,   4.52);
  SLOPE_WIRE_DIFF(PLX,VDC0,   0.017794, -0.000340,   0.000258);
  SLOPE_LIMIT(PLX,VDC0,   0.007233,  0.024885);
  SLOPE_WIRE_DIFF(PLX,VDC1,   0.017822, -0.000339,   0.000269);
  SLOPE_LIMIT(PLX,VDC1,   0.007090,  0.024840);
  PLANE_DT_WIRE_DIFF(PLU,  -0.56,  0.10,   4.00);
  SLOPE_WIRE_DIFF(PLU,VDC0,   0.014985, -0.000340,   0.000235);
  SLOPE_LIMIT(PLU,VDC0,   0.006832,  0.021601);
  SLOPE_WIRE_DIFF(PLU,VDC1,   0.015031, -0.000341,   0.000232);
  SLOPE_LIMIT(PLU,VDC1,   0.006880,  0.021657);
  XU_TIMING(VDC0,    3.57,    3.58);
  XU_TIMING(VDC1,    4.32,    2.80);
}
 
typedef struct
{
  int   _index;
  float _var;
} match_data;

typedef struct
{
  match_data _m[MAX_Q_TRACKS];
  int   _num;
} matching_q;

matching_q match_plane[2][MAX_Q_TRACKS];
matching_q match_vdc[2][MAX_Q_TRACKS];

typedef struct
{
  double _var;

  int _x0;
  int _x1;
  int _u0;
  int _u1;
} full_match;

#define MAX_MATCHES 1000

full_match matches[MAX_MATCHES];
int num_matches;

void
match_tracks()
{
  num_matches = 0;
  
  printf ("Match -------------------------------------------------------------\n");

  for (int plane = 0; plane < 2; plane++)
    {
      /* Try to match all tracks against each other.
       */

      for (int t0 = 0; t0 < tracks[0][plane]._ntracks; t0++)
	{
	  q_track& track0 = tracks[0][plane]._track[t0];

	  match_plane[plane][t0]._num = 0;

	  for (int t1 = 0; t1 < tracks[1][plane]._ntracks; t1++)
	    {
	      q_track& track1 = tracks[1][plane]._track[t1];

	      double wire_diff = track1._param._w0 - track0._param._w0;

	      double dt    = track1._param._t0 - track0._param._t0;

	      double dtdx1 = track0._param._s;
	      double dtdx2 = track1._param._s;

	      double dxdt1 = 1/dtdx1;
	      double dxdt2 = 1/dtdx2;

	      double expect_dxdt1 = 
		_calib._dxdtdw[0][plane]._offset + 
		_calib._dxdtdw[0][plane]._per_wire_diff * wire_diff;
	      double expect_dxdt2 = 
		_calib._dxdtdw[1][plane]._offset + 
		_calib._dxdtdw[1][plane]._per_wire_diff * wire_diff;

	      double expect_dt =
		_calib._dtdw[plane]._offset + 
		_calib._dtdw[plane]._per_wire_diff * wire_diff;

	      double sigma_dxdt1_sqr = SQR(dxdt1 - expect_dxdt1) / _calib._dxdtdw[0][plane]._var;
	      double sigma_dxdt2_sqr = SQR(dxdt2 - expect_dxdt2) / _calib._dxdtdw[1][plane]._var;
	      double sigma_dt_sqr    = SQR(dt - expect_dt) / _calib._dtdw[plane]._var;

	      bool slope1_ok = (dxdt1 >= _calib._s_limit[0][plane]._min_s &&
				dxdt1 <= _calib._s_limit[0][plane]._max_s);
	      bool slope2_ok = (dxdt2 >= _calib._s_limit[1][plane]._min_s &&
				dxdt2 <= _calib._s_limit[1][plane]._max_s);

	      if (slope1_ok && slope2_ok &&
		  (sigma_dxdt1_sqr + sigma_dxdt2_sqr + sigma_dt_sqr) < 30)
		{
		  printf("Match:   %c Q%02d%c -   Q%02d%c : %5.1f %5.1f %5.1f\n",
			 plane == 0 ? 'X' : 'U',
			 t0,slope1_ok ? '*' : ' ',
			 t1,slope2_ok ? '*' : ' ',
			 sqrt(sigma_dxdt1_sqr),
			 sqrt(sigma_dxdt2_sqr),
			 sqrt(sigma_dt_sqr));

		  match_data *m = &match_plane[plane][t0]._m[match_plane[plane][t0]._num++];

		  m->_index = t1;
		  m->_var   = sigma_dxdt1_sqr + sigma_dxdt2_sqr + sigma_dt_sqr;
		}
	    }
	}
    }     

  for (int vdc = 0; vdc < 2; vdc++)
    {

      for (int t0 = 0; t0 < tracks[vdc][0]._ntracks; t0++)
	{
	  q_track& track0 = tracks[vdc][0]._track[t0];

	  match_vdc[vdc][t0]._num = 0;

	  for (int t1 = 0; t1 < tracks[vdc][1]._ntracks; t1++)
	    {
	      q_track& track1 = tracks[vdc][1]._track[t1];

	      double wire_diff = track1._param._w0 - track0._param._w0;

	      double dt        = track1._param._t0 - track0._param._t0;
	      
	      double expect_dt = _calib._xu_dt[vdc]._dt;
	      double var_dt    = _calib._xu_dt[vdc]._var;

	      double sigma_dt_sqr = SQR(expect_dt - dt) / var_dt;

	      double dtdx1 = track0._param._s;
	      double dtdx2 = track1._param._s;

	      double dxdt1 = 1/dtdx1;
	      double dxdt2 = 1/dtdx2;

	      bool slope1_ok = (dxdt1 >= _calib._s_limit[vdc][0]._min_s &&
				dxdt1 <= _calib._s_limit[vdc][0]._max_s);
	      bool slope2_ok = (dxdt2 >= _calib._s_limit[vdc][1]._min_s &&
				dxdt2 <= _calib._s_limit[vdc][1]._max_s);

	      if (slope1_ok && slope2_ok &&
		  (sigma_dt_sqr) < 30 &&
		  (wire_diff >= -25 && wire_diff <= 25))
		{
		  printf("Match: %d X Q%02d%c - U Q%02d%c : %5.1f (dw:%5.1f)\n",
			 vdc,
			 t0,slope1_ok ? '*' : ' ',
			 t1,slope2_ok ? '*' : ' ',
			 sqrt(sigma_dt_sqr),
			 wire_diff);

		  match_data *m = &match_vdc[vdc][t0]._m[match_vdc[vdc][t0]._num++];

		  m->_index = t1;
		  m->_var   = sigma_dt_sqr;
		}
	    }
	}
    }

  
  for (int tx0 = 0; tx0 < tracks[0][0]._ntracks; tx0++) // x0
    {
      for (int ix1 = 0; ix1 < match_plane[0][tx0]._num; ix1++)
	{
	  int tx1 = match_plane[0][tx0]._m[ix1]._index;

	  // tx0 and tx1 are matching tracks

	  // find matching u track

	  for (int iu0 = 0; iu0 < match_vdc[0][tx0]._num; iu0++)
	    {
	      int tu0 = match_vdc[0][tx0]._m[iu0]._index;

	      for (int iu1 = 0; iu1 < match_vdc[1][tx1]._num; iu1++)
		{
		  int tu1 = match_vdc[1][tx1]._m[iu1]._index;

		  // So we have candidates, tx0, tx1, tu0, tu1
		  // does tu0 and tu1 match??

		  for (int iu1_x = 0; iu1_x < match_plane[0][tu0]._num; iu1_x++)
		    {
		      int tu1_x = match_plane[1][tu0]._m[iu1_x]._index;

		      // We have a fully matching candidate track with all four
		      // chambers

		      if (tu1_x == tu1)
			{
			  double var = (match_plane[0][tx0]._m[ix1]._var +
					match_vdc[0][tx0]._m[iu0]._var + 
					match_vdc[1][tx1]._m[iu1]._var +
					match_plane[1][tu0]._m[iu1_x]._var);

			  printf ("Match: x0: Q%02d  u0: Q%02d   x1: Q%02d  u1: Q%02d  %5.1f\n",
				  tx0,tu0,
				  tx1,tu1,
				  var);


			  full_match *m = &matches[num_matches++];

			  m->_x0 = tx0;
			  m->_x1 = tx1;
			  m->_u0 = tu0;
			  m->_u1 = tu1;
			  m->_var = var;
			}
		    }
		}
	    }
	}
    }

  // Then sort the list of full matches we have according to var

  qsort(matches,num_matches,sizeof(matches[0]),compare_values<double>);

  // Go through the matches, except the first one, then remove tracks
  // from later ones that we already used

  for (int i = 0; i < num_matches; i++)
    {
      full_match *m = &matches[i];

      int tracks = ((m->_x0 != -1) + 
		    (m->_x1 != -1) +
		    (m->_u0 != -1) + 
		    (m->_u1 != -1));

      if (tracks >= 4)
	{
	  // accept the track
	  
	  printf ("Match: ACCEPT: x0: Q%02d  u0: Q%02d   x1: Q%02d  u1: Q%02d  %5.1f\n",
		  m->_x0,m->_u0,
		  m->_x1,m->_u1,
		  m->_var);

	  for (int j = i+1; j < num_matches; j++)
	    {
	      // Kill any later tracks (also us, if we later try to find
	      // ones with fewer tracks)

	      full_match *m2 = &matches[j];

	      if (m2->_x0 == m->_x0) 
		m2->_x0 = -1;
	      if (m2->_x1 == m->_x1) 
		m2->_x1 = -1;
	      if (m2->_u0 == m->_u0) 
		m2->_u0 = -1;
	      if (m2->_u1 == m->_u1) 
		m2->_u1 = -1;

	    }
	}
    }
}

/******************************************************************/
