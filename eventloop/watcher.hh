
#ifndef __WATCHER_HH__
#define __WATCHER_HH__

#ifdef USE_CURSES

#include "multi_info.hh"

void watcher_init(const char *command);
void watcher_event();

void watcher_one_event(WATCH_MEMBERS_SINGLE_PARAM);

#endif

#endif//__WATCHER_HH__
