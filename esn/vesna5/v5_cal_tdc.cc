#include "vesna5.h"
#include "v5_calib.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <float.h>

#include "fit_linear.hh"

#include "mille_output.hh"

/******************************************************************/

/* The TDCs used for the VDC (LeCroy 3377) use a FPGA with an
 * oscillator as clock (and some interpolation) to measure the time
 * between the common signals and the individual channels.
 *
 * As the osciallator is the same for each module, the 32 channels
 * (say 16 heare for ease) of each module cannot have any frequency
 * (gain) differences, only different offsets.  Between the modules we
 * should however also investigate any gain differences.
 *
 * So with each chunk of 16 channels, we only need to calculate any
 * unexpected offsets, and only on the borders, the possible offset
 * and gain has to be matched.  This entire calibration is a perfect
 * case for the millipede program.
 */

/******************************************************************/

struct rise_fall_R_stretch
{
  int h[3];
  double h2_diff;
};

void
find_rise_fall_R_stretch(vdc_hit* hit,int first,int last,
			 rise_fall_R_stretch stretches[2])
{
  int h1, h2, h3;

  for (h1 = first; h1 < last - 2; h1++)
    {
      word h1_wire = hit[h1]._wire;

      for (h2 = h1 + 1; h2 < last - 1; h2++)
	{
	  word h2_wire = hit[h2]._wire;
	  
	  if (h2_wire == h1_wire) // doublet hits in wire for h1
	    break;
	  if (h2_wire > h1_wire + 3) // h2 wire too far away
	    break;

	  for (h3 = h2 + 1; h3 < last; h3++)
	    {
	      word h3_wire = hit[h3]._wire;
	      
	      if (h3_wire == h2_wire) // doublet hits in wire for h2
		break;;
	      if (h3_wire > h1_wire + 4) // h3 wire too far away from h1
		break;
	      
	      if (h3 < last - 1 &&
		  hit[h3+1]._wire == h3_wire) // doublet hits in wire for h3
		continue; 

	      // so, we have three hits, that are alone in their wires
	      // assume this can make a line...

	      word h1_time = hit[h1]._time;
	      word h2_time = hit[h2]._time;
	      word h3_time = hit[h3]._time;

	      int rising;

	      if (h1_time < h2_time - 15 &&
		  h2_time < h3_time - 15)
		rising = 1;
	      else if (h1_time > h2_time + 15 &&
		       h2_time > h3_time + 15)
		rising = 0;
	      else
		continue; // neither rising nor falling

	      // How well does h2 time match with expectations?

	      double expect_h2_time = h1_time + 
		(double) (h3_time - h1_time) * 
		(double) (h2_wire - h1_wire) / (h3_wire - h1_wire);

	      double h2_diff = fabs(h2_time - expect_h2_time);

	      if (h2_diff < stretches[rising].h2_diff)
		{
		  stretches[rising].h2_diff = h2_diff;
		  stretches[rising].h[0] = h1;
		  stretches[rising].h[1] = h2;
		  stretches[rising].h[2] = h3;
		}
	    }
	}
    }

}

struct cal_R_stretch
{
  int nh;
  int h[20];

  double t0;
  double dtdw;

  int used;
};

mille_file *mille_files[2][3] = { { NULL,NULL,NULL }, { NULL,NULL,NULL } };
int use_mille[2] = { 0,0 };

