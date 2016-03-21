
#ifndef __MULTI_INFO_HH__
#define __MULTI_INFO_HH__

#include "control_include.hh"

#if !USING_MULTI_EVENTS
#define MAP_MEMBERS_SINGLE_PARAM
#define MAP_MEMBERS_PARAM
#define MAP_MEMBERS_ARG

#define WATCH_MEMBERS_SINGLE_PARAM
#define WATCH_MEMBERS_PARAM
#define WATCH_MEMBERS_SINGLE_ARG
#define WATCH_MEMBERS_ARG
#endif//!USING_MULTI_EVENTS

#if USING_MULTI_EVENTS
struct map_members_info
{
  int _multi_event_no;
  int _event_type;  
};

#define MAP_MEMBERS_SINGLE_PARAM    const map_members_info &info
#define MAP_MEMBERS_PARAM         , const map_members_info &info
#define MAP_MEMBERS_ARG           , info

#define WATCH_MEMBERS_SINGLE_PARAM    const map_members_info &info
#define WATCH_MEMBERS_PARAM         , const map_members_info &info
#define WATCH_MEMBERS_SINGLE_ARG      info
#define WATCH_MEMBERS_ARG           , info

#endif//USING_MULTI_EVENTS

#endif//__MULTI_INFO_HH__
