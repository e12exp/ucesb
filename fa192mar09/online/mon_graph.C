
// root -x mon_setup_compile.C+ -x mon_graph.C+

#include "mon_common.h"

#include "mon_defines.h"

#include "fa192_sadcp_onl.h"

void mon_graph()
{
  MON_SETUP;

  NEW_CANVAS(c1,"Sampling ADC 0");

  TH1I *line1 = new TH1I("Ch0","Ch0",2560,0,2560);

  // line1->SetLineColor(3);
  line1->Draw("");
  _hists.push_back(line1);

  NEW_CANVAS(c2,"Sampling ADC 1");

  TH1I *line2 = new TH1I("Ch1","Ch1",2560,0,2560);

  line2->SetLineColor(6);

  line2->Draw("");
  _hists.push_back(line2);

  MON_EVENT_LOOP_TOP(56002,EXT_STR_h101_onion,
		     EXT_STR_h101_layout,EXT_STR_h101_LAYOUT_INIT,e)
    {
      (void) last_update_events;

      if (e.sadcp[0]._ || e.sadcp[1]._)
	{
	  int rot = (e.sadcpp_trigrec & 0x007f)*20;

	  int minv[2] = { 0x1000, 0x1000} , maxv[2] = { 0, 0 };

	  for (unsigned int i = 0; i < e.sadcp[0]._; i++)
	    {
	      line1->Fill(((2560+1280+i+rot)%2560)+0.5,e.sadcp[0].v[i]);
	      if (e.sadcp[0].v[i] < minv[0]) minv[0] = e.sadcp[0].v[i];
	      if (e.sadcp[0].v[i] > maxv[0]) maxv[0] = e.sadcp[0].v[i];
	    }

	  for (unsigned int i = 0; i < e.sadcp[1]._; i++)
	    {
	      line2->Fill(((2560+1280+i+rot)%2560)+0.5,e.sadcp[1].v[i]);
	      if (e.sadcp[1].v[i] < minv[1]) minv[1] = e.sadcp[1].v[i];
	      if (e.sadcp[1].v[i] > maxv[1]) maxv[1] = e.sadcp[1].v[i];
	    }

	  mon_pads_modified_update_reset_hists();
	  // printf ("%d %04x %04x\n",e.TRIGGER,e.sadcpp_trigrec,e.sadcpp_vernier);
	  printf ("%5d %5d %5d   %5d %5d %5d\n",
		  minv[0],maxv[0],maxv[0]-minv[0],
		  minv[1],maxv[1],maxv[1]-minv[1]);

	}
    }
  MON_EVENT_LOOP_BOTTOM(e);
}

