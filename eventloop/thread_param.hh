
#ifndef __THREAD_PARAM_HH__
#define __THREAD_PARAM_HH__

#ifdef USE_PTHREAD

#define PTHREAD_PARAM(type_param) , type_param
#define PTHREAD_ARG(arg)          , arg

#else

#define PTHREAD_PARAM(type_param)
#define PTHREAD_ARG(arg)           

#endif

#endif//__THREAD_PARAM_HH__
