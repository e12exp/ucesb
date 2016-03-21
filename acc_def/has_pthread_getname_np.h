#ifndef __HAS_PTHREAD_GETNAME_NP_H__
#define __HAS_PTHREAD_GETNAME_NP_H__

#ifndef ACC_DEF_RUN
# include "gen/acc_auto_def/has_pthread_getname_np.h"
#endif

#if ACC_DEF_HAS_PTHREAD_GETNAME_NP_pthread_h
# define HAS_PTHREAD_GETNAME_NP 1
# ifndef _GNU_SOURCE
# define _GNU_SOURCE /* for pthread_setname_np */
# endif
#endif
#if ACC_DEF_HAS_PTHREAD_GETNAME_NP_notavail
# define HAS_PTHREAD_GETNAME_NP 0
#endif

#ifdef ACC_DEF_RUN
# include <pthread.h>

void acc_test_func(pthread_t thread)
{
#if HAS_PTHREAD_GETNAME_NP
  char fullname[16];
  pthread_getname_np(thread, fullname, sizeof (fullname));
  pthread_setname_np(thread, fullname);
#else
  (void) thread;
#endif
}
#endif

#endif/*__HAS_PTHREAD_GETNAME_NP_H__*/
