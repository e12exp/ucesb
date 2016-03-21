#include "vesna5.h"
#include "v5_track.h"
#include "v5_defines.h"
#include "v5_calib.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "fit_linear.hh"
#include "fit_gaussian.hh"
#include "analyse_spread.h"

#define SQR(x) ((x)*(x))


/******************************************************************/

int
uses_same_r_sigs(r_track* r_sigs1,
		 r_track* r_sigs2)
{
  int i;

  for (int i = 0; i < R_TRACK_USED_SIZE; i++)
    if (r_sigs1->_used[i] & r_sigs2->_used[i])
      return true;
  return false;
}

/******************************************************************/

#define abs(x) ((x)>0?(x):(-x))

fit_linear avfit[2][2];

fit_linear tdc_fit[2][2][VDC_WIRES];



void calib_init()
{
  int vdc, plane;

  for (vdc = 0; vdc < 2; vdc++)
    for (plane = 0; plane < 2; plane++)
      avfit[vdc][plane].init();
}

/******************************************************************/

void
fit_vdc_tdc(int vdc,int plane,const q_track& track)
{
  vdc_hit* hit = _event->_vdc_hit  [vdc][plane];
  int nhits    = _event->_vdc_nhits[vdc][plane];

  int sig;

  /* So find the rising and falling hits, and see
   * how they match to the fitted lines.
   */

  double rising = 1;

  for (sig = 0; sig < MAX_VDC_HITS; sig++)
    {
      if (track._r_sigs._used[sig / (sizeof (unsigned int) * 8)] &
	  (((unsigned int) 1) << (sig % (sizeof (unsigned int) * 8))))
	{
	  
	  if (sig == track._peak)
	    {
	      rising = -1;
	      continue;
	    }

	  /* Calculate expected time:
	   */

	  double t_expect = track._param._t0 - 
	    track._param._s * rising * (track._param._w0 - hit[sig]._wire);
	  /*
	  printf ("Local t: %d/%d %d : got %3d expect: %6.1f\n",
		  vdc,plane,hit[sig]._wire,
		  hit[sig]._time,t_expect);
	  */

	  printf ("%d:%d:%03d Got %3d Expect: %6.1f => diff = %6.1f\n",
		  vdc,plane,hit[sig]._wire,hit[sig]._time,t_expect,hit[sig]._time-t_expect);

	  fit_linear& fit = tdc_fit[vdc][plane][hit[sig]._wire];

	  fit.fill(hit[sig]._time,t_expect);

	  vect<2> param;
	  vect<2> param_unity;
	  
	  matrix_sym<2> var;

	  fit.get_param_var(param,var);

	  param_unity[0] = 0;
	  param_unity[1] = 1;

	  if (((int) fit._sum) % 10 == 0)
	    {
	      printf ("TDC: %d:%d:%03d %4.0f %3d %6.2f",
		      vdc,plane,hit[sig]._wire,fit._sum,hit[sig]._time,t_expect);
	      
	      printf (" # %6.2f , %6.2f%% | %6.3f , %6.3f%% # %6.2f < %6.2f\n",
		      param[0],(param[1]-1)*100,
		      sqrt(var.get(0,0)),sqrt(var.get(1,1))*100,
		      sqrt(fit.get_var_y(param)),
		      sqrt(fit.get_var_y(param_unity)));
	    }
	}
    }
}

/******************************************************************/

#include <vector>
#include <algorithm>

/* To simplify life, for each event which survives the initial cuts to
 * be considered for the calibration, we store all the information that
 * will be used, and do the evaluation at the end.  The point is also
 * that from each event we will be fitting several quantities.  To be
 * able to clean up the data as much as possible before fitting, if an
 * event as far off in any of the parameters, the entire event is
 * removed as most likely it has problems otherplaces as well.
 */

/* For calibrating x1 versus x2 or u1 versus u2.  x and u are handled
 * independently.
 */

typedef struct
{
  q_track_param _param[2];
} calib1_qq_same;

typedef std::vector<calib1_qq_same> calib1_qq_same_vect;

