
#include "location.hh"
#include "error.hh"

#include "gen/locations.hh"

void ucesb_loc_print_lineno(formatted_error &fe,int internal)
#define FID_PRINTF(...)  fe.printf(__VA_ARGS__)
#if 0
void ucesb_loc_print_lineno(FILE *fid,int internal)
#define FID_PRINTF(...)  fprintf(fid,__VA_ARGS__)
#endif
{
  location *start = spec_locations;
  location *map   = spec_locations + countof(spec_locations) - 1;

  if (countof(spec_locations) == 0)
    FID_PRINTF("BEFORE_START:0:");

  while (map >= start)
    {
      // fprintf(fid,"%s:%d %d,%d\n",map->_file,map->_line,map->_internal,internal);

      if (internal >= map->_internal)
	{
	  FID_PRINTF("%s:%d:",map->_file,map->_line + internal - map->_internal);
	  return;
	}
      map--;
    }
  FID_PRINTF("LINE_NO_TOO_EARLY(%d):0:",internal);
}
