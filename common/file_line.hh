
#ifndef __FILE_LINE_H__
#define __FILE_LINE_H__

#include <stdio.h>

struct lineno_map
{
  int         _internal;

  int         _line;
  const char* _file;

  lineno_map *_prev;
  lineno_map *_next;
};

extern lineno_map *_last_lineno_map;
extern lineno_map *_first_lineno_map;

void print_lineno(FILE* fid,int internal);

class file_line
{
public:
  file_line(int internal)
  {
    _internal = internal;
  }

  file_line()
  {
    _internal = 0;
  }

public:
  void print_lineno(FILE* fid) const
  {
    ::print_lineno(fid,_internal);
  }

public:
  int _internal;
};

void generate_locations();

#endif//__FILE_LINE_H__
