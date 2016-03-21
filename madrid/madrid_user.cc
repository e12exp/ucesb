
#include "structures.hh"

#include "user.hh"

void user_function(unpack_event *event,
		   raw_event    *raw_event)
{
  /*
  printf ("%7d: %5d %5d %5d %5d\n",
	  event->event_no,
	  raw_event->SCI[0][0].T,
	  raw_event->SCI[0][1].T,
	  raw_event->SCI[0][0].E,
	  raw_event->SCI[0][1].E);
  */
  
  // printf ("adc0: "); event->ev.adc[0].data.dump(); printf ("\n");
  
}
