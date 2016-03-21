// -*- C++ -*-

EVENT
{
  ignore_unknown_subevent;
}



// Before 'ignore_unknown_subevent' was implemented, all
// this was needed...  (along with the now removed files
// empty_external.hh:  #include "external_data.hh"
// control.hh:         #define USER_EXTERNAL_UNPACK_STRUCT_FILE \
//                     "empty_external.hh"

/*

external EXTERNAL_DATA_SKIP();

SUBEVENT(ANY_SUBEVENT)
{
  // We'll skip the entire subevent...
  
  external skip = EXTERNAL_DATA_SKIP();
}

EVENT
{
  // handle any subevent
  // allow it several times

  revisit any = ANY_SUBEVENT();
}

*/
