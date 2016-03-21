#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for pthread_setname_np
#endif

#include "has_pthread_getname_np.h"

#include "set_thread_name.hh"

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define THREADNAMELEN 16

void set_thread_name(pthread_t thread,
		     const char *name,
		     size_t maxlen)
{
#if HAS_PTHREAD_GETNAME_NP
  char fullname[THREADNAMELEN];
  size_t len;
  char *p;

  if (pthread_getname_np(thread, fullname, sizeof (fullname)) != 0)
    {
      fullname[0] = 0;
    }

  p = strchr(fullname, '\n');
  if (p)
    *p = 0;

  len = strlen(fullname);

  if (len > THREADNAMELEN - 1 - maxlen - 1)
    len = THREADNAMELEN - 1 - maxlen - 1;

  fullname[len] = 0;

  strcat(fullname, "/");
  strncat(fullname, name, maxlen);
  
  pthread_setname_np(thread, fullname);
#else
  (void) thread;
  (void) name;
  (void) maxlen;
#endif
}
