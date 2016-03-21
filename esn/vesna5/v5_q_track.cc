
#include "vesna5.h"
#include "v5_track.h"

#include "v5_defines.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/*******************************************************************
 *
 * Routines for finding Q tracks (from raw data (points)).
 *
 * Major operating assumption is that the hits are sorted according
 * to wire (and time), and that we have an list with the indices
 * of the hits sorted according to time
 */

/*******************************************************************
 *
 * Compare VDC hits.  For sorting, ordering by wire and then time.
 */

int
compare_vdc_hit(const void* l,const void* r)
{
  vdc_hit* lhs = (vdc_hit *) l;
  vdc_hit* rhs = (vdc_hit *) r;

  int diff;

  diff = ((int) lhs->_wire) - ((int) rhs->_wire);
  if (diff)
    return diff;
  return ((int) lhs->_time) - ((int) rhs->_time);
}

/*******************************************************************
 *
 * Compare the times of two hits, and return value to sort them in
 * reversed order.  NOTE: compare_hits is a global, so vesna5 is not
 * reentrant!
 */

vdc_hit* compare_hits = NULL;

int
compare_peak_order(const void* l,const void* r)
{
  /* Use int and not word as we need signed-ness for the subtraction
   * to perform as expected (i.e. also giving negative results.
   */
  
  int *lhs = (int *) l;
  int *rhs = (int *) r;

  int l_time, r_time;

  assert (compare_hits);

  l_time = compare_hits[*lhs]._time;
  r_time = compare_hits[*rhs]._time;
  
  return r_time - l_time;
}

/******************************************************************/

typedef struct 
{
  int _cmp;
  int _rise;
  int _fall;
} rise_fall;

int
compare_rise_fall(const void* l,const void* r)
{
  rise_fall *lhs = (rise_fall *) l;
  rise_fall *rhs = (rise_fall *) r;

  return lhs->_cmp - rhs->_cmp;
}

/******************************************************************/

void
set_r_sig(r_track* r_sigs,int sig)
{
  r_sigs->_sigs++;
  r_sigs->_used[sig / (sizeof (unsigned int) * 8)] |= 
    (((unsigned int) 1) << (sig % (sizeof (unsigned int) * 8)));
  /*
  printf ("%d(%d,%d):--- %d : %08x\n",
	  sig,
	  sig / (sizeof (unsigned int) * 8),
	  sig % (sizeof (unsigned int) * 8),
	  r_sigs->_sigs,r_sigs->_used[0]);
  */
}

/*******************************************************************
 *
 * Find next possible rising peak (backwards).
 *
 * rise is index of current end of rising track.
 * first is next allowed suggestion. ()
 */

int
find_next_rise(int rise,int first,
	       vdc_hit* hit)
{
  for ( ; first >= 0; first--)
    {
      if (hit[first]._wire == hit[rise]._wire)
	continue; /* Same wire. */
      if (hit[first]._wire <  hit[rise]._wire - MAX_NO_SIGNAL_WIRES - 1)
	return -1; /* Too far away. */
      if (hit[first]._time >  hit[rise]._time)
	continue; /* Not rising. */
      return first; /* This could be a match. */
    }
  return -1;
}

/*******************************************************************
 *
 * Find next possible falling peak (forwards).
 */

int
find_next_fall(int fall,int first,
	       vdc_hit* hit,int nhits)
{
  for ( ; first < nhits; first++)
    {
      if (hit[first]._wire == hit[fall]._wire)
	continue; /* Same wire. */
      if (hit[first]._wire >  hit[fall]._wire + MAX_NO_SIGNAL_WIRES + 1)
	return nhits; /* Too far away. */
      if (hit[first]._time >  hit[fall]._time)
	continue; /* Not rising. */
      return first; /* This could be a match. */
    }
  return nhits;
}

/*******************************************************************
 *
 * Prototypes.
 */

void find_q_track(int rise,int peak,int fall,r_track* r_sigs,
		  vdc_hit* hit,int nhits,
		  q_tracks* tracks);

void find_q_track2(int rise,int peak,int fall,r_track* r_sigs,
		   q_track_fit* fit,
		   vdc_hit* hit,int nhits,
		   q_tracks* tracks);

/******************************************************************/

#define max(a,b) ((a)>(b)?(a):(b))

/*******************************************************************
 *
 * Find a Q track from raw hits.
 */

