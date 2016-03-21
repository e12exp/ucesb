
#include "structures.hh"

#include "event_base.hh"

uint32 MWPC::has_align32_word() const
{
  if (header.wc & 1)
    return 1;
  return 0;
}

uint32 VDC::has_align32_word() const
{
  if (header.wc & 1)
    return 1;
  return 0;
}


