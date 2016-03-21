
#ifndef __ENCODE_HH__
#define __ENCODE_HH__

#include "file_line.hh"

#include "variable.hh"
#include "param_arg.hh"

#include <vector>

#define ES_APPEND_LIST 0x01

struct encode_spec
{
public:
  encode_spec(const file_line &loc,
	      const var_name *name,
	      const argument_list *args,
	      int flags)
  {
    _loc = loc;

    _name = name;
    _args = args;

    _flags = flags;
  }

public:
  virtual ~encode_spec() { }

public:
  virtual void dump(dumper &d) const;

public:
  const var_name *_name;
  const argument_list *_args;
  int _flags;

public:
  file_line _loc;
};

typedef std::vector<encode_spec*> encode_spec_list;

#endif//__ENCODE_HH__