void 
find_Q_tracks(vdc_hit* hit,int nhits,q_tracks* tracks)
{
  /* We'll do the find R tracks, create from them E-tracks and then
   * combine into Q tracks in one sweep.
   *
   * For any normal event there is not very much noise, so the danger
   * of finding many random Q tracks is not very large.
   *
   * We cut early on hit combinations that hardly make a line.
   * 
   * Only pathological events with hit patterns that have hits that
   * may be part of several tracks at the same time.  This can however
   * be avoided by having the event unpacker reject times to close
   * together from the same wire.  If they are not allowed to be too
   * similar, then the many possible tracks will quickly be stopped by
   * requiring them to be straight.  (I.e. all permutations of hits
   * will not be alloved.
   *
   * xxx x     Three             xxxxxx  This could be interpreted
   *    xxx    possibilities.    xxxxxx  as very many possible 
   *    x  xxx                           tracks.                  
   *   x                      
   * 
   */

  /* For each track we require: All participant hits must come from
   * different wires.
   *
   * To be able to fit a Q track, we need 4 hits.  At least 2 in each
   * E track.
   *
   * Since we make cuts on that the rising and falling track should be
   * similar, we might cut away tracks when one of the hits is the one
   * closest to wire crossing, since this one might have a distorted
   * time.  We do not care...
   *
   * Since our aim is to find Q tracks, we should start the search by
   * looking for their most prominent structure: the intersection of a
   * rising and falling track.  I.e. lets attempt starting a search
   * for Q tracks around each structure in the raw signals where the
   * start signal has a time larger than two surrounding ones - the
   * peak.
   *
   * And to be really effective, start with the largest times as
   * attempted peaks.  Then if we later get a smaller time as a peak
   * that is already part of a track, we'll quickly abort the search
   * for that one.
   *
   * Hmm, looking for the peak signal could be dangerous.  But since
   * we allow for a few missing signals, the actual peak wire could be
   * missing and then one of the lines (E tracks) will contain our
   * 'assigned' peak.  The assigned peak is not used for anything
   * except as a way to keep track of things.  It does not affect the
   * fit.
   */

  int peak; /* Signal at peak. */

  int find_rise; /* Next(prev) signal on rising side. */
  int find_fall; /* Next signal on falling side. */

  int rise; /* Next(prev) signal on rising side. */
  int fall; /* Next signal on falling side. */

  word peak_time;
  word peak_wire;

  static int peak_order[MAX_VDC_HITS];

  int i, j, k;

  r_track r_sigs;

#define MAX_FIND_RISE_FALL (MAX_NO_SIGNAL_WIRES+1)*(MAX_SIGNALS_PER_VDC_WIRE)

  int rise_hit[MAX_FIND_RISE_FALL];
  int fall_hit[MAX_FIND_RISE_FALL];

  int rise_hits;
  int fall_hits;
  
  rise_fall comb[MAX_FIND_RISE_FALL*MAX_FIND_RISE_FALL];
  int ncomb;

  /* Since we need at least one signal on each side of the peak,
   * there is no point in trying the first and last signals as peak
   * signals...
   */

  /* In order to avoid confusion at other places, we'll always sort
   * the hits, even if there are less than 3.
   *
   * One should also do a search for unreasonable things, i.e. hits with
   * crazy wire numbers or hit times, or hits that are recorded multiple
   * times.  However, since we do not store into arrays, we can avoid
   * these tests.  (Chances are anyway slim that these spurios hits will
   * create any hit pattern that will be recognized).  NOTE: for
   * calibration, we'll be filling into arrays, so checking for valid
   * input should be done.  This is a job for the event unpacker.
   */

  /* Sort the hits according to wire. */

  qsort (hit,nhits,sizeof(vdc_hit),compare_vdc_hit);

  if (nhits < 3)
    return; /* No way to find a Q track here. */

  return;

  for (i = 0; i < nhits - 2; i++)
    peak_order[i] = i+1;

  assert (compare_hits == NULL);

  /* An array with indices into the hits array.  Sort it according
   * to hit time.
   */

  compare_hits = hit;
  qsort(peak_order,nhits-2,sizeof (int),compare_peak_order);
  compare_hits = NULL;
  
#ifdef V5_DEBUG_PRINTING
  printf ("order:");
  for (i = 0; i < nhits - 2; i++)
    printf (" %2d",peak_order[i]);
  printf ("\n");
#endif

  /* Do not search further than two signals of the end, since we also
   * need rise and fall signals.
   *
   * Try the hits in order of decreasing time.  I.e. first try the
   * hits most likely to be peaks.
   */

  for (k = 0; k < nhits - 2; k++)
    {
      peak = peak_order[k];
      
      peak_time = hit[peak]._time;
      peak_wire = hit[peak]._wire;

      /* Try all combinations of rising and falling...  But try them in
       * order of smallest total number of skipped wires, i.e. smallest
       * distance between rising and falling peak.
       *
       * As we want to do it in this (strange) order, I see no other
       * alternative than to simply first generate lists of the peaks
       * that may be considered, then create all the combinations, and
       * sort them before trying them.  The pain with this is that if we
       * really find something, which we usually will on the first
       * attempt if there is anything to find at all, then there will be
       * no reward in trying the following solutions...
       *
       * Assume there is one stray signal on each side, then we'll find
       * four signals, that combined give 16 combinations.  This is not
       * too bad.  We can live with that, as when we find something,
       * most of these combinations (9-1(found)) will be part of the
       * found track and short circuited away quickly, anyway before we
       * start doing too much track fitting.
       *
       * Worst case: 3 signals on three wires each direction: 9x9 signals
       * -> 81 combinations.  Also not a disaster.  Go for this.  Hopefully
       * it wont be too messy...
       */

      find_rise = peak; 
      rise_hits = 0;

      while ((find_rise = find_next_rise(peak,find_rise - 1,hit)) >= 0 &&
	     rise_hits < MAX_FIND_RISE_FALL - 1)
	{
	  rise_hit[rise_hits++] = find_rise;
	  //printf ("r:%d ",find_rise);
	}
      
      if (!rise_hits)
	continue; /* no way */

      find_fall = peak; 
      fall_hits = 0;

      while ((find_fall = find_next_fall(peak,find_fall + 1,hit,nhits)) < nhits &&
	     fall_hits < MAX_FIND_RISE_FALL - 1)
	{
	  fall_hit[fall_hits++] = find_fall;
	  //printf ("f:%d ",find_fall);
	}

      if (!fall_hits)
	continue; /* no way */

#ifdef V5_DEBUG_PRINTING
      printf ("r:");
      for (i = 0; i < rise_hits; i++)
	printf("%d ",rise_hit[i]);
      printf ("p:%d f:",peak);
      for (i = 0; i < fall_hits; i++)
	printf("%d ",fall_hit[i]);
      printf ("\n");
#endif

      ncomb = 0;

      for (i = 0; i < rise_hits; i++)
	for (j = 0; j < fall_hits; j++)
	  {
	    rise = rise_hit[i];
	    fall = fall_hit[j];

	    comb[ncomb]._cmp  = 
	      (hit[fall]._wire - hit[rise]._wire) * 256 +
	      max(hit[fall]._wire - peak_wire,
		  peak_wire - hit[rise]._wire);

	    comb[ncomb]._rise = rise;
	    comb[ncomb++]._fall = fall;
	    /*
	    printf ("%d,(%d,%d),(%d,%d)\n",
		    comb[ncomb-1]._cmp,comb[ncomb-1]._rise,comb[ncomb-1]._fall,
		    rise,fall);
	    */
	  }

      // printf ("%d combinations\n",ncomb);

      qsort(comb,ncomb,sizeof (*comb),compare_rise_fall);
	
      for (i = 0; i < ncomb; i++)
	{
	  rise = comb[i]._rise;
	  fall = comb[i]._fall;
	  
      /* 
      for (rise = peak - 1; rise >= 0; rise--)
	{
	  if (hit[rise]._wire == peak_wire)
	    continue; 
	  if (hit[rise]._wire <  peak_wire - MAX_NO_SIGNAL_WIRES - 1)
	    break;    
	  if (hit[rise]._time >  peak_time)
	    continue; 

	  for (fall = peak + 1; fall < nhits; fall++)
	    {
	      if (hit[fall]._wire == peak_wire)
		continue; 
	      if (hit[fall]._wire >  peak_wire + MAX_NO_SIGNAL_WIRES + 1)
		break;    
	      if (hit[fall]._time >  peak_time)
		continue; 
*/

#define HIT(n) hit[n]._wire,hit[n]._time

#ifdef V5_DEBUG_PRINTING
	  printf ("-----------------------------------------------\n");
	  
	  printf ("rpf=[%2d,%2d,%2d] (%3d,%3d) (%3d,%3d) (%3d,%3d) {%d}\n",
		  rise,peak,fall,
		  HIT(rise),HIT(peak),HIT(fall),
		  comb[i]._cmp);
#endif	  

	      /* So we have something that might be a Q track.  It
	       * should have the signals:
	       *
	       * @rise in the rising part.
	       * @fall in the falling part.
	       *
	       * @peak might be part of both, or just one.
	       *
	       * Also note that the @peak signal might be distorted
	       * (i.e.  not good for extrapolating), should the case
	       * be that the crossing of the plane of wires was close
	       * to the @peak wire.
	       */

	      /* The search is done in such a way that we first find
	       * @peak along with any closest @rise and @fall wires.
	       * Only after that do we start to find combinations
	       * where the @rise and/or @fall peaks have been skipped.
	       * When they are skipped, the possibility of starting to
	       * find the same track again (with some signals ignored)
	       * arises.  To avoid this, we wont allow to search for a
	       * next track if @rise, @peak, and @fall already is part
	       * of another track...
	       */

	      /*                                                 
	       *   x                           x                    
	       *          x      xx    xx                           
	       *    x                         x                  
	       *  x      x x    x        x      x               
	       */

	      /* With only three signals, we can hardly determine a
	       * first guess of the parameters of the track.  In
	       * principle we can, and perhaps should if we cannot
	       * find any further hits att all that have any chance of
	       * matching.
	       *
	       * However, life will be much simpler if we find another
	       * hit such that we do not need to use the @peak hit
	       * since it may be distorted.
	       *
	       * We should begin the search on the rising or falling
	       * side depending on which one is closest to @peak
	       * (i.e. has the largest value, and therefore the
	       * largest margin for having another hit
	       */
	      
	      memset (&r_sigs,0,sizeof (r_sigs));
	      
	      set_r_sig(&r_sigs,rise);
	      set_r_sig(&r_sigs,peak);
	      set_r_sig(&r_sigs,fall);

	      find_q_track(rise,peak,fall,&r_sigs,
			   hit,nhits,
			   tracks);
	}
    }
}

