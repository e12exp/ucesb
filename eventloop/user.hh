
#ifndef __USER_HH__
#define __USER_HH__

#include "watcher_event_info.hh"
#include "control_include.hh"
#include "multi_info.hh"

#ifdef INIT_USER_FUNCTION
void INIT_USER_FUNCTION();
#endif

#ifdef EXIT_USER_FUNCTION
void EXIT_USER_FUNCTION();
#endif

#ifdef UNPACK_EVENT_USER_FUNCTION
int UNPACK_EVENT_USER_FUNCTION(unpack_event *event);
#endif

#ifdef UNPACK_EVENT_END_USER_FUNCTION
void UNPACK_EVENT_END_USER_FUNCTION(unpack_event *event);
#endif

#ifdef RAW_EVENT_USER_FUNCTION
void RAW_EVENT_USER_FUNCTION(unpack_event *event,
			     raw_event    *raw_event
			     MAP_MEMBERS_PARAM);
#endif

#ifdef CAL_EVENT_USER_FUNCTION
void CAL_EVENT_USER_FUNCTION(unpack_event *event,
			     raw_event    *raw_event,
			     cal_event    *cal_event
#ifdef USER_STRUCT 
			     ,USER_STRUCT *user_event
#endif
			     MAP_MEMBERS_PARAM);
#endif

#ifdef WATCHER_EVENT_INFO_USER_FUNCTION
void WATCHER_EVENT_INFO_USER_FUNCTION(watcher_event_info *info,
				      unpack_event *event);
#endif

#ifdef CORRELATION_EVENT_INFO_USER_FUNCTION
bool CORRELATION_EVENT_INFO_USER_FUNCTION(unpack_event *event);
#endif

#ifdef MERGE_COMPARE_EVENTS_AFTER
bool MERGE_COMPARE_EVENTS_AFTER(unpack_event *x,unpack_event *y);
#endif

#ifdef COPY_OUTPUT_FILE_EVENT
#ifdef FILE_INPUT_EVENT
struct lmd_event_out;

bool COPY_OUTPUT_FILE_EVENT(lmd_event_out *event_out,
			    FILE_INPUT_EVENT *file_event,
			    unpack_event *event,
			    const select_event *select,
			    bool combine_event);
#endif
#endif

#ifdef OPEN_CLOSE_USER_FUNCTION
void OPEN_CLOSE_USER_FUNCTION(bool open, const char* filename,
			      uint64 file_size, uint32 n_events);
#endif

// The following function/interface WILL change, getopt_long is to be
// used instead.  You're welcome to fix ucesb yourself, to make it
// happen now, or expect that you'll have to adapt your program in the
// future... :-)
#ifdef HANDLE_COMMAND_LINE_OPTION
bool HANDLE_COMMAND_LINE_OPTION(const char *arg);
#endif

#ifdef USAGE_COMMAND_LINE_OPTIONS
void USAGE_COMMAND_LINE_OPTIONS();
#endif

#ifdef USAGE_COMMAND_LINE_OPTION
#error USAGE_COMMAND_LINE_OPTION changed to USAGE_COMMAND_LINE_OPTIONS
#endif

// The (user-defined) structure with calibration parameters.  Current
// idea: there is only one object with this data.  In case of
// multi-threading, it may be used SIMULTANEOUSLY by several user
// functions (unless precautions are taken).  When (if) varying
// calibration parameters are implemented, multi-processing will be
// handled such that it is updated inbetween handling of events from
// different regimes, i.e. update will have to block processing by the
// user function.

#ifdef CALIB_STRUCT
extern CALIB_STRUCT      _calib;
#endif

#endif//__USER_HH__
