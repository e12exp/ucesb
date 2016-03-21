
#ifndef __PREFIX_UNIT_HH__
#define __PREFIX_UNIT_HH__

#include "file_line.hh"

#include <map>

// For each unit (for a calib param, i.e. where prefixes are just
// given, not marked) we keep track of each unit block, and it's
// exponent.  Exponents are negative for units that are to be divided.
/*
struct unit_exponent
{
public:
  const char *_str;
  int         _exponent;

public:
  bool operator<(const unit_exponent &rhs) const
  {
    int cmp = strcmp(_str,rhs._str);

    if (cmp < 0)
      return true;
    return false;
  }
};
*/
typedef std::map<const char*,int> unit_exponent_map;

// For units on values, we just keep track of the raw blocks (there
// can be no prefix markers)

class units_exponent
{
public:
  unit_exponent_map _blocks;

public:
  bool operator<(const units_exponent &rhs) const 
  {
    return _blocks < rhs._blocks;
  }
};

// For a unit with prefixes, we keep track of each unit block. And if
// it had a prefix, we remember that too (such that prefixes are not
// allowed) on those that did not have them.

class prefix_units_exponent
{
public:
  unit_exponent_map _blocks_simple;
  unit_exponent_map _blocks_prefix;
  int               _total_prefix;


public:
  bool operator<(const prefix_units_exponent &rhs) const 
  {
    if (_blocks_simple < rhs._blocks_simple)
      return true;
    if (_blocks_simple > rhs._blocks_simple)
      return false;
    if (_blocks_prefix < rhs._blocks_prefix)
      return true;
    if (_blocks_prefix > rhs._blocks_prefix)
      return false;
    
    return _total_prefix < rhs._total_prefix;
  }
};

void convert_units_exponent(const file_line &loc,const char *str,
			    units_exponent *units,
			    prefix_units_exponent *prefix_units);


const units_exponent *insert_units_exp(const file_line &loc,
				       const char *str);

const prefix_units_exponent *insert_prefix_units_exp(const file_line &loc,
						     const char *str);

const prefix_units_exponent *divide_units(const prefix_units_exponent *nom,
					  const prefix_units_exponent *denom);

bool match_units(const prefix_units_exponent *dest,
		 const units_exponent *src,
		 double &factor);

char *format_unit_str(const units_exponent *,char *str,size_t n);
char *format_unit_str(const prefix_units_exponent *,char *str,size_t n);

#endif//__PREFIX_UNIT_HH__
