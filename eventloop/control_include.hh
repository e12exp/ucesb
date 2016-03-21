
#ifndef __CONTROL_INCLUDE_HH__
#define __CONTROL_INCLUDE_HH__

#ifdef CONTROL_INCLUDE
#include "control.hh"
#endif

#ifndef USING_MULTI_EVENTS
#define USING_MULTI_EVENTS 0
#endif

#if USING_MULTI_EVENTS
#ifndef UNPACK_EVENT_USER_FUNCTION
#error UNPACK_EVENT_USER_FUNCTION must be defined when USING_MULTI_EVENTS
#endif
#endif

/* Warn if there are some troubles with old bad names. */

#ifdef INITUSERFUNCTION
#error INITUSERFUNCTION deprecated, use INIT_USER_FUNCTION
#endif

#ifdef EXITUSERFUNCTION
#error EXITUSERFUNCTION deprecated, use EXIT_USER_FUNCTION
#endif

#ifdef RAWEVENTUSERFUNCTION
#error RAWEVENTUSERFUNCTION deprecated, use RAW_EVENT_USER_FUNCTION
#endif

#ifdef CALEVENTUSERFUNCTION
#error CALEVENTUSERFUNCTION deprecated, use CAL_EVENT_USER_FUNCTION
#endif

#endif//__CONTROL_INCLUDE_HH__
