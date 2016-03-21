
// This file is only used to force compilation of the user structures
// before they are parsed by psdc.

#include "struct_fcns.hh"

#ifdef USER_STRUCT_FILE
#include USER_STRUCT_FILE
#endif

#ifdef CALIB_STRUCT_FILE
#include CALIB_STRUCT_FILE
#endif
