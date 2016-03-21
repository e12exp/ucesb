#include "vesna5.h"
#include "v5_track.h"
#include "v5_defines.h"
#include "v5_calib.h"
#include "v5_match.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/* vesna5 is not reentrant.  Global variables are used to speed things
 * up.  The consequence of this is that you can only process one event
 * at a time.  This should not be a big problem.  
 */

/******************************************************************/


/******************************************************************/

void
print_r_sigs(r_track* r_sigs)
{
  int sig;

  for (sig = 0; sig < MAX_VDC_HITS; sig++)
    {
      if (r_sigs->_used[sig / (sizeof (unsigned int) * 8)] &
	  (((unsigned int) 1) << (sig % (sizeof (unsigned int) * 8))))
	printf ("%d ",sig);
    }
}


/******************************************************************/

q_tracks tracks[2][2];

v5_event_t* _v5_event;

/* Routine to match VDC hits together.  
 *
 * This is quick and dirty, and should in now way be part of a final,
 * frequently used release, since things can be done SO much better.
 *
 * Eg. one should be able to deal with tracks that are not only full 4
 * hit Q tracks (one per chamber), but also other nastinesses.
 *
 * In short: a few more calibration parameters should be taken into
 * account, so that the very hard cuts that are made here could be
 * loosened.
 */

double calib_mult[2][2] = { { 2550 , 2650 } , { 2450 , 2550 } };
double calib_add [2][2] = { { -46  , -47  } , { -37  , -38  } };

/* The expected log(s1/s2) (difference of the slopes)
 */

#define LOG_S_SIGMA   0.1

void
find_tracks()
{
  /* Let's be nasty: try to find tracks that match according to slope
   * and projection to the other chamber.  If there are more than two
   * possibilities reject (this is nasty, should be relaxed by making
   * judgement on more features, like time, FPP continuation...)
   */

  int plane;

  for (plane = 0; plane < 2; plane++)
    {
      /* Try to match all tracks against each other.  If there are more
       * than one possible match in the other chamber, there is no match
       * at all!  (this must! be relaxed, or we cannot find some of the
       * very beautiful events that are almost completely overlapping an
       * e.g. X, but separated in U)
       */

      for (int t0 = 0; t0 < tracks[0][plane]._ntracks; t0++)
	{
	  q_track& track0 = tracks[0][plane]._track[t0];

	  for (int t1 = 0; t1 < tracks[1][plane]._ntracks; t1++)
	    {
	      q_track& track1 = tracks[1][plane]._track[t1];
	      
	      /* The slopes should be more or less matching! */

	      if (fabs (track0._param._s - track1._param._s) > 20)
		continue;

	      // /* Average slope as expected slope. */
	      // double s = (track0._param._s + track1._param._s) / 2;

	      double w_dist = track0._param._w0 - track1._param._w0;

	      double dtdx1 = track0._param._s;
	      double dtdx2 = track1._param._s;

	      double w_dist_expect_0 = calib_add[0][plane] + calib_mult[0][plane] / dtdx1;
	      double w_dist_expect_1 = calib_add[1][plane] + calib_mult[1][plane] / dtdx2;
	      
	      // The standard error in projection is around 4 wires

	      if (fabs (w_dist_expect_0 - w_dist) > 15 ||
		  fabs (w_dist_expect_1 - w_dist) > 15)
		continue;

	      /* So we believe that the tracks should match.  We know
	       * have to do some checking to see that neither of the
	       * tracks had a previous assumed match.  
	       */
	       
	      printf("Match: %c Q%02d - Q%02d\n",plane == 0 ? 'X' : 'U',t0,t1);
	      
	    }
	}
  }
  printf("Match: -----\n");
}

/******************************************************************/

#include <sys/time.h>

extern v5_conf_t _v5_conf;

extern "C" void 
vesna5_init()
{
  // calib_init();
  match_init();

  cal_vdc_tdc_init(_v5_conf._cal_vdc_tdc_prefix);
}

