
#ifndef __FORKED_CHILD_HH__
#define __FORKED_CHILD_HH__

#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>

// This class encapsulates the work to fork of a child process
// (successfully).
//
// Then you may get a file handle for the process' stdout.
//
// After you have read what you expect, you can wait for the child to
// see that it worked as expected (did not exit with error code).  In
// fact, one must wait for child processes, or they are left as
// zombies.

// The child is provided with a broken stdin, and stderr is inherited
// from us.

class forked_child
{
public:
  forked_child();
  ~forked_child();

public:
  pid_t _child;

  int _fd_in;  // from child
  int _fd_out; // to child

  char *_argv0; // for error messages

public:
  void fork(const char *file,const char *const argv[],
	    int *fd_in,int *fd_out,
	    int fd_dest = -1,int fd_src = -1,
	    int fd_keep = -1,char *fork_pipes = NULL);
  void wait(bool terminate_child,
	    int *exit_status = NULL);
  void close_fds();
  
};

size_t full_write(int fd,const void *buf,size_t count);

///

extern const char *main_argv0;

char *argv0_replace(const char *filename);

#endif//__FORKED_CHILD_HH__