/*******************************************************************
 *
 * Add a point to a fit.  It is not very beautiful to have three
 * versions of almost the same function.
 */

void
set_fit_hit(linear_fit* fit,vdc_hit* hit)
{
  fit->_s  = 1;
  fit->_w  = hit->_wire;
  fit->_t  = hit->_time;
  fit->_wt = hit->_wire * hit->_time;
  fit->_ww = hit->_wire * hit->_wire;
}

void
copy_add_fit_hit(linear_fit* fit,const linear_fit* src,vdc_hit* hit)
{
  fit->_s  = src->_s  + 1;
  fit->_w  = src->_w  + hit->_wire;
  fit->_t  = src->_t  + hit->_time;
  fit->_wt = src->_wt + hit->_wire * hit->_time;
  fit->_ww = src->_ww + hit->_wire * hit->_wire;
}

void
add_fit_hit(linear_fit* fit,vdc_hit* hit)
{
  fit->_s  += 1;
  fit->_w  += hit->_wire;
  fit->_t  += hit->_time;
  fit->_wt += hit->_wire * hit->_time;
  fit->_ww += hit->_wire * hit->_wire;
}

/*******************************************************************
 *
 * Check if the proposed signals are entirely part of another track.
 */

int
part_of_track(const r_track* check,const r_track* other)
{
  unsigned int i;

  for (i = 0; i < R_TRACK_USED_SIZE; i++)
    if (check->_used[i] & ~(other->_used[i]))
      return 0; /* There was some bit set in check that was not in
		 * other, meaning that we're at least not entirely
		 * part of other.
		 */
  return 1;
}

