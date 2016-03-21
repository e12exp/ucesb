
#ifndef __DUMPER_HH__
#define __DUMPER_HH__

#include <stdio.h>

class dumper_dest
{
public:
  virtual ~dumper_dest() { }
  
public:
  virtual void put(char c) = 0;
  virtual void write(const char* p,size_t len) = 0;
};

class dumper_dest_file
  : public dumper_dest
{
public:
  dumper_dest_file(FILE *fid);
  virtual ~dumper_dest_file() { }
  
protected:
  FILE *_fid;
  
public:
  virtual void put(char c);
  virtual void write(const char* p,size_t len);
};

class dumper_dest_memory
  : public dumper_dest
{
public:
  dumper_dest_memory();
  virtual ~dumper_dest_memory();
  
protected:
  char *_start;
  char *_end;
  char *_cur;
  
public:
  char *get_string();
  void fwrite(FILE* fid);
  
public:
  virtual void put(char c) { write(&c,1); }
  virtual void write(const char* p,size_t len);
};



class dumper
{
public:
  dumper(dumper_dest *dest);
  dumper(dumper &src,int more = 0,bool comment = false);
  
public:
  size_t _indent;
  size_t _current;

  bool _comment;
  bool _comment_written;
  
  dumper_dest *_dest;
  
public:
  void indent();
  
  void nl();
  void text(const char *str,bool optional_newline = false);
  void text_fmt(const char *fmt,...)
    __attribute__ ((__format__ (__printf__, 2, 3)));
  void col0_text(const char *str);
};

void print_header(const char *marker,
                  const char *comment);
void print_footer(const char *marker);

#endif//__DUMPER_HH__