calib1_qq_same_vect _calib1_qq_same[2];

/* For calibrating timing between x and u chambers.  In order to have
 * several statistics to make initial cutting, consider entire
 * detector at once.
 *
 * We should get same timing from x versus x and u versus u as from the
 * previous data.  We actually use events from the previous data.  And
 * let them do the cutting.
 */

typedef struct
{
  int _x_index;
  int _u_index;
} calib1_xu;

typedef std::vector<calib1_xu> calib1_xu_vect;

calib1_xu_vect _calib1_xu;

/******************************************************************/

int
add_calib1_qq_same(int plane,
		   q_track_param *src1,
		   q_track_param *src2)
{
  int index = _calib1_qq_same[plane].size();

  calib1_qq_same entry;

  entry._param[0] = *src1;
  entry._param[1] = *src2;

  _calib1_qq_same[plane].push_back(entry);

  return index;
}

/******************************************************************/

const char PLANE[2][4] = { "PLX"  , "PLU"  };
const char VDC[2][5]   = { "VDC0" , "VDC1" };


/******************************************************************/

#define MAX_WIRE_DIFF 100

void
calc_first_round_calibration()
{
  /* For each plane individually (x-x and u-u) we want to see how
   * they correlate.  We calculate:
   *
   * How coordinate in other vdc correspond to coordinate and angle
   * in first vcd.
   * Timing difference.  (Which in principle would depend also on
   * angle, as the distance will differ a bit.
   */

  for (int plane = 0; plane < 2; plane++)
    {
      /* Now for the dependance of coordinate in other VDC as function
       * of coordinate and angle in first, we need to clean up a two-
       * dimensional spectrum.  This is a pain.  Therefore we
       * do first bin it in wire-difference and clean the data up
       * there.
       */

#define WIRE_DIFF_BINS (MAX_WIRE_DIFF*2+1)

      std::vector<double> wire_diff_s[2][WIRE_DIFF_BINS];
      std::vector<double> wire_diff_dt[WIRE_DIFF_BINS];

      int n = _calib1_qq_same[plane].size();

      for (int i = 0; i < n; i++)
	{
	  calib1_qq_same &data = _calib1_qq_same[plane][i];

	  double wire_diff = 
	    data._param[1]._w0 - 
	    data._param[0]._w0;

	  // fprintf (stderr,"%6.1f \n",wire_diff);
	  
	  if (wire_diff <= -MAX_WIRE_DIFF ||
	      wire_diff >=  MAX_WIRE_DIFF)
	    continue;
	  
	  // The addition of MAX_WIRE_DIFF must be before the cast
	  // to int, otherwise, negative numbers will be off by one
	  // (casting a double to int cuts the decimals, it does not
	  // round down)
	  
	  int bin = (int) (wire_diff + 0.5 + MAX_WIRE_DIFF);

	  for (int vdc = 0; vdc < 2; vdc++)
	    {
	      /* @vdc is number the one with coordinate and angle
	       */
	      
	      double dtdx = data._param[vdc]._s;
	      double dxdt = 1/dtdx;

	      wire_diff_s[vdc][bin].push_back(dxdt);

	      // fprintf (stderr,"%d \n",bin);
	    }

	  double dt = 
	    data._param[1]._t0 - 
	    data._param[0]._t0;

	  wire_diff_dt[bin].push_back(dt);
	}

      fit_linear   fit_wire_diff_dt;
      fit_gaussian fit_dt_var;

      for (int bin = 0; bin < WIRE_DIFF_BINS; bin++)
	{
	  int n = wire_diff_dt[bin].size();
	  
	  float *v = (float *) malloc(n * sizeof (float));
	  
	  for (int i = 0; i < n; i++)
	    v[i] = wire_diff_dt[bin][i];
	  
	  float min_accept, max_accept;
	  bool good_spread;
	  
	  good_spread = analyse_spread<float>(v,n,
					      min_accept,max_accept,
					      3);
	  
	  if (good_spread)
	    {
	      fit_gaussian fit;
	      
	      for (int i = 0; i < n; i++)
		{
		  double dt = v[i];
		  
		  if (dt > min_accept &&
		      dt < max_accept)
		    fit.fill(dt);
		}
	      
	      fprintf (stderr,"%d,%3d (%5.1f,%5.1f)%d  %5.1f,%5.1f  %4.0f\n",
		       plane,bin,
		       min_accept,max_accept,
		       good_spread,
		       fit.mean(),fit.std(),
		       fit.sum());

	      double mean = fit.mean();
	      double var  = fit.var();
	      
	      // TODO, use isfinite in var
	      // if (isfinite(var))
	      if (fit.sum() > 2)
		{
		  fit_wire_diff_dt.fill(bin-MAX_WIRE_DIFF,
					fit.mean(),fit.var());
		  fit_dt_var.fill(fit.var(),fit.sum());
		}
	    }
	}

      fprintf(stderr,"%d dt=%5.2f + %5.2f*wire_diff  var=%5.2f\n",
	      plane,
	      fit_wire_diff_dt.y0(),
	      fit_wire_diff_dt.dydx(),
	      sqrt(fit_dt_var.mean()));

      double dt_off = fit_wire_diff_dt.y0();
      double ddtdw  = fit_wire_diff_dt.dydx();
      double dt_var = fit_dt_var.mean();
      
      fit_linear   fit2_wire_diff_dt;
      fit_gaussian fit2_dt_var;

      std::vector<float> dts;

      for (int i = 0; i < n; i++)
	{
	  calib1_qq_same &data = _calib1_qq_same[plane][i];
	  
	  double wire_diff = 
	    data._param[1]._w0 - 
	    data._param[0]._w0;
	      
	  double dt = 
	    data._param[1]._t0 - 
	    data._param[0]._t0;

	  double expect_dt = dt_off + ddtdw * wire_diff;
	  
	  double sigma_sqr = SQR(expect_dt - dt) / dt_var;
	  
#define EXPECT_DT_SIGMA_CUT 5.0
	  
	  if (sigma_sqr < SQR(EXPECT_DT_SIGMA_CUT))
	    {
	      fit2_wire_diff_dt.fill(wire_diff,dt);
	      fit2_dt_var.fill(expect_dt - dt);
	      
	      // slopes.push_back(dxdt);
	    }
	      /*
	      fprintf (stderr,"dw = %6.1f (%3d) => %9.6f : s = %9.6f (%9.6f) %5.1f\n",
		       wire_diff,bin,
		       expect_dxdt,dxdt,
		       expect_dxdt-dxdt,
		       sqrt(sigma_sqr));
	      */
	}
	  
      fprintf(stderr,"%d dt=%5.2f + %5.2f*wire_diff  var=%5.2f\n",
	      plane,
	      fit2_wire_diff_dt.y0(),
	      fit2_wire_diff_dt.dydx(),
	      sqrt(fit2_dt_var.var()));
      
      printf ("/*prm*/ PLANE_DT_WIRE_DIFF(%s,/* dt */ %5.2f,/* +delta_w* */%5.2f,/* std */ %5.2f);\n",
	      PLANE[plane],
	      fit2_wire_diff_dt.y0(),
	      fit2_wire_diff_dt.dydx(),
	      sqrt(fit2_dt_var.var()));

      for (int vdc = 0; vdc < 2; vdc++)
	{
	  fit_linear   fit_wire_diff_slope;
	  fit_gaussian fit_slope_var;

	  for (int bin = 0; bin < WIRE_DIFF_BINS; bin++)
	    {
	      int n = wire_diff_s[vdc][bin].size();
	      
	      float *v = (float *) malloc(n * sizeof (float));

	      for (int i = 0; i < n; i++)
		v[i] = wire_diff_s[vdc][bin][i];

	      float min_accept, max_accept;
	      bool good_spread;

	      good_spread = analyse_spread<float>(v,n,
						  min_accept,max_accept,
						  0.002);

	      if (good_spread)
		{
		  fit_gaussian fit;
		  
		  for (int i = 0; i < n; i++)
		    {
		      double dxdt = v[i];
		      
		      if (dxdt > min_accept &&
			  dxdt < max_accept)
			fit.fill(dxdt);
		    }
		  
		  fprintf (stderr,"%d,%d,%3d (%5.1f,%5.1f)%d  %5.1f,%5.1f  %4.0f\n",
			   plane,vdc,bin,
			   min_accept*1000.0,max_accept*1000.0,
			   good_spread,
			   fit.mean()*1000.0,fit.std()*1000.0,
			   fit.sum());
		  
		  double mean = fit.mean();
		  double var  = fit.var();

		  // TODO, use isfinite in var
		  // if (isfinite(var))
		  if (fit.sum() > 2)
		    {
		      fit_wire_diff_slope.fill(bin-MAX_WIRE_DIFF,
					       fit.mean(),fit.var());
		      fit_slope_var.fill(fit.var(),fit.sum());
		    }
		}
	    }

	  fprintf(stderr,"%d,%d s=%9.6f + %9.6f*wire_diff  var=%9.6f\n",
		  plane,vdc,
		  fit_wire_diff_slope.y0(),
		  fit_wire_diff_slope.dydx(),
		  sqrt(fit_slope_var.mean()));
	      /*
	      for (int i = 0; i < wire_diff_s[vdc][bin].size(); i++)
		{
		  fprintf (stderr,"%5.1f ",wire_diff_s[vdc][bin][i]*1000.0);
		  
		}
	      fprintf (stderr,"\n");
	      */

	  double s_off = fit_wire_diff_slope.y0();
	  double dsdw  = fit_wire_diff_slope.dydx();
	  double s_var = fit_slope_var.mean();

	  fit_linear   fit2_wire_diff_slope;
	  fit_gaussian fit2_slope_var;

	  std::vector<float> slopes;

	  for (int i = 0; i < n; i++)
	    {
	      calib1_qq_same &data = _calib1_qq_same[plane][i];
	      
	      /* @vdc is number the one with coordinate and angle
	       */
	      
	      double wire_diff = 
		data._param[1]._w0 - 
		data._param[0]._w0;
	      
	      double dtdx = data._param[vdc]._s;
	      double dxdt = 1/dtdx;

	      double expect_dxdt = s_off + dsdw * wire_diff;
	      
	      // int bin = (int) (wire_diff + 0.5 + MAX_WIRE_DIFF);

	      double sigma_sqr = SQR(expect_dxdt - dxdt) / s_var;

#define EXPECT_S_SIGMA_CUT 5.0

	      if (sigma_sqr < SQR(EXPECT_S_SIGMA_CUT))
		{
		  fit2_wire_diff_slope.fill(wire_diff,dxdt);
		  fit2_slope_var.fill(expect_dxdt - dxdt);

		  slopes.push_back(dxdt);
		}
	      /*
	      fprintf (stderr,"dw = %6.1f (%3d) => %9.6f : s = %9.6f (%9.6f) %5.1f\n",
		       wire_diff,bin,
		       expect_dxdt,dxdt,
		       expect_dxdt-dxdt,
		       sqrt(sigma_sqr));
	      */
	    }
	  
	  fprintf(stderr,"%d,%d s=%9.6f + %9.6f*wire_diff  var=%9.6f\n",
		  plane,vdc,
		  fit2_wire_diff_slope.y0(),
		  fit2_wire_diff_slope.dydx(),
		  sqrt(fit2_slope_var.var()));

	  printf ("/*prm*/ SLOPE_WIRE_DIFF(%s,%s,/* s0 */ %9.6f,/* +delta_w* */%9.6f,/* std */ %9.6f);\n",
		  PLANE[plane],VDC[vdc],
		  fit2_wire_diff_slope.y0(),
		  fit2_wire_diff_slope.dydx(),
		  sqrt(fit2_slope_var.var()));

	  if (slopes.size())
	    {
	      sort(slopes.begin(),slopes.end());
	      
	      float at_02 = slopes[(slopes.size() *  1 )/100];;
	      float at_06 = slopes[(slopes.size() *  5 )/100];;
	      float at_94 = slopes[(slopes.size() * 95 )/100];;
	      float at_98 = slopes[(slopes.size() * 99 )/100];;
	      
	      fprintf (stderr,"%9.6f %9.6f | %9.6f || %9.6f | %9.6f %9.6f\n",
		       at_02,at_06,
		       at_06-at_02,
		       at_98-at_94,
		       at_94,at_98);

	      double enlarge = at_98 - at_02;
	      double at_min  = (at_06 - at_02);
	      double at_max  = (at_98 - at_94);

	      double lim_min = at_02 - enlarge * (.25 + (at_min / (at_min + at_max)));
	      double lim_max = at_98 + enlarge * (.25 + (at_max / (at_min + at_max)));

	      printf ("/*prm*/ SLOPE_LIMIT(%s,%s,/* smin */ %9.6f,/* xmax */%9.6f);\n",
		      PLANE[plane],VDC[vdc],
		      lim_min,lim_max);


	    }
	}
      










    }












  /* Timing between x and u chambers.
   */

  fit_gaussian t_diff_xu[2];

  float *diff[2] = { NULL , NULL };

  float min_accept[2], max_accept[2];
  bool  good_spread[2];

  int n = _calib1_xu.size();

  for (int vdc = 0; vdc < 2; vdc++)
    {
      diff[vdc] = (float*) malloc(n * sizeof(float));

      for (int i = 0; i < n; i++)
	{
	  int index1 = _calib1_xu[i]._x_index;
	  int index2 = _calib1_xu[i]._u_index;

	  calib1_qq_same &qq_same_x = (_calib1_qq_same[0])[index1];
	  calib1_qq_same &qq_same_u = (_calib1_qq_same[1])[index2];

	  double xu_diff = (qq_same_u._param[vdc]._t0 -
			    qq_same_x._param[vdc]._t0);

	  diff[vdc][i] = xu_diff;
	}

      good_spread[vdc] = analyse_spread<float>(diff[vdc],n,
					       min_accept[vdc],max_accept[vdc],
					       5.0);
    }
  
  for (int i = 0; i < n; i++)
    {
      for (int vdc = 0; vdc < 2; vdc++)
	{
	  if (diff[vdc][i] < min_accept[vdc] ||
	      diff[vdc][i] > max_accept[vdc])
	    goto next_entry;
	}

      for (int vdc = 0; vdc < 2; vdc++)
	t_diff_xu[vdc].fill(diff[vdc][i]);

    next_entry:
      ;
    }

  for (int vdc = 0; vdc < 2; vdc++)
    {
      printf ("xu_res : %d %5d  m=%6.2f s=%6.2f  (%6.2f %6.2f) %d\n",
	      vdc,_calib1_xu.size(),
	      t_diff_xu[vdc].mean(),
	      t_diff_xu[vdc].std(),
	      min_accept[vdc],max_accept[vdc],
	      good_spread[vdc]);

      printf ("/*prm*/ XU_TIMING(%s,/* u-x */%7.2f,/* std */ %6.2f);\n",
	      VDC[vdc],
	      t_diff_xu[vdc].mean(),
	      t_diff_xu[vdc].std());

      free(diff[vdc]);
    }


}

