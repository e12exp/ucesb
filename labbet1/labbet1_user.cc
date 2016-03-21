
#include "structures.hh"

#include "user.hh"

void user_function(unpack_event *event,
		   raw_event    *raw_event,
		   cal_event    *cal_event)
{
  /*
  printf ("%7d: %5d %5d %5d %5d\n",
	  event->event_no,
	  raw_event->SCI[0][0].T,
	  raw_event->SCI[0][1].T,
	  raw_event->SCI[0][0].E,
	  raw_event->SCI[0][1].E);
  */
  /*
  printf ("tdc0: "); event->vme.tdc0.data.dump(); printf ("\n");
  printf ("qdc0: "); event->vme.qdc0.data.dump(); printf ("\n");
  printf ("adc0: "); event->vme.adc0.data.dump(); printf ("\n");
  printf ("scaler: "); event->vme.scaler0.data.dump(); printf ("\n");
  */
  /*
  volatile int j = 0;

  for (int i = 0; i < 2000; i++)
    j = ((j * 3) + 1) ^ j;
  */
}