extern "C" void 
vesna5_exit()
{
  //int vdc, plane;

  //for (vdc = 0; vdc < 2; vdc++)
  //for (plane = 0; plane < 2; plane++)
  //cal_vdc_tdc_exit(vdc,plane);
  
  cal_vdc_tdc_exit();
}

extern "C" void 
vesna5(v5_event_t* event)
{
  int vdc, plane;
  /*
  static struct timeval time1;
  static struct timeval time2;
  double elapse_in, elapse_out;
  gettimeofday(&time1,NULL);
  */

  /* The first operations is to find all possible Q tracks. 
   *
   * To find a track at all, we must have a Q track to start
   * with.  An E track does not give enough information.
   */

  // q_tracks tracks[2][2];

  // display_event(event);

  _v5_event = event;
  
  for (vdc = 0; vdc < 2; vdc++)
    for (plane = 0; plane < 2; plane++)
      {
	tracks[vdc][plane]._ntracks = 0;

	// display_event_chamber(event,vdc,plane);

	find_Q_tracks(event->_vdc_hit  [vdc][plane],
		      event->_vdc_nhits[vdc][plane],
		      &tracks[vdc][plane]);

	find_Q_tracks2(event->_vdc_hit  [vdc][plane],
		       event->_vdc_nhits[vdc][plane],
		       &tracks[vdc][plane]);

	/*
	cal_vdc_tdc(vdc,plane,
		    event->_vdc_hit  [vdc][plane],
		    event->_vdc_nhits[vdc][plane]);

	*/
#ifdef V5_DEBUG_PRINTING_FOUND_TRACKS
	printf ("VDC: %d %d : %d Q tracks\n",
		vdc,plane,tracks[vdc][plane]._ntracks);

	int t1;

	for (t1 = 0; t1 < tracks[vdc][plane]._ntracks; t1++)
	  {
	    printf ("  Track: %d  (%6.2f,%6.2f),%6.2f | ",
		    t1,
		    tracks[vdc][plane]._track[t1]._param._w0,
		    tracks[vdc][plane]._track[t1]._param._t0,
		    tracks[vdc][plane]._track[t1]._param._s);

	    print_r_sigs (&tracks[vdc][plane]._track[t1]._r_sigs);

	    printf ("\n");
	  }
#endif /* V5_DEBUG_PRINTING */
      }	

  /* For calibrating first round, we need (very) clear signals.  This
   * is to avoid misinterpretations.  This way, we can easily
   * calibrate the parameters without having to care too much about
   * background.
   *
   * Then for the second round, we can use more nasty signals, since
   * with the first parameters, we'll be able to make better cuts on
   * what is possible and not.  This will improve accuracy.
   *
   * So first we aim to get ball-park values.  One proton events are
   * nice (hmm, DSP code might prevent this)...  So separted events
   * are nice...
   */

  // first_round_calibration();

  /*
  elapse_out = time1.tv_sec - time2.tv_sec + (time1.tv_usec - time2.tv_usec) * 0.000001;
  gettimeofday(&time2,NULL);
  elapse_in = time2.tv_sec - time1.tv_sec + (time2.tv_usec - time1.tv_usec) * 0.000001;
 
  printf ("Elapse:  %.3f | %.3f => %.1f%%\n",
	  elapse_in,elapse_out,
	  elapse_in/(elapse_in+elapse_out)*100);
  */
  // find_tracks();

  vdc_tdc_calib();

#if 0
  match_tracks();
#endif


}

/******************************************************************/


#ifdef TEST

void
add_vdc_hit(event_t* event,
	    int vdc,int plane,word wire,word time)
{
  int hit = event->_vdc_nhits[vdc][plane]++;

  event->_vdc_hit[vdc][plane][hit]._wire = wire;
  event->_vdc_hit[vdc][plane][hit]._time = time;
}

main()
{

  event_t event;

  memset (&event,0,sizeof (event));

  add_vdc_hit(&event,0,0,  98,105);
  add_vdc_hit(&event,0,0,  99,120);
  add_vdc_hit(&event,0,0, 100,140);
  add_vdc_hit(&event,0,0, 101,120);
  add_vdc_hit(&event,0,0, 102,100);

  vesna5(&event);
}

#endif/* NTEST */
