// -*- C++ -*-

SUBEVENT(LAND_FASTBUS_ERROR)
{
  UINT32 error_code
    {
      0:  timeout_waiting_smi;
      1:  illegal_block_transfer;
      2:  fifa_address_overflow;
      3:  fifa_register_error;
      16: init_error;
    }
  UINT32 error_num;
}


SUBEVENT(LAND_FASTBUS)
{
  UINT32 zero NOENCODE { 0_31: 0; }

  select several
    {
#include "fastbus_c1.hh"
    }
}
