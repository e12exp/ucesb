
#include "calib_info.hh"

int calib_param_type(const char *type)
{
  if (strcmp(type,"SLOPE") == 0)
    return CALIB_TYPE_SLOPE;
  if (strcmp(type,"OFFSET") == 0)
    return CALIB_TYPE_OFFSET;
  if (strcmp(type,"SLOPE_OFFSET") == 0)
    return CALIB_TYPE_SLOPE_OFFSET;
  if (strcmp(type,"OFFSET_SLOPE") == 0)
    return CALIB_TYPE_OFFSET_SLOPE;
  if (strcmp(type,"CUT_BELOW_OE") == 0)
    return CALIB_TYPE_CUT_BELOW_OR_EQUAL;

  return 0;
}
