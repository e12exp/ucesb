
#ifndef __THREAD_DEBUG_HH__
#define __THREAD_DEBUG_HH__

#include <sys/types.h>

#include <assert.h>

#define DEBUG_THREADING 0

#if DEBUG_THREADING

#ifdef  __ASSERT_FUNCTION
#define TDBG_FUNCTION __PRETTY_FUNCTION__ // __ASSERT_FUNCTION
#else
#define TDBG_FUNCTION __PRETTY_FUNCTION__ // __func__
#endif

#include <linux/unistd.h>
inline _syscall0(pid_t,gettid);

#define TDBG(...) { \
  fprintf(stderr,"<%5d> %s ",(int)gettid(),TDBG_FUNCTION); \
  fprintf(stderr,__VA_ARGS__); \
  fprintf(stderr,"\n"); \
}
#define TDBG_LINE(...) { \
  fprintf(stderr,__VA_ARGS__); \
  fprintf(stderr,"\n"); \
}

#else

#define TDBG(...) ((void) 0)
#define TDBG_LINE(...) ((void) 0)

#endif

#endif//__THREAD_DEBUG_HH__