int
part_of_tracks(const r_track* check,const q_tracks* tracks)
{
  int i;

  for (i = 0; i < tracks->_ntracks; i++)
    if (part_of_track(check,&tracks->_track[i]._r_sigs))
      return 1;
  return 0;
}

/*******************************************************************
 *
 * Find a fourth hit, so that we can determine the slope.
 */

void
find_q_track(int rise,
	     int peak,
	     int fall,
	     r_track* r_sigs,
	     vdc_hit* hit,
	     int nhits,
	     q_tracks* tracks)
{
  /* Try to find a fourth hit.  Preferably on the side with the
   * largest time so far.  But also at minimum wire skipping.  We
   * might have to try a few combinations, so simply loop through
   * everything.  (I.e. skipping wires with signals may be necessary.)
   */

  int diff_dists;

  int find_rise = find_next_rise(rise,rise - 1,hit);
  int find_fall = find_next_fall(fall,fall + 1,hit,nhits);

  q_track_fit q_fit;
  q_track_fit q_fit2;
  r_track r_sigs2;

  set_fit_hit(&q_fit._rise,&hit[rise]);
  set_fit_hit(&q_fit._fall,&hit[fall]);

  //printf ("-q-----------------\n");
  /*
  printf ("--- %d : %08x\n",r_sigs->_sigs,r_sigs->_used[0]);
  */
  for ( ; ; )
    {
#ifdef V5_DEBUG_PRINTING
      printf ("find:[%2d,%2d] ",find_rise,find_fall);
      if (find_rise >= 0)
	printf ("(%3d,%3d)",HIT(find_rise));
      else
	printf ("()");
      if (find_fall < nhits)
	printf ("(%3d,%3d)",HIT(find_fall));
      else
	printf ("()");
      printf ("\n");
#endif

      /* So make an attempt with what we have here...
       */

      /* Which should we use?  @find_rise or @find_fall
       * If one of them is invalid, the choice is easy.
       * Otherwise we prefer the one with least number of
       * skipped wires.  And finally we begin with the one
       * with smallest time.
       */

      if (find_rise < 0)
	{
	  if (find_fall >= nhits)
	    break; /* All possibilities now tested! */
	  goto test_falling;
	}
      if (find_fall >= nhits)
	goto test_rising;

      /* Both are valid. */

      diff_dists = 
	(hit[rise]._wire - hit[find_rise]._wire) -
	(hit[find_fall]._wire - hit[fall]._wire);

      /* Skip as few as possible. */

      if (diff_dists < 0)
	goto test_rising;
      if (diff_dists > 0)
	goto test_falling;

      diff_dists = 
	(hit[peak]._wire - hit[find_rise]._wire) -
	(hit[find_fall]._wire - hit[peak]._wire);

      /* Try side closest to peak. */

      if (diff_dists < 0)
	goto test_rising;
      if (diff_dists > 0)
	goto test_falling;

      /* Highest time?  I.e. closest to peak first. */

      if (hit[fall]._time > hit[rise]._time)
	goto test_falling;
      /* goto find_rising; */

    test_rising:
      /* If @find_rise, @rise, @peak, @fall already are part
       * of a track, then we have nothing useful to add...
       */

      r_sigs2 = *r_sigs;
      set_r_sig(&r_sigs2,find_rise);
      
      if (!part_of_tracks(&r_sigs2,tracks))
	{
	  /* Fit E track (slope & offset) to find_rise & rise,
	   * find offset of other E by fall
	   * and see if peak is in any way sane.
	   *
	   * If so, try to find all other signals.
	   */

	  /* We are really not interested in the offsets of the E
	   * tracks, but only of the slopes.  The offsets are only
	   * interesting in form of the intersection of the two
	   * tracks.  icpx and icpt, which are the searched for
	   * parameters of the Q track.
	   *
	   * So we should only fit the slope, and then fit the
	   * intercept.
	   *
	   * The slopes of the two parts is the same except for a very
	   * tiny correction for the flight time of the particle.
	   *
	   * With a drifttime about 0.05 mm/ns and a chamber half
	   * thickness of 15 mm this amounts to 300 ns maximum
	   * drifttime.  Normally about 5 wires are hit on each side
	   * giving a slope of 60 ns/wire.  Now assuming particles
	   * with half the speed of light, traversing the chamber
	   * takes 0.3 ns (effective length of 45 mm, and speed 150
	   * mm/ns).  This would make the furthest incoming track
	   * signal (300 ns) come 0.3 ns earilier compared to the zero
	   * time signal.  And for the outgoing particle this would be
	   * +0.3 ns.  So we would have slopes 59.94 ns/wire compared
	   * to 60.06 ns/wire, a difference of 0.002.  Does it make an
	   * additional difference that the incoming particle is
	   * directed towards the wires, while the hits in the
	   * outgoing track are directed from the wires.  Should not
	   * affect the slopes, as it will be a constant offset for
	   * incoming and another for outgoing.  But it would affect
	   * the calculated intercept, since it offsets incoming
	   * versus outgoing.
	   *
	   * After finding the tracks, one should check if there is
	   * any difference in the slopes of the incoming and outgoing
	   * tracks.
	   */

	  q_fit2._fall = q_fit._fall;
	  copy_add_fit_hit(&q_fit2._rise,&q_fit._rise,&hit[find_rise]);

	  //	  printf ("--- %d : %08x\n",r_sigs2._sigs,r_sigs2._used[0]);

	  find_q_track2(find_rise,peak,fall,&r_sigs2,
			&q_fit2,
			hit,nhits,
			tracks);
	}

      find_rise = find_next_rise(rise,find_rise - 1,hit);
      continue;

    test_falling:
      r_sigs2 = *r_sigs;
      set_r_sig(&r_sigs2,find_fall);
      
      if (!part_of_tracks(&r_sigs2,tracks))
	{
	  q_fit2._rise = q_fit._rise;
	  copy_add_fit_hit(&q_fit2._fall,&q_fit._fall,&hit[find_fall]);
	  
	  //printf ("--- %d : %08x\n",r_sigs2._sigs,r_sigs2._used[0]);

	  find_q_track2(rise,peak,find_fall,&r_sigs2,
			&q_fit2,
			hit,nhits,
			tracks);
	}

      find_fall = find_next_fall(fall,find_fall + 1,hit,nhits);
      continue;
    }
  //printf ("-q-ret-\n");
}




