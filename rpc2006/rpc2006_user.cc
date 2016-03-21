
#include "structures.hh"

#include "user.hh"

void user_function(unpack_event *event,
		   raw_event    *raw_event,
		   cal_event    *cal_event)
{
  printf ("%7d: %5d %5d %5d %5d\n",
	  event->event_no,
	  raw_event->SCI[0][0].T.value,
	  raw_event->SCI[0][1].T.value,
	  raw_event->SCI[0][0].E.value,
	  raw_event->SCI[0][1].E.value);
  /*
  printf ("tdc0: "); event->vme.tdc0.data.dump(); printf ("\n");
  printf ("qdc0: "); event->vme.qdc0.data.dump(); printf ("\n");
  printf ("adc0: "); event->vme.adc0.data.dump(); printf ("\n");
  printf ("scaler: "); event->vme.scaler0.data.dump(); printf ("\n");
  */
}
