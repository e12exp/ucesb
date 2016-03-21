
#ifndef __ESN_CALIB_STRUCT_HH__
#define __ESN_CALIB_STRUCT_HH__

// Careful.  These structures are also parsed by psdc in order to
// prepare for enumeration for ntuple writing.  I.e. only a small
// subset of C struct features are allowed!

struct vdc_calib
{
  float t0 UNIT("#ns");

  float t_off[240] UNIT("#ns");

  float t_slope UNIT("#ns/ch");
  
  CALIB_STRUCT_FCNS_DECL;
};

struct esn_calib_struct
{
  vdc_calib vdc[2][2];

  CALIB_STRUCT_FCNS_DECL;
};

#endif//__ESN_CALIB_STRUCT_HH__
