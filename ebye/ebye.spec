// -*- C++ -*-

// Not really needed as an external unpacker is used for all
#include "spec/midas.spec"

external EXT_EBYE_DATA();

SUBEVENT(EV_EVENT)
{
  external data = EXT_EBYE_DATA();
}


EVENT
{
  ev = EV_EVENT();
}
