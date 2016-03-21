#ifndef __LOGFILE_HH__
#define __LOGFILE_HH__

class logfile 
{
public:
  logfile(const char* file);
  ~logfile();

protected:
  int _fd;

public:
  size_t append(const char* msg);
};

#endif
