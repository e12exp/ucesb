#ifndef __SET_THREAD_NAME_HH__
#define __SET_THREAD_NAME_HH__

#include <pthread.h>

void set_thread_name(pthread_t thread,
		     const char *name,
		     size_t maxlen);

#endif/*__SET_THREAD_NAME_HH__*/
