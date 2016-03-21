
#ifndef __CONTROL_HH__
#define __CONTROL_HH__

#define INIT_USER_FUNCTION          fa192mar09_user_init
#define EXIT_USER_FUNCTION          fa192mar09_user_exit

#define UNPACK_EVENT_USER_FUNCTION  fa192mar09_user_function_multi
#define RAW_EVENT_USER_FUNCTION     fa192mar09_user_function_raw
#define CAL_EVENT_USER_FUNCTION     fa192mar09_user_function

#define HANDLE_COMMAND_LINE_OPTION  fa192mar09_handle_command_line_option
#define USAGE_COMMAND_LINE_OPTIONS  fa192mar09_usage_command_line_options

#define USER_EXTERNAL_UNPACK_STRUCT_FILE "fa192mar09_external.hh"

#define WATCHER_EVENT_INFO_USER_FUNCTION fa192mar09_watcher_event_info

#define WATCHER_EVENT_INFO_INCLUDE "fa192mar09_watch_info.hh"

#define USING_MULTI_EVENTS 1

#endif//__CONTROL_HH__
