
#include "file_line.hh"
#include "dumper.hh"

lineno_map *_last_lineno_map  = NULL;
lineno_map *_first_lineno_map = NULL;;

void print_lineno(FILE* fid,int internal)
{
  lineno_map *map = _last_lineno_map;

  if (!map)
    {
      fprintf (fid,"BEFORE_START:0:");
      return;
    }

  while (internal < map->_internal)
    {
      map = map->_prev;
      if (!map)
	{
	  fprintf (fid,"LINE_NO_TOO_EARLY(%d):0:",internal);
	  return;
	}
    }

  fprintf(fid,"%s:%d:",map->_file,map->_line + internal - map->_internal);
}


#ifdef UCESB_SRC
void generate_locations()
{
  print_header("LOCATIONS",
	       "File and line locations from the parsed specification files.");

  printf ("// It's left to the compiler to only store one copy of each\n"
	  "// unique string.\n"
	  "\n");

  printf ("location spec_locations[] =\n"
	  "{ \n");
  for (lineno_map *map = _first_lineno_map; map; map = map->_next)
    {
      printf ("  { %d, %d, \"%s\" },\n",
	      map->_internal,
	      map->_line,
	      map->_file);
    }
  printf ("};\n");


  print_footer("LOCATIONS");
}
#endif
