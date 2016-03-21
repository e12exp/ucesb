// -*- C++ -*-

#include "spec/spec.spec"

#include "land_camac_converter.spec"
#include "land_camac_scaler.spec"

#include "land_fastbus.spec"

#include "land_vme.spec"

#include "land_type_subtype.spec"

EVENT
{
  // Globally will be added (always) (from event header):
  // TRIG, EVENTNO

  camac       = LAND_CAMAC_CONVERTER(type    = SUBEVENT_CAMAC,
				     subtype = SUBEVENT_CAMAC_CONVERTERS);
#if 0
  camac_scalers  = LAND_CAMAC_SCALER(type    = SUBEVENT_START_STOP_ACQ,
				     subtype = SUBEVENT_CAMAC_SCALERS);
#endif
  camac_scalers  = LAND_CAMAC_SCALER(type    = SUBEVENT_CAMAC,
				     subtype = SUBEVENT_CAMAC_SCALERS);
  fastbus_error = LAND_FASTBUS_ERROR(type    = SUBEVENT_FASTBUS_ERROR,
				     subtype = SUBEVENT_FASTBUS_ERROR_INFO);
  fastbus             = LAND_FASTBUS(type    = SUBEVENT_FASTBUS,
				     subtype = SUBEVENT_FASTBUS_DATA1);
}

#include "mapping_beam.hh"
#include "mapping_land.hh"
#include "mapping_tdet.hh"

SIGNAL(ZERO_SUPPRESS: N1_1); // zero suppress along the second index
SIGNAL(ZERO_SUPPRESS: V1_1); // zero suppress along the second index
SIGNAL(ZERO_SUPPRESS: TFW1); // zero suppress along the first index
SIGNAL(ZERO_SUPPRESS: TOF1); // zero suppress along the first index
SIGNAL(ZERO_SUPPRESS: CS1_1);
SIGNAL(ZERO_SUPPRESS: CV1_1);
SIGNAL(ZERO_SUPPRESS: GFI1_1);
SIGNAL(ZERO_SUPPRESS: FGR1_1);