void
evaluate_rise_fall_stretch(int vdc,int plane,
			   vdc_hit* hit,int first,int last,
			   rise_fall_R_stretch *stretch,
			   cal_R_stretch *cal)
{
  //printf ("%6.1f\n",stretch->h2_diff);

  /*
	      printf ("%3d %3d %3d : ",h1_wire,h2_wire,h3_wire);

	      printf ("%3d %3d (%6.1f / %6.1f) %3d\n",
		      h1_time,h2_time,expect_h2_time,
		      h2_time-expect_h2_time,h3_time);
  */

  cal->nh = 0;

  // First include the three points we know about

  fit_linear fit;

  for (int i = 0; i < 3; i++)
    {
      vdc_hit* h = &hit[stretch->h[i]];
      
      fit.fill(h->_wire,h->_time);
      cal->h[cal->nh++] = stretch->h[i];
    }

  // Try to include further points.  Going outwards from the center.
  // note that we must also try inbetween the known hits, as we may
  // have skipped wires there before.  (we do not care about the
  // risk of including several hits for one wire)

  double t0   = fit.y0();
  double dtdw = fit.dydx();

  for (int i = stretch->h[0]+1; i < stretch->h[2]; i++)
    if (i != stretch->h[1])
      {
	// Does the hit fit with a tolerance of +/- 10. ?
	// Then include it in further fitting
	
	vdc_hit* h = &hit[i];

	if (fabs(h->_time - (t0 + dtdw * h->_wire)) < 10.0)
	  {
	    fit.fill(h->_wire,h->_time);
	    cal->h[cal->nh++] = i;
	  }
      }

  //printf ("%6.0f\n",fit.sum());

  for (int o = 1; ; o++)
    {
      t0   = fit.y0();
      dtdw = fit.dydx();

      int i1 = stretch->h[0]-o;
      int i2 = stretch->h[2]+o;

      if (i1 < first && i2 >= last)
	break;

      if (i1 >= first)
	{
	  vdc_hit* h = &hit[i1];
	  
	  if (fabs(h->_time - (t0 + dtdw * h->_wire)) < 10.0)
	    {
	      fit.fill(h->_wire,h->_time);
	      if (cal->nh < 20)
		cal->h[cal->nh++] = i1;
	    }
	}
      if (i2 < last)
	{
	  vdc_hit* h = &hit[i2];
	  
	  if (fabs(h->_time - (t0 + dtdw * h->_wire)) < 10.0)
	    {
	      fit.fill(h->_wire,h->_time);
	      if (cal->nh < 20)
		cal->h[cal->nh++] = i2;
	    }
	}
    }

  cal->t0   = t0;
  cal->dtdw = dtdw;
  cal->used = 0;

#if 0
  //printf ("%6.0f\n",fit.sum());

  Mille *mille_file;

  if (!mille_files[vdc][plane])
    {
      char filename[128];
      
      sprintf (filename,"mille_%d_%d.bin",vdc,plane);

      mille_files[vdc][plane] = new Mille(filename);
    }
  
  mille_file = mille_files[vdc][plane];

  for (int i = first; i < last; i++)
    {
      vdc_hit* h = &hit[i];

      double expect = (t0 + dtdw * h->_wire);

      double diff = h->_time - expect;

      if (fabs(diff) < 25.0)
	{
	  /*
	  printf ("%d/%d/%03d : %6.1f = %3d - %6.1f\n",
		  vdc,plane,h->_wire,
		  diff,h->_time,expect);
	  */

	  float derLc[2] = { 1, h->_wire };
	  float derGl[1] = { 1 };
	  int   label[1] = { 1000 + h->_wire };
	  
	  mille_file->mille(2,derLc,
			    1,derGl,label,
			    h->_time - expect,1);
	}
    }

  mille_file->end();
#endif
}

/******************************************************************/

