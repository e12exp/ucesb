/* Template for an 'ext_data' reader.
 *
 * To generate the needed ext_h101.h file (ucesb), use e.g.
 *
 * empty/empty /dev/null --ntuple=UNPACK,STRUCT_HH,ext_h101.h
 *
 * Compile with (from unpacker/ directory):
 *
 * cc -g -O3 -o ext_reader_h101 -I. -Ihbook hbook/example/ext_data_reader.c hbook/ext_data_client.o
 */

#include "ext_data_client.h"

#include "../hbook/example/test_caen_v775_data.h"
#include "../hbook/example/test_caen_v1290_data.h"

/* Change these, here or replace in the code. */

#define EXT_EVENT_STRUCT_H_FILE       "ext_xtst_regress.h"
#define EXT_EVENT_STRUCT              EXT_STR_xtst_regress
#define EXT_EVENT_STRUCT_LAYOUT       EXT_STR_xtst_regress_layout
#define EXT_EVENT_STRUCT_LAYOUT_INIT  EXT_STR_xtst_regress_LAYOUT_INIT

/* */

#include EXT_EVENT_STRUCT_H_FILE

#include <stdlib.h>
#include <stdio.h>

int main(int argc,char *argv[])
{
  struct ext_data_client *client;

  EXT_EVENT_STRUCT event;
#if 0
  EXT_EVENT_STRUCT_LAYOUT event_layout = EXT_EVENT_STRUCT_LAYOUT_INIT;
#endif
  struct ext_data_structure_info *struct_info = NULL;
  int ok;

  uint64_t num_good = 0;
  uint64_t num_good_data = 0;

  if (argc < 2)
    {
      fprintf (stderr,"No server name given, usage: %s SERVER\n",argv[0]);
      exit(1);
    }

  struct_info = ext_data_struct_info_alloc();
  if (struct_info == NULL)
    {
      perror("ext_data_struct_info_alloc");
      fprintf (stderr,"Failed to allocate structure information.\n");
      exit(1);
    }

  EXT_STR_xtst_regress_ITEMS_INFO(ok, struct_info, EXT_EVENT_STRUCT, 1);
  if (!ok)
    {
      perror("ext_data_struct_info_item");
      fprintf (stderr,"Failed to setup structure information.\n");
      exit(1);
    }

  /* Connect. */
  
  client = ext_data_connect_stderr(argv[1]);

  if (client == NULL)
    exit(1);

  if (ext_data_setup_stderr(client,
#if 0 /* Do it by the structure info, so we can handle differing input. */
			    &event_layout,sizeof(event_layout),
#else
			    NULL, 0,
#endif
			    struct_info,/*NULL,*/
			    sizeof(event)))
    {
      /* Handle events. */
      
      for ( ; ; )
	{
	  /*uint32_t i;*/
	  int ok;

	  caen_v775_data v775a;
	  caen_v775_data v775b;

	  caen_v775_data v775a_good;
	  caen_v775_data v775b_good;

	  caen_v1290_data v1290a;
	  caen_v1290_data v1290b;

	  caen_v1290_data v1290a_good;
	  caen_v1290_data v1290b_good;

	  /* To 'check'/'protect' against mis-use of zero-suppressed
	   * data items, fill the entire buffer with random junk.
	   *
	   * Note: this IS a performance KILLER, and is not
	   * recommended for production!
	   */

	  ext_data_rand_fill(&event,sizeof(event));
	  
	  /* Fetch the event. */
	  
	  if (!ext_data_fetch_event_stderr(client,&event,sizeof(event)))
	    break;
	  
	  /* Do whatever is wanted with the data. */

	  /*
	  printf ("%10d: %2d\n",event.EVENTNO,event.TRIGGER);
	  */
	  /*
	  printf ("%d\n",event.regress_v775mod1n);
	  for (i = 0; i < event.regress_v775mod1n; i++)
	    {
	      printf ("%2d %d\n",
		      event.regress_v775mod1nI[i],
		      event.regress_v775mod1data[i]);
	    }
	  */
	  /*
	  printf ("h:%x\n",event.regress_v1290mod1header);
	  printf ("t:%x\n",event.regress_v1290mod1trigger);
	  printf ("n:%d\n",event.regress_v1290mod1n);
	  for (i = 0; i < event.regress_v1290mod1n; i++)
	    {
	      printf ("%d\n",
		      event.regress_v1290mod1data[i]);
	    }
	  */
	  
	  ok = 1;

	  /* */

	  ok &= fill_caen_v775_data(&v775a,
				    event.regress_v775mod1n,
				    event.regress_v775mod1nI,
				    event.regress_v775mod1data,
				    event.regress_v775mod1eob);
	  
	  ok &= fill_caen_v775_data(&v775b,
				    event.regress_v775mod2n,
				    event.regress_v775mod2nI,
				    event.regress_v775mod2data,
				    event.regress_v775mod2eob);

	  create_caen_v775_event(&v775a_good,
				 1, 0x80 - 1,
				 event.EVENTNO + 0xdef,
				 event.regress_seed);

	  create_caen_v775_event(&v775b_good,
				 2, 0x80 - 2,
				 event.EVENTNO + 0xdef,
				 event.regress_seed);

	  ok &= compare_caen_v775_event(&v775a_good, &v775a);
	  ok &= compare_caen_v775_event(&v775b_good, &v775b);

	  /* */

	  ok &= fill_caen_v1290_data(&v1290a,
				     event.regress_v1290mod1nM,
				     event.regress_v1290mod1nMI,
				     event.regress_v1290mod1nME,
				     event.regress_v1290mod1n,
				     event.regress_v1290mod1data,
				     event.regress_v1290mod1header,
				     event.regress_v1290mod1trigger);
	  
	  ok &= fill_caen_v1290_data(&v1290b,
				     event.regress_v1290mod2nM,
				     event.regress_v1290mod2nMI,
				     event.regress_v1290mod2nME,
				     event.regress_v1290mod2n,
				     event.regress_v1290mod2data,
				     event.regress_v1290mod2header,
				     event.regress_v1290mod2trigger);
	  
	  create_caen_v1290_event(&v1290a_good,
				  1,
				  event.EVENTNO + 0xdef,
				  event.regress_seed);

	  create_caen_v1290_event(&v1290b_good,
				  2,
				  event.EVENTNO + 0xdef,
				  event.regress_seed);

	  ok &= compare_caen_v1290_event(&v1290a_good, &v1290a);
	  ok &= compare_caen_v1290_event(&v1290b_good, &v1290b);

	  /* */

	  if (ok)
	    {
	      num_good++;
	      num_good_data += event.regress_v775mod1n;
	      num_good_data += event.regress_v775mod2n;
	      num_good_data += event.regress_v1290mod1n;
	      num_good_data += event.regress_v1290mod2n;
	    }
	  else
	    {
	      printf ("Error was in event %d.\n",event.EVENTNO);
	    }
	  
	  /* ... */
	}  
    }

  printf ("%" PRIu64 " events passed test (%" PRIu64 " words).\n",
	  num_good, num_good_data);

  ext_data_close_stderr(client);

  return 0;
}
