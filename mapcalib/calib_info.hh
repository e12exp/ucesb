
#ifndef __CALIB_INFO_HH__
#define __CALIB_INFO_HH__

#include "map_info.hh"
#include "../common/prefix_unit.hh"

#include <vector>

struct double_unit
{
public:
  double                _value;
  const units_exponent *_unit;
};

typedef std::vector<double_unit> vect_double_unit;

#define CALIB_TYPE_SLOPE              1
#define CALIB_TYPE_OFFSET             2
#define CALIB_TYPE_SLOPE_OFFSET       3
#define CALIB_TYPE_OFFSET_SLOPE       4
#define CALIB_TYPE_CUT_BELOW_OR_EQUAL 5

struct calib_param 
  : public map_info
{
public:
  virtual ~calib_param() { }

public:
  calib_param(const file_line &loc,
	      const signal_id_info *src,const signal_id_info *dest,
	      int type,vect_double_unit *param)
    : map_info(loc,src,dest,NULL,NULL)
  {
    _type   = type;
    _param  = param;
  }

public:
  int               _type;
  vect_double_unit *_param;

};

struct user_calib_param 
  : public map_info // src not USED
{
public:
  virtual ~user_calib_param() { }

public:
  user_calib_param(const file_line &loc,
		   const signal_id_info *dest,
		   vect_double_unit *param)
    : map_info(loc,NULL,dest,NULL,NULL)
  {
    _param  = param;
  }

public:
  vect_double_unit *_param;

};

int calib_param_type(const char *type);

#endif//__CALIB_INFO_HH__
