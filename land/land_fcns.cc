
#include "structures.hh"

#include "land_triggers.hh"

#include "event_base.hh"
#include "worker_thread.hh"

uint32 LAND_CAMAC_SCALER::has_timestamp() const
{
  if (CURRENT_EVENT->_unpack.trigger == LAND_EVENT_SPILL_BEGIN ||
      CURRENT_EVENT->_unpack.trigger == LAND_EVENT_SPILL_END ||
      CURRENT_EVENT->_unpack.trigger == LAND_EVENT_START_ACQ ||
      CURRENT_EVENT->_unpack.trigger == LAND_EVENT_STOP_ACQ)
    return 1;
  return 0;
}
