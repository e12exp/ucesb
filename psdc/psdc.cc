
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_error.hh"

#include "c_struct.hh"

#include "corr_struct.hh"

extern int lexer_read_fd;

bool parse_definitions();

void usage()
{
  printf ("psdc\n"
	  "usage psdc\n");
  printf ("    ");
}

extern bool _ignore_missing_corr_struct;

int main(int argc,char *argv[])
{
  lexer_read_fd = 0; // read from stdin

  for (int i = 1; i < argc; i++)
    if (strcmp(argv[i],"--ignore-missing-corr-struct") == 0)
      _ignore_missing_corr_struct = true;

  if (!parse_definitions())
    ERROR("%s: Aborting!\n",argv[0]);

  map_definitions();

  dump_definitions();

  mirror_struct();
  mirror_struct_decl();
  fcn_call_per_member();

  corr_struct();

  return 0;
}

