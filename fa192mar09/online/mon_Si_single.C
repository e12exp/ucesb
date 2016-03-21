
// root -x mon_setup_compile.C+ -x mon_graph.C+

#include "mon_common.h"

#include "mon_defines.h"

#include "fa192_SSSD_LU_onl.h"

void mon_Si_single()
{
  MON_SETUP;

  gStyle->SetPalette(1);

  //NEW_CANVAS(c1,"Si strip singles");

  // TH1I *line1 = new TH1I("Ch1","Ch1",2560,0,2560);

  //TH2I* cross = new TH2I("Image","Image",32,1,33,32,1,33);
  
  NEW_CANVAS(c1,"Si strip 1 energies");

  for (int i = 0; i < 32; i++)
    {
      PAD_CD_ADD(c1,i);

      TH1I *line1 = new TH1I("Ch1","Ch1",2560,0,2560);

    } 
  
  //cross->Draw("colz");
  //_hists.push_back(cross);

  MON_EVENT_LOOP_TOP(56001,EXT_STR_h101_onion,
		     EXT_STR_h101_layout,EXT_STR_h101_LAYOUT_INIT,e)
    {
      /*
      if (e.SSSD[0]._ == 1 && e.SSSD[1]._ == 1)
	{
	  cross->Fill(e.SSSD[0].I[0]+.5,e.SSSD[1].I[0]+.5);
	}
      */
      MON_EVENT_LOOP_UPDATE(e,2,1);
      /*
      if (e.TRIGGER == 2)
	{
	  last_update_events;

	  c1->Modified();
	  c1->Update();

	  cross->Reset();

	  printf (".. %d",e.EVENTNO);
	}
      */
    }
  MON_EVENT_LOOP_BOTTOM(e);
}

