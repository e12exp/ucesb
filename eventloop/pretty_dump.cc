
#include "pretty_dump.hh"

#include <stdio.h>

void pretty_dump(const signal_id &id,const char *buf,
		 pretty_dump_info &pdi)
{
  // There are two ways.  Either we format the new @id completely, and
  // compare to the previous id we had.  Then we can blank out any
  // parts which are the same.  Or we compare the parts of the signals
  // themselves, and create only the new items.

  // Comparing the strings, we'll be forced to compare more than
  // char-by-char, as we'd like a difference to start at an entire
  // item, and not in the middle of a name, due to that character
  // changing...

  // It would also be nice to every 25 lines or so, rewrite the entire
  // string, such that one need not scroll around like mad...

  char name[245];

  id.format(name,sizeof(name));

  printf ("%s: %s\n",name,buf);

}

