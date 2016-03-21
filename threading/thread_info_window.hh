
#ifndef __THREAD_INFO_WINDOW_HH__
#define __THREAD_INFO_WINDOW_HH__

#include "thread_info.hh"

#include <curses.h>

class thread_info_window
{
public:
  thread_info_window();

protected:
  WINDOW *mw;

  union
  {
    WINDOW *wall[5];
    struct
    {
      WINDOW *winput;
#ifdef USE_THREADING
      WINDOW *wtasks;
      WINDOW *wthreads;
#endif
      WINDOW *wtotals;
      WINDOW *werrors;
    };
  };

  thread_info *_ti;

public:
  void init(thread_info *ti);
  void display();

  void add_error(const char *text,
		 int severity);


};

#endif//__THREAD_INFO_WINDOW_HH__
