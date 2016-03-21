
#ifndef __MILLE_OUTPUT__
#define __MILLE_OUTPUT__

// This file constitutes an alternative to using the Mille.h/Mille.cc
// source for writing data file for the Millepede system.  (see
// http://www.desy.de/~blobel/mptalks.html)
//
// The reasons would be:
//
// - ability to handle multi-threaded generation of the local
// parameters (only the actual file writing need to be serialized)
//
// - different call sequence (also local parameters handled with a
// list of labels) (if this is an improvement is up to the user)
// (these local labels should start at 1, and be consequtively used
// within each record)
//
// - ability to include this code as part of UCESB (GPLed) (works, as
// the GPL ends at program boundaries)
//
// - in no way is this an attempt att circumventing the licence of
// Millepede.

#include <stdio.h>



struct mille_lbl_der
{
  int   lbl;
  float der;
};

class mille_record
{
public:
  mille_record();
  ~mille_record();

public:
  size_t _array_used;
  size_t _array_allocated;
  float *_array_float;
  int   *_array_int;

  int    _num_eqn;

public:
  void reallocate(size_t need_more);
  
public:
  void reset();

  void add_der_lbl(int num,float *der,int *lbl);
  void add_eqn(int num_local,float *der_local,int *lbl_local,
	       int num_global,float *der_global,int *lbl_global,
	       float measured,float sigma);

  void add_der_lbl(int num,mille_lbl_der *lbl_der);
  void add_eqn(int num_local,mille_lbl_der *lbl_der_local,
	       int num_global,mille_lbl_der *lbl_der_global,
	       float measured,float sigma);
};

class mille_file
{
public:
  mille_file();
  ~mille_file();

public:
  FILE *_fid; // use buffered file handling (each record is small)

public:
  void open(const char *filename);
  void close();

  void write(const mille_record &record);

};

#endif//__MILLE_OUTPUT__