typedef struct
{
  double _value;
  double _std_err;
} val_err;

/*******************************************************************
 *
 * Calculate the parameters of a Q track, 
 * given the collected statistics.
 */

void
calc_q_track_fit(const q_track_fit* fit,q_track_param* param)
{
  /*
    t0 = (Qw*Qw*Rt + Qw*(-(Qt*Rw) - Q*Rwt) + Q*Qt*Rww + R*(Rw*Rwt - Rt*Rww))/
    (Qw*Qw*R - 2*Q*Qw*Rw + Q*Q*Rww + R*(Rw*Rw - R*Rww))
    
    s = (Qt*Qw*R - Q*Qw*Rt - Q*Qt*Rw + R*Rt*Rw + Q*Q*Rwt - R*R*Rwt)/
    (Qw*Qw*R - 2*Q*Qw*Rw + Q*Q*Rww + R*(Rw*Rw - R*Rww))
        
    w0 =(-Qt*Rw*Rw + Q*Rw*Rwt - Qw*(-Rt*Rw + R*Rwt) + Qt*R*Rww - Q*Rt*Rww)/
    (Q*Qw*Rt + Qt*(-(Qw*R) + Q*Rw) + Q*Q*Rwt - R*(Rt*Rw + R*Rwt))

    div1 = (Qw*Qw*R - 2*Q*Qw*Rw + Q*Q*Rww + R*(Rw*Rw - R*Rww)) =
    (Qw*Qw*R + R*Rw*Rw - 2*Q*Qw*Rw  + Q*Q*Rww - R*R*Rww) =
    R*(Qw*Qw+Rw*Rw)-2*Q*Qw*Rw+Rww*(Q*Q-R*R) = 
    4*Sw*Tw*R-2*Q*Qw*Rw-4*S*T*Rww

  */

#define S    (fit->_rise._s)
#define Sw   (fit->_rise._w)
#define St   (fit->_rise._t)
#define Swt  (fit->_rise._wt)
#define Sww  (fit->_rise._ww)

#define T    (fit->_fall._s)
#define Tw   (fit->_fall._w)
#define Tt   (fit->_fall._t)
#define Twt  (fit->_fall._wt)
#define Tww  (fit->_fall._ww)

  /* Some short hands. */

  double R = S + T;
  double Q = S - T;

  double Rt = St + Tt;
  double Qt = St - Tt;

  double Rw = Sw - Tw;
  double Qw = Sw + Tw;

  double Rwt = Swt - Twt;

  double Rww = Sww + Tww;

  /* Common */

  double QwRt = Qw * Rt;
  double QtRw = Qt * Rw;

  double QtQw = Qt * Qw;
  double RtRw = Rt * Rw;

  /* Parts of expressions. */

  double div1 =   R*(Qw*Qw+Rw*Rw) - 2*Q*Qw*Rw         - 4*S*T*Rww;
  double div2 =   R*(QtQw + RtRw) -   Q*(QwRt + QtRw) - 4*S*T*Rwt;
  /*
  printf ("%6.1f %6.1f %6.1f %6.1f %6.1f\n",S,Sw,St,Swt,Sww);
  printf ("%6.1f %6.1f %6.1f %6.1f %6.1f\n",T,Tw,Tt,Twt,Tww);
  */
  /* And calculate */

  param->_s  = div2 / div1;

  param->_t0 = ( Qw*(QwRt - QtRw) - Rwt*(Q*Qw - R*Rw) + Rww*(Q*Qt - R*Rt)) / div1;
  param->_w0 = ( Rw*(QwRt - QtRw) + Rwt*(Q*Rw - R*Qw) - Rww*(Q*Rt - R*Qt)) / div2;
    
  //printf ("param: (%5.1f,%5.7f),%5.7f\n",param->_w0,param->_t0,param->_s);

  /* There is always the question of how much to squeeze out the common
   * subexpressions - a good compiler will be able to do it...
   */ 

  /* And then we should also calculate the errors on the parameters, so they can be
   * used to extrapolate and figure out if hits may or may not be part of the track.
   */

#undef S    
#undef St   
#undef Sw   
#undef Stw  
#undef Sww  

#undef T    
#undef Tt   
#undef Tw   
#undef Ttw  
#undef Tww  
}




