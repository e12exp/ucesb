// -*- C++ -*-

external EXT_CROS3(ccb_id);

SUBEVENT(CROS3_SUBEVENT)
{
  select several
    {
      norevisit external ccb[0] = EXT_CROS3(ccb_id=1);
      norevisit external ccb[1] = EXT_CROS3(ccb_id=2);
    }
}

#include "cros3_v27rewrite.spec"

SUBEVENT(CROS3_REWRITE_SUBEVENT)
{
  select several
    {
      norevisit ccb[0] = CROS3_REWRITE(ccb_id=1);
      norevisit ccb[1] = CROS3_REWRITE(ccb_id=2);
    }
}

