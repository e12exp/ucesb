#include "vesna5.h"
#include "v5_track.h"

#include "v5_defines.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


int
compare_vdc_hit(const void* l,const void* r);

template<typename T>
int compare_values(const void* a,const void* b)
{
  if (*((const T*) a) < *((const T*) b))
    return -1;
  return *((const T*) a) > *((const T*) b);
}

// #define MAX_VDC_HITS  128

typedef struct
{
  float _slope;

  short int _wire1;
  short int _wire2;
} slope_info;

slope_info rising[MAX_VDC_HITS * 3 * 3];
slope_info falling[MAX_VDC_HITS * 3 * 3];

int num_rising;
int num_falling;

void 
find_Q_tracks2(vdc_hit* hit,int nhits,q_tracks* tracks)
{
#if 0

  /* Sort the hits according to wire. */

  qsort (hit,nhits,sizeof(vdc_hit),compare_vdc_hit);

  /* Then calculate differences that may be part of rising and falling
   * tracks.
   *
   * We at most handle 3 missing wires.
   */

  num_rising  = 0;
  num_falling = 0;

  for (int i = 0; i < nhits - 1; i++)
    {
      int j = i+1;

      for ( ; j < nhits && hit[j]._wire <= hit[i]._wire; j++)
	;
      
      for ( ; j < nhits; j++)
	{
	  int w_diff = hit[j]._wire - hit[i]._wire;

	  if (w_diff >= 4)
	    break;

	  int t_diff = hit[j]._time - hit[i]._time;

	  double slope = (double) t_diff / (double) w_diff;

	  if (slope > 0)
	    {
	      if (slope < 20 || slope > 200)
		continue; // impossible slope

	      slope_info *info = &rising[num_rising++];

	      info->_slope = slope;
	      info->_wire1 = i;
	      info->_wire2 = j;
	    }
	  else
	    {
	      slope = -slope;

	      if (slope < 20 || slope > 200)
		continue; // impossible slope

	      slope_info *info = &falling[num_falling++];

	      info->_slope = slope;
	      info->_wire1 = i;
	      info->_wire2 = j;
	    }
	}	 
    }

  // Then we sort the rising and falling entries

  qsort(rising ,num_rising, sizeof(rising [0]),compare_values<float>);
  qsort(falling,num_falling,sizeof(falling[0]),compare_values<float>);

  printf ("------------------------------------------------------------\n");

  for (int i = 0; i < num_rising; i++)
    printf ("%7.2f(%d)",rising[i]._slope,hit[rising[i]._wire2]._wire-hit[rising[i]._wire1]._wire);
  printf ("\n");

  for (int i = 0; i < num_falling; i++)
    printf ("%7.2f(%d)",falling[i]._slope,hit[falling[i]._wire2]._wire-hit[falling[i]._wire1]._wire);
  printf ("\n");

#endif
}


