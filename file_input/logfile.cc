
#include "error.hh"
#include "logfile.hh"
#include "forked_child.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

logfile::logfile(const char* file)
{
  _fd = open(file, O_CREAT | O_WRONLY | O_APPEND,
	     S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (_fd == -1)
    ERROR("Failed to open logfile '%s'", file);
  INFO(0,"Opened logfile '%s'.", file);
}

logfile::~logfile()
{
  if (_fd != -1)
    {
      if (close(_fd) != 0)
	perror("close");
    }
}

size_t logfile::append(const char* msg)
{
  if (_fd == -1)
    return (size_t) -1;
  return full_write(_fd, msg, strlen(msg));
}

