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
 * ASCII art display of an event.
 */

void
display_event_chamber(v5_event_t* event,int vdc,int plane)
{
#define WIDTH   (240)
#define HEIGHT  (20/2)
#define MAXT     350

  char img[4][HEIGHT][WIDTH+2];
  char dump[WIDTH];
  
  memset (dump,0,WIDTH);
  
  for (int y = 0; y < HEIGHT; y++)
    {
      memset (img[plane][y],' ',WIDTH);
      img[plane][y][WIDTH] = '\n';
      img[plane][y][WIDTH+1] = 0;
    }
  
  for (int h = 0; h < event->_vdc_nhits[vdc][plane]; h++)
    {
      vdc_hit hit = event->_vdc_hit[vdc][plane][h];
      
      int time = hit._time;
      int wire = hit._wire;
      
      //printf ("%4d ",time);
      
      int x = wire;
      int y = (time * HEIGHT) / MAXT;
      int yleft = ((time - (y * MAXT) / HEIGHT)*3) / (MAXT / HEIGHT+1);
      
      if (time < 0)
	img[plane][0][x] = 'u';
      else if (time >= MAXT)
	img[plane][HEIGHT-1][x] = 'o';
      else 
	{
	  char tkn = (yleft > 1 ? '_' : (yleft > 0 ? '-' : '^'));
	  
	  if (img[plane][y][x] == ' ')
	    img[plane][y][x] = tkn;
	  else if (img[plane][y][x] != tkn)
	    img[plane][y][x] = 'x';		      
	}
      dump[x] = 1;
    }
  
	/*	
	pResult->TrackList.GoToHead();

	for (int i = pResult->nNoTracks; i; i--)
	  {
	    ResultListEPT		pTrackData;
	    
	    pTrackData = pResult->TrackList.GetData();
	    
	    float wire;
	
	    switch(plane)
	      {
	      case 0:
		wire = ( pTrackData->Track( VDC1|DMX|ICPT ) );
		break;
	      case 1:
		wire = ( pTrackData->Track( VDC1|DMU|ICPT ) );
		break;
	      case 2:
		wire = ( pTrackData->Track( VDC2|DMX|ICPT ) );
		break;
	      case 3:
		wire = ( pTrackData->Track( VDC2|DMU|ICPT ) );
		break;
	      }

	    float time = pTrackData->Track( TIME );

	    printf ("Hit: w=%.1f t=%.1f\n",wire,time);
	    
	    int x = (int) (wire);
	    int y = (int) ((time * HEIGHT) / MAXT);
	    
	    if (x < 0) x = 0;
	    if (x >= WIDTH) x = WIDTH-1;
	    if (y < 0) y = 0;
	    if (y >= HEIGHT) y = HEIGHT-1;
	    
	    img[plane][y][x] = 'X';
	    dump[x] = 1;
	    
	    pResult->TrackList.Next();
	  }
	*/
  
  for (int x = 0; x < WIDTH-2; x++)
    dump[x] |= dump[x+1] | dump[x+2];
  for (int x = WIDTH-1; x >= 2; x--)
    dump[x] |= dump[x-1] | dump[x-2];
  
  for (int x = 0; x < WIDTH; x++)
    {
      if (dump[x] && (x == 0 || !dump[x-1]))
	printf ("%03d",x);
      else if (x > 0 && dump[x-1])
	printf (" ");
    }
  printf ("\n");
  
  /*  for (int plane = 0; plane < 4; plane++)
      {*/
      for (int x = 0; x < WIDTH; x++)
	{
	  if (dump[x])
	    printf ("-");
	  else if (x > 0 && dump[x-1])
	    printf ("   ");
	}
      printf (": %d\n",plane);
      
      for (int y = 0; y < HEIGHT; y++)
	{
	  for (int x = 0; x < WIDTH; x++)
	    {
	      if (dump[x])
		printf ("%c",img[plane][y][x]);
	      else if (x > 0 && dump[x-1])
		printf ("   ");
	    }
	  printf ("\n");
	}
      /*}*/
}

/******************************************************************/

void
display_event(v5_event_t* event)
{
  int vdc, plane;

  for (vdc = 0; vdc < 2; vdc++)
    {      
      for (plane = 0; plane < 2; plane++)
	{
	  display_event_chamber(event,vdc,plane);
	}
    }
      
      
      
      
      printf ("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	      "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" "\n");
    /*
    printf ("%d:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	    //"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	    //"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	    //"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	    //"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" "\n",pResult->nNoTracks);
    */

    fflush (stdout);
}

/******************************************************************/