/******************************************************************/

typedef struct
{
  double _wire[2];
  int    _index;
} qq_same_info;


/******************************************************************/

/* How alone (in wires) a Q track must be in order to avoid mis-matching them
 */

#define FIRST_CALIB_Q_TRACK_CLEAN        50

/* Requirement on number of signals, to avoid bad pointing accuracy. 
 */

#define FIRST_CALIB_Q_TRACK_MIN_SIGNALS  6

/* Given wire x, what wires can be hit in u?  (Too far away, and it
 * cannot be the same hit.)
 */

#define MIN_OVERLAP_X_MINUS_U           -25
#define MAX_OVERLAP_X_MINUS_U            25

void
first_round_calibration()
{
  int vdc, plane;
  int t1, t2;
  int t;

  double a, dtdx1, dtdx2;

  q_track *track1;
  q_track *track2;
  q_track *track;

  qq_same_info qq_same[2][VDC_WIRES / FIRST_CALIB_Q_TRACK_CLEAN + 1];
  int num_qq_same[2] = { 0 , 0 };

  /* There are many things to calibrate.  First problem
   * is to see if the signals possibly are good.  We want an
   * clearly defined track in this chamber, as well as the other
   * so require that the found Q track is alone with a margin
   * of 25 wires to each side in both chambers.
   *
   * Also require them to have at least 7 signals.
   */

#ifdef V5_DEBUG_PRINTING
  printf ("first_round_calibration\n");
#endif

  for (plane = 0; plane < 2; plane++)
    {
      // check all signals in first of type x and Y

#ifdef V5_DEBUG_PRINTING
      printf ("plane: %d,  [%d,%d]\n",
	      plane,tracks[0][plane]._ntracks,
	      tracks[1][plane]._ntracks);
#endif

      /* If the tracks have enough signals (say at least 5 in
       * total, excluding the peak), we can also fit the offset
       * and nonuniformity of the VDC TDCs.  This is possibly a
       * dangerous operation, since we do not want to give the
       * TDCs a total offset, but merely want to make them even
       * more equal, so that straight lines will show up even
       * more straight, allowing for better cuts.
       */

      for (vdc = 0; vdc < 2; vdc++)
	{      
	  for (t1 = 0; t1 < tracks[vdc][plane]._ntracks; t1++)
	    {
	      track1 = &tracks[vdc][plane]._track[t1];
	      
	      if (track1->_fit._rise._s +
		  track1->_fit._fall._s >= 6)
		continue;
	      
	      for (t = 0; t < tracks[vdc][plane]._ntracks; t++)
		{
		  if (t == t1)
		    continue;
		  
		  track = &tracks[vdc][plane]._track[t];
		  
		  if (fabs(track1->_param._w0 - 
			   track ->_param._w0) < 25)
		    goto try_next_t1_vdc_tdc;
		}
	      
	      fit_vdc_tdc(vdc,plane,*track1);
	      
	    try_next_t1_vdc_tdc:;
	    }
	}




      for (t1 = 0; t1 < tracks[0][plane]._ntracks; t1++)
	{
	  track1 = &tracks[0][plane]._track[t1];

#ifdef V5_DEBUG_PRINTING
	  printf ("Plane: %d (vdc: 0), attempt: %d  (%5.1f,%5.7f),%5.7f\n",
		  plane,t1,
		  track1->_param._w0,
		  track1->_param._t0,
		  track1->_param._s);
#endif

	  if (track1->_fit._rise._s +
	      track1->_fit._fall._s < FIRST_CALIB_Q_TRACK_MIN_SIGNALS)
	    goto try_next_t1;

	  for (t = 0; t < tracks[0][plane]._ntracks; t++)
	    {
	      if (t == t1)
		continue;

	      track = &tracks[0][plane]._track[t];

	      if (fabs(track1->_param._w0 - 
		       track ->_param._w0) < FIRST_CALIB_Q_TRACK_CLEAN)
		goto try_next_t1;

	      // The track may also not share signals with another track.
	      // This will basically not happen as they then would be
	      // rather close as well.
	      /*
	      if (uses_same_r_sigs(&track1->_r_sigs,
				   &track ->_r_sigs))
		goto try_next_t1;
	      */
	    }
	  
	  // find a matching track in other chamber, that also
	  // is alone

	  for (t2 = 0; t2 < tracks[1][plane]._ntracks; t2++)
	    {
	      track2 = &tracks[1][plane]._track[t2];
	      
#ifdef V5_DEBUG_PRINTING
	      printf ("Plane: %d (vdc: 1), attempt: %d  (%6.2f,%6.2f),%6.2f\n",
		      plane,t2,
		      track2->_param._w0,
		      track2->_param._t0,
		      track2->_param._s);
#endif

	      if (track2->_fit._rise._s +
		  track2->_fit._fall._s < FIRST_CALIB_Q_TRACK_MIN_SIGNALS)
		goto try_next_t1;

	      if (fabs(track1->_param._w0 - 
		       track2->_param._w0) > 25)
		continue;

	      for (t = 0; t < tracks[1][plane]._ntracks; t++)
		{
		  if (t == t2)
		    continue;

		  track = &tracks[1][plane]._track[t];
		  
		  if (fabs(track2->_param._w0 - 
			   track ->_param._w0) < FIRST_CALIB_Q_TRACK_CLEAN)
		    goto try_next_t2;
		  
		  // the track may also not share signals with another track
		  /*
		  if (uses_same_r_sigs(&track2->_r_sigs,
				       &track ->_r_sigs))
		    goto try_next_t2;
		  */
		}
	      
	      // Aha, so t1 and t2 should be matching tracks!
	      
	      {
		int index = add_calib1_qq_same(plane,
					       &track1->_param,
					       &track2->_param);

		qq_same_info *info = &qq_same[plane][num_qq_same[plane]++];

		info->_wire[0] = track1->_param._w0;
		info->_wire[1] = track2->_param._w0;
		info->_index   = index;
	      }

	      a = (track1->_param._w0-
		   track2->_param._w0);
	      dtdx1 = track1->_param._s;
	      dtdx2 = track2->_param._s;

#ifdef V5_DEBUG_PRINTING
	      printf ("*** Clean enough for first round calibration. ***\n");
#endif
	      printf ("%6.2f %6.4f %6.4f %5.2fs %5.1ft |"
		      "Cali%d:%2d-%2d: "
		      "%5.1f-%5.1f : %5.1f-%5.1f : %5.1f-%5.1f %.0f,%.0f/%.0f,%.0f"
		      "\n",
		      a,1/dtdx1,1/dtdx2,
		      log(track1->_param._s/track2->_param._s),
		      track1->_param._t0-track2->_param._t0,
		      plane,t1,t2,
		      track1->_param._w0,
		      track2->_param._w0,
		      track1->_param._s,
		      track2->_param._s,
		      track1->_param._t0,
		      track2->_param._t0,
		      track1->_fit._rise._s,
		      track1->_fit._fall._s,
		      track2->_fit._rise._s,
		      track2->_fit._fall._s);

	      // Slope is in s/mm Via a drifttime (mm/s) we get it
	      // back to mm.  We then have dz/dx.  Invert to get
	      // dx/dz.  Multiplied with the distance between the
	      // VDCs (z), this become an offset in wires.

	      // First make some sanity checks:

	      {
		double s_ratio = 
		  track1->_param._s /
		  track2->_param._s;

		if (s_ratio < 0.5 || s_ratio > 2)
		  {
		    printf ("Bad match...\n");

		    continue; /* Very bad match.  This means that
			       * we do not assume drift times to be
			       * this different!.
			       */
		  }
	      }

	      // Now we can do the fitting.

	      avfit[0][plane].fill(1/dtdx1,a);
	      avfit[1][plane].fill(1/dtdx2,a);

	      vect<2> param1;
	      vect<2> param2;

	      matrix_sym<2> var1;
	      matrix_sym<2> var2;

	      avfit[0][plane].get_param_var(param1,var1);
	      avfit[1][plane].get_param_var(param2,var2);
	      /*
	      printf ("(%6.1f,%6.2f,%6.1f,%6.3f,%6.2f)",
		      avfit[0][plane]._sum,
		      avfit[0][plane]._sum_x,
		      avfit[0][plane]._sum_y,
		      avfit[0][plane]._sum_x2,
		      avfit[0][plane]._sum_xy);
	      */

#ifdef V5_DEBUG_PRINTING
	      printf (" # %6.4f , %6.4f | %6.4f , %6.4f , %6.4f",
		      param1[0],param1[1],
		      sqrt(var1.get(0,0)),sqrt(var1.get(1,1)),
		      sqrt(avfit[0][plane].get_var_y(param1)));

	      printf (" # %6.4f , %6.4f | %6.4f , %6.4f , %6.4f\n",
		      param2[0],param2[1],
		      sqrt(var2.get(0,0)),sqrt(var2.get(1,1)),
		      sqrt(avfit[1][plane].get_var_y(param2)));
#endif
      
	    try_next_t2:;
	    }
 
	try_next_t1:;
	}
    }

  /* Now see if we have events where x and u can be matched together...
   *
   * We require u and x hit within
   */

#if 0
  printf ("XU: ");

  for (plane = 0; plane < 2; plane++)
    {
      for (int i = 0; i < num_qq_same[plane]; i++)
	{
	  qq_same_info *info = &qq_same[plane][i];

	  printf ("%d  %5.1f %7d  | ",plane,info->_wire,info->_index);
	}
    }

  printf ("\n");

  if (num_qq_same[0] == 1 && num_qq_same[1] == 1)
    {
      qq_same_info *info_x = &qq_same[0][0];
      qq_same_info *info_u = &qq_same[1][0];

      printf ("xu_diff: %6.1f  %6.1f\n",
	      info_x->_wire[0] - info_u->_wire[0],
	      info_x->_wire[1] - info_u->_wire[1]);
    }
#endif

  for (int i_x = 0; i_x < num_qq_same[0]; i_x++)
    for (int i_u = 0; i_u < num_qq_same[1]; i_u++)
      {
	qq_same_info *info_x = &qq_same[0][i_x];
	qq_same_info *info_u = &qq_same[1][i_u];

	double x_minus_u_0 = info_x->_wire[0] - info_u->_wire[0];
	double x_minus_u_1 = info_x->_wire[1] - info_u->_wire[1];

	if ((x_minus_u_0 > MIN_OVERLAP_X_MINUS_U &&
	     x_minus_u_0 < MAX_OVERLAP_X_MINUS_U) &&
	    (x_minus_u_1 > MIN_OVERLAP_X_MINUS_U &&
	     x_minus_u_1 < MAX_OVERLAP_X_MINUS_U))
	  {
	    /* Both hits match.  Accept.
	     */

	    int index1 = info_x->_index;
	    int index2 = info_u->_index;

	    calib1_xu entry;

	    entry._x_index = index1;
	    entry._u_index = index2;

	    _calib1_xu.push_back(entry);

	    calib1_qq_same &qq_same_x = (_calib1_qq_same[0])[index1];
	    calib1_qq_same &qq_same_u = (_calib1_qq_same[1])[index2];

	    printf ("xu_diff: %6.1f  %6.1f (%6.1f,%6.1f)=%6.1f  (%6.1f,%6.1f)=%6.1f  (%6.1f,%6.1f)\n",
		    info_x->_wire[0] - info_u->_wire[0],
		    info_x->_wire[1] - info_u->_wire[1],
		    qq_same_x._param[0]._t0,
		    qq_same_u._param[0]._t0,
		    (qq_same_x._param[0]._t0 -
		     qq_same_u._param[0]._t0),
		    qq_same_x._param[1]._t0,
		    qq_same_u._param[1]._t0,
		    (qq_same_x._param[1]._t0 -
		     qq_same_u._param[1]._t0),
		    (qq_same_x._param[0]._t0 -
		     qq_same_x._param[1]._t0),
		    (qq_same_u._param[0]._t0 -
		     qq_same_u._param[1]._t0));
	    
	  }
      }





  static int counter = 0;

  if ((counter++ & 0x7ff) == 0)
    calc_first_round_calibration();
  




  /*
    for (vdc = 0; vdc < 2; vdc++)
      {
	if (tracks[vdc][plane]._ntracks < 1)
	  return; // no track -> no calibration
	
	if (tracks[vdc][plane]._ntracks == 1)
	  {
	    // nice and clean
	    
	    break;
	}
	
	return;
      }
  */
  //  printf ("*** Clean enough for first round calibration. ***\n");
}

/******************************************************************/