void 
cal_vdc_tdc(int vdc,int plane,vdc_hit* hit,int nhits,
	    cal_R_stretch cal[100][2],int *ncal)
{
  // For the calibration, we need to find R-tracks.  The hit with
  // largest time value (i.e. smallest time - common stop) is not
  // included, as it usually is affected by it's closeness to the
  // wires.  With four entries in the fit, quite some verification of
  // the fit being reasonable is possible.

  // Take an 'odd' approach to the participating hit finding.  As we
  // only want to calibrate, we can throw many events away...  First
  // find three hits in three unique wires, which are at most five
  // wires apart.  If they somehow manage to look like a line, then we
  // try to extend the line with other surrounding hits (possibly with
  // wires that have more then one hit.)

  *ncal = 0;

  for (int i = 0; i < nhits; )
    {
      word prev_wire = hit[i]._wire;

      int j = i + 1;

      for ( ; j < nhits; j++)
	{
	  word next_wire = hit[j]._wire;

	  if (next_wire > prev_wire + 3)
	    break;
	  prev_wire = next_wire;
	}
      
      // So, stretch of indices i .. j-1 is a range which has no holes
      // larger than 3 wires.  Within that, find (if any) the best
      // line fitting a rising and falling slope

      //printf ("%2d .. %2d (%3d .. %3d)\n",
      //	      i,j,hit[i]._wire,hit[j-1]._wire);

      rise_fall_R_stretch stretches[2];

      for (int rising = 0; rising < 2; rising++)
	stretches[rising].h2_diff = DBL_MAX;

      find_rise_fall_R_stretch(hit,i,j,stretches);

      if (stretches[0].h2_diff < 10.0 ||
	  stretches[1].h2_diff < 10.0)
	{
	  cal[*ncal][0].nh = 0;
	  cal[*ncal][1].nh = 0;

	  for (int rising = 0; rising < 2; rising++)
	    if (stretches[rising].h2_diff < 10.0)
	      evaluate_rise_fall_stretch(vdc,plane,hit,i,j,
					 &stretches[rising],
					 &cal[*ncal][rising]);

	  (*ncal)++;
	}

      i = j;
    }

  /*

*/
}

/******************************************************************/

void 
cal_vdc_tdc_init(const char *prefix)
{
  for (int vdc = 0; vdc < 2; vdc++)
    for (int u = 0; u < 3; u++)
      {
	char filename[256];
	
	sprintf (filename,"%s.vdc%d_%d.bin",
		 prefix ? prefix : "def",vdc,u);
	
	mille_files[vdc][u] = new mille_file;
	mille_files[vdc][u]->open(filename);
      }

}

void 
cal_vdc_tdc_exit()
{
  for (int vdc = 0; vdc < 2; vdc++)
    for (int u = 0; u < 3; u++)
      {
	delete mille_files[vdc][u];
      }
}

/******************************************************************/

extern v5_event_t* _v5_event;

struct cal_Q_stretch
{
  cal_R_stretch *R[2];

  double tx;
  double wx;

  int good;
  int used;
};

void
vdc_tdc_cal_add_eqn(int vdc,
		    cal_Q_stretch *q0,
		    cal_Q_stretch *q1,
		    vdc_hit* hit0,
		    vdc_hit* hit1)
{
  cal_Q_stretch *q[2] = { q0, q1 };
  vdc_hit* hit[2] = { hit0, hit1 };

  double tx = 0;

  if (q0)
    tx += q0->tx;
  if (q1)
    tx += q1->tx;

  tx /= (!!q0 + !!q1);

  int wx_off[2];

  wx_off[0] = 1;
  wx_off[1] = wx_off[0] + !!q0;

  int dtdw_off[2][2];

  dtdw_off[0][0] = (q0 && q1) ? 3 : 2;
  dtdw_off[0][1] = dtdw_off[0][0] + 1;

  dtdw_off[1][0] = dtdw_off[0][0] + 2 * !!q0;
  dtdw_off[1][1] = dtdw_off[0][1] + 2 * !!q0;

  //printf ("%6.1f\n",tx);
  /*
  printf ("%d %d : %d  %d %d  %d %d %d %d\n",
	  !!q0,!!q1,nL,
	  wx_off[0],wx_off[1],
	  dtdw_off[0][0],dtdw_off[0][1],
	  dtdw_off[1][0],dtdw_off[1][1]);
  */
  mille_file *mille_file = mille_files[vdc][use_mille[vdc]];

  use_mille[vdc] = (use_mille[vdc]+1)%3;

  mille_record record;

  record.reset();

  for (int plane = 0; plane < 2; plane++)
    if (q[plane])
    for (int r = 0; r < 2; r++)
      {
	cal_R_stretch *R = q[plane]->R[r];

	R->used = 1;

	//printf ("%d:%d %d %8.1f %6.1f\n",plane,r,R->nh,R->t0,R->dtdw);

	for (int i = 0; i < R->nh; i++)
	  {
	    vdc_hit *h = hit[plane]+R->h[i];
	    /*
	    printf ("%d:%d:%d  w%03d t%03d",
		    plane,r,i,h->_wire,h->_time);
	    */
	    if (h->_time > q[plane]->tx - 20)
	      {
		// hit close to intersection, bad fit
		//printf ("*\n");
	      }
	    else
	      {
		double expect_t = tx + R->dtdw * (h->_wire - q[plane]->wx);

		//printf ("- %6.1f = %6.1f",expect_t,h->_time - expect_t);

		// local variables:
		//
		// tx, wx[p=0,1], dtdw[p=0,1][r=0,1] 

		mille_lbl_der local[3] = {
		  { 1 ,                   1.0 }, // tx
		  { 1+wx_off[plane],      (h->_wire - q[plane]->wx)}, // wx[p]
		  { 1+dtdw_off[plane][r], -R->dtdw}, // dtdw[p][r]
		};
		  
		mille_lbl_der global[1] = {
		  { 10000 + 1000 * plane + h->_wire , 1.0 },
		};

		record.add_eqn(3,local,
			       1,global,
			       h->_time - expect_t,1);

		//printf ("\n");
	      }
	  }
      }

  mille_file->write(record);
}


