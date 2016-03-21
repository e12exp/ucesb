#include <curses.h>
#include <term.h>

int main()
{
  int errret, ret;

  ret = setupterm(NULL, 1, &errret);

  tparm(tigetstr("setaf"),1);

  return 0;
}
