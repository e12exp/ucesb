
#ifndef __CONTROL_HH__
#define __CONTROL_HH__

#define INIT_USER_FUNCTION          esn_user_init
#define EXIT_USER_FUNCTION          esn_user_exit

#define UNPACK_EVENT_USER_FUNCTION  esn_user_function_unpack
#define RAW_EVENT_USER_FUNCTION     esn_user_function_raw
#define CAL_EVENT_USER_FUNCTION     esn_user_function

#define HANDLE_COMMAND_LINE_OPTION  esn_handle_command_line_option
#define USAGE_COMMAND_LINE_OPTIONS  esn_usage_command_line_options

// #define USING_MULTI_EVENTS 0

#define CALIB_STRUCT esn_calib_struct

#endif//__CONTROL_HH__