val_err
q_track_expect_rising(const q_track_param* param,
		      word wire)
{
  val_err ve;

  ve._std_err = 0;
  
  ve._value = param->_t0 - param->_s * (param->_w0 - wire);

  return ve;
}

val_err
q_track_expect_falling(const q_track_param* param,
		       word wire)
{
  val_err ve;

  ve._std_err = 0;

  ve._value = param->_t0 - param->_s * (wire - param->_w0);

  return ve;
}



typedef struct
{
  int _rise;
  int _fall;
  int _find_rise;
  int _find_fall;
  r_track       _r_sigs;
  q_track_fit   _fit;
  q_track_param _param;
} backtrack_q_info;

#define MAX_BACKTRACK  5

/*******************************************************************
 *
 * Find a fourth hit, so that we can determine the slope.
 */

void
find_q_track2(int rise, 
	      int peak,
	      int fall, 
	      r_track* r_sigs,
	      q_track_fit* fit,
	      vdc_hit* hit,
	      int nhits,
	      q_tracks* tracks)
{
  /* So the signals rise and fall should each be part of lines
   * with slope and -slope.  We want to calculate their intercept.
   */

  backtrack_q_info backtrack[MAX_BACKTRACK];
  int nbacktracks = 0;

  int diff_dists;

  q_track_param param;

  /* Now we want to add more points to the track.
   *
   * We differentiate between three possibilities:  
   *
   * If a point agrees well to the track estimate so far, and the estimate is
   * based on at least five points, then the point is added.  The point may not
   * be jumped over.
   *
   * If we not yet have five points in the estimate, then we can do two things,
   * either add points or not.  This gives more permutations.
   *
   * Important is that the permutations are generated in order such that
   * the most dense (tracks with least points jumped over) tracks are
   * generated first.  If we are about to generate a new track, then we require
   * that at least one point differ from the tracks already found.
   */

  int find_rise = find_next_rise(rise,rise - 1,hit);
  int find_fall = find_next_fall(fall,fall + 1,hit,nhits);

  //printf ("-q2----------------\n");
  //printf ("edges:[%2d,%2d,%2d]\n",rise,peak,fall);

  calc_q_track_fit(fit,&param);
  
  for ( ; ; )
    {
      // printf ("param: (%5.1f,%5.7f),%5.7f\n",param._w0,param._t0,param._s);

#ifdef V5_DEBUG_PRINTING
      printf ("find:[%2d,%2d] ",find_rise,find_fall);
      if (find_rise >= 0)
	printf ("(%3d,%3d)",HIT(find_rise));
      else
	printf ("()");
      if (find_fall < nhits)
	printf ("(%3d,%3d)",HIT(find_fall));
      else
	printf ("()");
      printf ("\n");
#endif

      /* So make an attempt with what we have here...
       */

      /* Which should we use?  @find_rise or @find_fall
       * If one of them is invalid, the choice is easy.
       * Otherwise we prefer the one with least number of
       * skipped wires.  
       */

      if (find_rise < 0)
	{
	  if (find_fall >= nhits)
	    goto all_found; /* All possibilities now tested. */
	  goto test_falling;
	}
      if (find_fall >= nhits)
	goto test_rising;
      
      /* Both valid. */

      diff_dists = 
	(hit[peak]._wire - hit[find_rise]._wire) -
	(hit[find_fall]._wire - hit[peak]._wire);

      if (diff_dists < 0)
	goto test_rising;
      if (diff_dists > 0)
	goto test_falling;

      diff_dists = 
	(hit[rise]._wire - hit[find_rise]._wire) -
	(hit[find_fall]._wire - hit[fall]._wire);

      //      if (diff_dists < 0)
      //	goto test_rising;
      if (diff_dists > 0)
	goto test_falling;

      //      if (hit[peak]._wire - hit[find_rise]._wire <
      //        hit[find_fall]._wire - hit[peak]._wire)
      //   goto test_falling;
      /* goto find_rising; */

    test_rising:
      /* Is the found item anywhere close to where we expect it? */

      {
	val_err expect = q_track_expect_rising(&param,hit[find_rise]._wire);

	double diff = expect._value - hit[find_rise]._time;

#ifdef V5_DEBUG_PRINTING
	printf ("diff-r %d: %6.1f\n",find_rise,diff);
#endif

	if (fabs(diff) < (expect._std_err + VDC_TIME_STD_ERR) * VDC_FIT_TOL)
	  {
	    /* Hit was inside possible range. */

	    /* If it not a too good hit, or we do not yet have enough
	     * statistics to have an good extrapolation, we also have
	     * the possibility of not using this hit.  This possibility
	     * should however be explored after exploring the one with
	     * this hit.
	     *
	     * If there are more hits on the same wire, the should get the
	     * same chance.
	     */

	    if (nbacktracks < MAX_BACKTRACK &&
		(fit->_rise._s + fit->_fall._s < 5 ||
		 fabs(diff) > ((expect._std_err + VDC_TIME_STD_ERR) * 
			      VDC_GOOD_FIT_TOL) ||
		 (find_rise > 0 && 
		  hit[find_rise-1]._wire == hit[find_rise]._wire)))	
	      {
		backtrack[nbacktracks]._rise = rise;
		backtrack[nbacktracks]._fall = fall;
		backtrack[nbacktracks]._find_rise = 
		  find_next_rise(rise,find_rise-1,hit);
		backtrack[nbacktracks]._find_fall = find_fall;
		backtrack[nbacktracks]._r_sigs = *r_sigs;
		backtrack[nbacktracks]._fit  = *fit;
		backtrack[nbacktracks]._param = param;
		nbacktracks++;
	      }

	    /* So it should be added. */
	    
	    add_fit_hit(&fit->_rise,&hit[find_rise]);
	    set_r_sig(r_sigs,find_rise);
	    //printf ("--- %d : %08x\n",r_sigs->_sigs,r_sigs->_used[0]);
	    
	    /* Recalculate the fit parameters. */
	    
	    calc_q_track_fit(fit,&param);

	    rise = find_rise; /* Last rising hit in track. */
	  }
	find_rise = find_next_rise(rise,find_rise - 1,hit);	    
	continue;
      }
    test_falling:
      {
	val_err expect = q_track_expect_falling(&param,hit[find_fall]._wire);

	double diff = expect._value - hit[find_fall]._time;
	
#ifdef V5_DEBUG_PRINTING
	printf ("diff-f %d: %6.1f\n",find_fall,diff);
#endif

	if (fabs(diff) < (expect._std_err + VDC_TIME_STD_ERR) * VDC_FIT_TOL)
	  {
	    /* Hit was inside possible range. */

	    /* If it not a too good hit, or we do not yet have enough statistics
	     * to have an good extrapolation, we also have the possibility of
	     * not using this hit.  This possibility should however
	     * be explored after exploring the one with this hit.
	     */

	    if (nbacktracks < MAX_BACKTRACK &&
		(fit->_rise._s + fit->_fall._s < 5 ||
		 fabs(diff) > ((expect._std_err + VDC_TIME_STD_ERR) *
			      VDC_GOOD_FIT_TOL) ||
		 (find_fall < nhits-1 && 
		  hit[find_fall+1]._wire == hit[find_fall]._wire)))
	    {
		backtrack[nbacktracks]._rise = rise;
		backtrack[nbacktracks]._fall = fall;
		backtrack[nbacktracks]._find_rise = find_rise;
		backtrack[nbacktracks]._find_fall = 
		  find_next_fall(fall,find_fall+1,hit,nhits);
		backtrack[nbacktracks]._r_sigs = *r_sigs;
		backtrack[nbacktracks]._fit  = *fit;
		backtrack[nbacktracks]._param = param;

		nbacktracks++;
	      }

	    /* So it should be added. */
	    
	    add_fit_hit(&fit->_fall,&hit[find_fall]);
	    set_r_sig(r_sigs,find_fall);
	    //printf ("--- %d : %08x\n",r_sigs->_sigs,r_sigs->_used[0]);
	    
	    /* Recalculate the fit parameters. */
	    
	    calc_q_track_fit(fit,&param);

	    fall = find_fall; /* Last rising hit in track. */
	  }
	find_fall = find_next_fall(fall,find_fall+1,hit,nhits);	    
	continue;
      }

    all_found:

      /* Nothing else to add.  The Q track is finished.
       */
      
      /* Then, when everything has been found, one can make a more
       * informed decision of whether and how to include the @peak signal.
       *
       * What should have been done before is to make sure that the peak
       * hit is somewhere close to being valid.
       */
      
      /* Include the peak hit into the fit.  Perform corrections for @peak
       * hit being close to a wire.
       */
      
      /* Add the track to the list of known ones.
       */

      if ((param._s < 40 && r_sigs->_sigs <= 5) ||
	  (param._s < 30 && r_sigs->_sigs <= 4))
	{
	  // printf ("Discarded: bad slope!\n",rise,peak,fall);
	}
      else if (part_of_tracks(r_sigs,tracks))
	{
	  // printf ("Discarded: part of known!\n",rise,peak,fall);
	}
      else
	{
	  // printf ("Q TRACK FOUND:[%2d,%2d,%2d]\n",rise,peak,fall);

	  if (tracks->_ntracks == MAX_Q_TRACKS)
	    {
	      /* Either we say that this should not happen, or we
	       * should get rid of the most unlikely one.  E.g. the one
	       * with fewest hits.
	       */
	      
	      /* For now:  Be brutal!  Please remove this!
	       */
	      
	      fprintf (stderr,
		       "Error: Found more than MAX_Q_TRACKS.\n"
		       "Fix the source!\n");
	      
	      exit(1); /* Have fun! */
	    }
	  /*
	  printf ("--- %d : %08x : ",r_sigs->_sigs,r_sigs->_used[0]);
	  print_r_sigs(r_sigs);
	  printf ("\n");
	  */
	  tracks->_track[tracks->_ntracks]._peak   = peak;

	  tracks->_track[tracks->_ntracks]._r_sigs = *r_sigs;

	  tracks->_track[tracks->_ntracks]._fit    = *fit;
	  tracks->_track[tracks->_ntracks]._param  = param;

	  tracks->_ntracks++;
	}      

      /* And now try to back up and see if there is any further
       * possibilites that should be tested by leaving some hits out.
       */
      
      if (nbacktracks)
	{
#ifdef V5_DEBUG_PRINTING
	  printf ("Backtracking...\n");
#endif	  

	  nbacktracks--;
	  rise      = backtrack[nbacktracks]._rise;
	  fall      = backtrack[nbacktracks]._fall;
	  
	  find_rise = backtrack[nbacktracks]._find_rise;
	  find_fall = backtrack[nbacktracks]._find_fall;
	  
	  *r_sigs   = backtrack[nbacktracks]._r_sigs;
	  *fit      = backtrack[nbacktracks]._fit;
	  param     = backtrack[nbacktracks]._param;
	}
      else
	break; /* We're done. */

    }
#ifdef V5_DEBUG_PRINTING
  printf ("-q2-ret-\n");
#endif
}

/******************************************************************/
