// -*- C++ -*-

#include "spec/spec.spec"

external EXTERNAL_DATA_SKIP();

SUBEVENT(XTST_TRIG10)
{
  external data = EXTERNAL_DATA_SKIP(); 
}

SUBEVENT(XTST_TRIG1)
{

}

SUBEVENT(XTST_TRIG14)
{

}

SUBEVENT(XTST_TRIG15)
{

}

EVENT
{
  trig1  = XTST_TRIG10(type=0x100,subtype=0x101);
  trig10 = XTST_TRIG10(type=0x100,subtype=0x10a);
  trig14 = XTST_TRIG14(type=0x100,subtype=0x10e);
  trig15 = XTST_TRIG15(type=0x100,subtype=0x10f);




}
