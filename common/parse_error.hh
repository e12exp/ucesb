
#ifndef __ERROR_HH__
#define __ERROR_HH__

#include <stdlib.h>



#if defined __GNUC__ && __GNUC__ < 3 // 2.95 do not do iso99 variadic macros
#define ERROR(__VA_ARGS__...) do { \
  fprintf(stderr,"%s:%d: ",__FILE__,__LINE__);  \
  fprintf(stderr,__VA_ARGS__);  \
  fputc('\n',stderr);           \
  exit(1);                      \
} while (0)
#else
#define ERROR(...) do {         \
  fprintf(stderr,"%s:%d: ",__FILE__,__LINE__);  \
  fprintf(stderr,__VA_ARGS__);  \
  fputc('\n',stderr);           \
  exit(1);                      \
} while (0)
#endif

#if defined __GNUC__ && __GNUC__ < 3 // 2.95 do not do iso99 variadic macros
#define WARNING(__VA_ARGS__...) do { \
  fprintf(stderr,"%s:%d: ",__FILE__,__LINE__);  \
  fprintf(stderr,__VA_ARGS__);  \
  fputc('\n',stderr);           \
} while (0)
#else
#define WARNING(...) do {       \
  fprintf(stderr,"%s:%d: ",__FILE__,__LINE__);  \
  fprintf(stderr,__VA_ARGS__);  \
  fputc('\n',stderr);           \
} while (0)
#endif

#if defined __GNUC__ && __GNUC__ < 3 // 2.95 do not do iso99 variadic macros
#define ERROR_LOC(loc,__VA_ARGS__...) do { \
  (loc).print_lineno(stderr);    \
  fputc(' ',stderr);             \
  fprintf(stderr,__VA_ARGS__);   \
  fputc('\n',stderr);            \
  exit(1);                       \
} while (0)
#else
#define ERROR_LOC(loc,...) do { \
  (loc).print_lineno(stderr);    \
  fputc(' ',stderr);             \
  fprintf(stderr,__VA_ARGS__);   \
  fputc('\n',stderr);            \
  exit(1);                       \
} while (0)
#endif

#if defined __GNUC__ && __GNUC__ < 3 // 2.95 do not do iso99 variadic macros
#define ERROR_LOC_PREV(loc,loc2,__VA_ARGS__...) do { \
  (loc).print_lineno(stderr);    \
  fputc(' ',stderr);             \
  fprintf(stderr,__VA_ARGS__);   \
  fputc('\n',stderr);            \
  (loc2).print_lineno(stderr);   \
  fprintf(stderr," Previous here.\n"); \
  exit(1);                       \
} while (0)
#else
#define ERROR_LOC_PREV(loc,loc2,...) do { \
  (loc).print_lineno(stderr);    \
  fputc(' ',stderr);             \
  fprintf(stderr,__VA_ARGS__);   \
  fputc('\n',stderr);            \
  (loc2).print_lineno(stderr);   \
  fprintf(stderr," Previous here.\n"); \
  exit(1);                       \
} while (0)
#endif

#if defined __GNUC__ && __GNUC__ < 3 // 2.95 do not do iso99 variadic macros
#define WARNING_LOC(loc,__VA_ARGS__...) do { \
  (loc).print_lineno(stderr);    \
  fputc(' ',stderr);             \
  fprintf(stderr,__VA_ARGS__);   \
  fputc('\n',stderr);            \
} while (0)
#else
#define WARNING_LOC(loc,...) do { \
  (loc).print_lineno(stderr);    \
  fputc(' ',stderr);             \
  fprintf(stderr,__VA_ARGS__);   \
  fputc('\n',stderr);            \
} while (0)
#endif




#endif//__ERROR_HH__


