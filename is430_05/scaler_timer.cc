// This file comes from CVS: isodaq/checker/scaler_timer.cc

#include "scaler_timer.hh"
#include "util.hh"

#include <stdio.h>
#include <limits.h>

scaler_timer::scaler_timer()
{
  bits_mask = 0;
  current0 = 0;
  current0_valid = 0;

  hist_i = 0;
}

void scaler_timer::set0(uint32 new0)
{
  current0 = new0;
  current0_valid = true;

  //printf ("%2d: %10d\n",(hist_i) & (ST_HISTORY_SIZE - 1),new0);

  history[(hist_i++) & (ST_HISTORY_SIZE - 1)] = new0;
  /*
  for (int i = 0; i < ST_HISTORY_SIZE; i++)
    printf ("+ %2d: %10d\n",i,history[i]);
  */
}

bool scaler_timer::set0_estimate(uint32 prev,uint32 now)
{
  //printf ("Estimate: prev: %10d  now: %10d\n",prev,now);

  // Analyse our history

  if (hist_i < ST_HISTORY_SIZE)
    return false;

  uint32 diff[ST_HISTORY_SIZE-1];
  /*
  for (int i = 0; i < ST_HISTORY_SIZE; i++)
    printf ("%2d: %10d\n",i,history[i]);
*/
  uint32 min_diff = INT_MAX;

  for (int i = 0; i < ST_HISTORY_SIZE-1; i++)
    {
      int i1 = (hist_i + i    ) & (ST_HISTORY_SIZE - 1); 
      int i2 = (hist_i + i + 1) & (ST_HISTORY_SIZE - 1);

      diff[i] = (history[i2] -
		 history[i1]) & bits_mask;
      /*
      printf ("%2d %2d  %10d %10d  %10d\n",
	      i1,i2,
	      history[i1],
	      history[i2],
	      diff[i]);
      */
      if (diff[i] < min_diff)
	min_diff = diff[i];
    }

  // Now see if all differences can be suspected to be integer
  // multiples of the minimum found.

  for (int i = 0; i < ST_HISTORY_SIZE-1; i++)
    {
      double multiplier = (double) diff[i] / min_diff;

      UNUSED(multiplier);

      //printf ("%6.1f ",multiplier);
    }
  //printf ("\n");
    
  
  return false;
}

uint32 scaler_timer::diff(uint32 now)
{
  //printf ("%d : %10d - %10d = %10d\n",(int) current0_valid,now,current0,(now - current0) & bits_mask);
  if (current0_valid)
    return (now - current0) & bits_mask;
  else
    return 0;
}