void
vdc_tdc_cal_add_eqn(int vdc,int plane,
		    cal_R_stretch *r,
		    vdc_hit* hit)
{

  double woff = -r->t0 / r->dtdw;
  
  //printf ("%8.1f %6.1f -> %6.1f\n",r->t0,r->dtdw,woff);

  int cut_t = 0;

  for (int i = 0; i < r->nh; i++)
    {
      vdc_hit *h = hit+r->h[i];

      if (h->_time > cut_t)
	cut_t = h->_time;
      /*
      printf ("  %d:%d  w%03d t%03d\n",
	      plane,i,h->_wire,h->_time);
      */
    }

  cut_t -= 20;
  

  mille_file *mille_file = mille_files[vdc][use_mille[vdc]];

  use_mille[vdc] = (use_mille[vdc]+1)%3;

  mille_record record;

  record.reset();



  for (int i = 0; i < r->nh; i++)
    {
      vdc_hit *h = hit+r->h[i];
      /*
      printf ("   %d:%d  w%03d t%03d",
	      plane,i,h->_wire,h->_time);
      */
      if (h->_time >= cut_t)
	{
	  // hit close to possible intersection, bad fit
	  //printf ("*\n");
	}
      else
	{
	  double expect_t = 0 + r->dtdw * (h->_wire - woff);
	  
	  //printf ("- %6.1f = %6.1f",expect_t,h->_time - expect_t);
	  
	  // local variables:
	  //
	  // tx, wx[p=0,1], dtdw[p=0,1][r=0,1] 
	  
	  mille_lbl_der local[3] = {
	    { 1 , 1.0 },               // tx
	    { 2 , (h->_wire - woff) }, // dtdw
	  };
	  
	  mille_lbl_der global[1] = {
	    { 10000 + 1000 * plane + h->_wire , 1.0 },
	  };
	  
	  record.add_eqn(2,local,
			 1,global,
			 h->_time - expect_t,1);
	  
	  // printf ("\n");
	}
    }

  mille_file->write(record);
}


