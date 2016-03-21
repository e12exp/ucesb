
#ifndef __CROS3_USER_STRUCT_HH__
#define __CROS3_USER_STRUCT_HH__

// Careful.  These structures are also parsed by psdc in order to
// prepare for enumeration for ntuple writing.  I.e. only a small
// subset of C struct features are allowed!

#include "pdc_user_struct.hh"

struct cros3_user_struct
{
  PDC_hits ccb[2];

  USER_STRUCT_FCNS_DECL;
};

#endif//__CROS3_USER_STRUCT_HH__

