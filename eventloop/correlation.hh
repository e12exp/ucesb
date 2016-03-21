
#ifndef __CORRELATION_HH__
#define __CORRELATION_HH__

#include "multi_info.hh"

#include "config.hh"

void correlation_init(const config_command_vect &commands);
void correlation_exit();
void correlation_event(WATCH_MEMBERS_SINGLE_PARAM);

void correlation_one_event(WATCH_MEMBERS_SINGLE_PARAM);

#endif//__CORRELATION_HH__
