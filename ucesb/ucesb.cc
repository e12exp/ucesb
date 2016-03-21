
#include <stdio.h>
#include <stdlib.h>

#include "definitions.hh"
#include "parse_error.hh"

#include "signal_errors.hh"

extern int lexer_read_fd;

bool parse_definitions();

void usage()
{
  printf ("ucesb\n"
	  "usage ucesb\n");
  printf ("    ");
}

int main(int /*argc*/,char *argv[])
{
  lexer_read_fd = 0; // read from stdin

  setup_segfault_coredump(argv[0]);

  if (!parse_definitions())
    ERROR("%s: Aborting!\n",argv[0]);

  map_definitions();

  dump_definitions();

  check_consistency();

  generate_unpack_code();

  generate_signals();

  generate_locations();

  return 0;
}