void
vdc_tdc_calib()
{
  int vdc, plane;

  for (vdc = 0; vdc < 2; vdc++)
    {
      int ncal = 0;
      cal_R_stretch cal[2][100][2];

      for (plane = 0; plane < 2; plane++)
	{
	  cal_vdc_tdc(vdc,plane,
		      _v5_event->_vdc_hit  [vdc][plane],
		      _v5_event->_vdc_nhits[vdc][plane],
		      cal[plane],&ncal);
	  
	}

      //printf ("- %d ------------------------------------------------\n",vdc);

      int nq[2] = { 0, 0 };
      cal_Q_stretch q[2][100];

      for (plane = 0; plane < 2; plane++)
	for (int i = 0; i < ncal; i++)
	  {
	    //printf ("%d:%d ",plane,i);

	    for (int r = 0; r < 2; r++)
	      {
		//printf ("%d(",cal[plane][i][r].nh);

		if (cal[plane][i][r].nh)
		  {
		    /*
		    printf ("%8.1f %6.1f",
			    cal[plane][i][r].t0,
			    cal[plane][i][r].dtdw);
		    */

		  }
		//printf (")");
	      }	

	    if (cal[plane][i][0].nh &&
		cal[plane][i][1].nh )
	      {
		// rising and falling, calculate intersect

		cal_R_stretch *a = &cal[plane][i][0]; 
		cal_R_stretch *b = &cal[plane][i][1]; 

		double tx = 
		  (a->t0 * b->dtdw - b->t0 * a->dtdw) / (b->dtdw - a->dtdw);
		double wx = (b->t0 - a->t0) / (a->dtdw - b->dtdw);

		//printf (" => %6.1f %6.1f",tx,wx);

		q[plane][nq[plane]].R[0] = a;
		q[plane][nq[plane]].R[1] = b;

		q[plane][nq[plane]].tx = tx;
		q[plane][nq[plane]].wx = wx;

		q[plane][nq[plane]].good = 
		  (cal[plane][i][0].nh >= 4 &&
		   cal[plane][i][1].nh >= 4);

		q[plane][nq[plane]].used = 0;

		nq[plane]++;
	      }
		
	    //printf ("\n");
	  }

      //printf ("%d %d\n",nq[0],nq[1]);
      
      for (int q1 = 0; q1 < nq[0]; q1++)
	{
	  int q2 = -1;
	  int matches = 0;

	  for (int q2t = 0; q2t < nq[1]; q2t++)
	    if (fabs(q[0][q1].tx - q[1][q2t].tx) < 10)
	      {
		q2 = q2t;
		matches++;
	      }

	  if (matches == 1 &&
	      q[0][q1].good && q[1][q2].good)
	    {
	      // So we have a unique match within 10 ns, q1 - q2
	      /*
	      printf ("m: (%d,%d) %6.1f %6.1f\n",
		      q1,q2,
		      q[0][q1].tx,q[1][q2].tx);
	      */

	      q[0][q1].used = 1;
	      q[1][q2].used = 1;

	      vdc_tdc_cal_add_eqn(vdc,
				  &q[0][q1],&q[1][q2],
				  _v5_event->_vdc_hit[vdc][0],
				  _v5_event->_vdc_hit[vdc][1]);

	    }
	}

      for (plane = 0; plane < 2; plane++)
	for (int q0 = 0; q0 < nq[plane]; q0++)
	  if (q[plane][q0].good &&
	      !q[plane][q0].used)
	    {
	      vdc_tdc_cal_add_eqn(vdc,
				  plane == 0 ? &q[0][q0] : NULL,
				  plane == 1 ? &q[1][q0] : NULL,
				  _v5_event->_vdc_hit[vdc][0],
				  _v5_event->_vdc_hit[vdc][1]);

	    }
      
      for (plane = 0; plane < 2; plane++)
	for (int i = 0; i < ncal; i++)
	  for (int r = 0; r < 2; r++)
	    if (cal[plane][i][r].nh >= 5 &&
		!cal[plane][i][r].used)
	      {
		// printf ("%d:%d /%d\n",vdc,plane,cal[plane][i][r].nh);
		
		vdc_tdc_cal_add_eqn(vdc,plane,
				    &cal[plane][i][r],
				    _v5_event->_vdc_hit[vdc][plane]);
		
		/*
		  printf ("%8.1f %6.1f",
		  cal[plane][i][r].t0,
		  cal[plane][i][r].dtdw);
		*/
	      }
    }
}

