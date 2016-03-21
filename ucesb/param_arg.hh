
#ifndef __PARAM_ARG_HH__
#define __PARAM_ARG_HH__

#include "variable.hh"
#include "dumper.hh"
#include "file_line.hh"

#include <vector>
#include <map>

class struct_member;

class param
{
public:
  param(const char *name,const const32 *def = NULL)
  {
    _name = name;
    _def  = def;

    _member = NULL;
  }

public:
  const char    *_name;
  const const32 *_def;

  struct_member *_member;

public:
  void dump(dumper &d);
};

typedef std::vector<param*> param_list;

class argument
{
public:
  argument(const char *name,const variable *var)
  {
    _name    = name;
    _var     = var;
  }

public:
  const char     *_name;
  const variable *_var;

public:
  void dump(dumper &d);
};

typedef std::vector<argument*> argument_list;

typedef std::map<const char *,const variable *> arguments_map;

typedef std::map<const char *,const const32 *> params_def_map;

struct arguments
{
public:
  arguments(const argument_list* src_arg,
	    const param_list *src_param,
	    const arguments *prev);

public:
  arguments_map  _arg_map;
  params_def_map _param_def_map;

public:
  const arguments *_prev;
};

void dump_param_args(const file_line &loc,
		     const param_list *params,
		     const argument_list *args,
		     dumper &d,
		     bool dump_member_args);

void dump_match_args(const file_line &loc,
		     const param_list *params,
		     const argument_list *args,
		     dumper &d);

#endif//__PARAM_ARGUMENT_HH__
