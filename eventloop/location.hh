
#ifndef __LOCATION_HH__
#define __LOCATION_HH__

#include <stdio.h>

struct location
{
  int         _internal;

  int         _line;
  const char* _file;
};

class formatted_error;

void ucesb_loc_print_lineno(FILE *fid,int internal);
void ucesb_loc_print_lineno(formatted_error &fe,int internal);

#endif//__LOCATION_HH__
