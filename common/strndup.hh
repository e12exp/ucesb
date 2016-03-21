
#ifndef __STRNDUP_HH__
#define __STRNDUP_HH__

#include <stdlib.h>
#include <string.h>  
// try to get it the normal way, hmm, won't actually
// work as outlined below (is not a macro, but anyhow)

#ifndef strndup
// Actually, since strndup is not a macro, we'll always be done...
inline char *strndup(const char *src,size_t length)
{
  // We wast memory in case the string actually is shorter...
  char *dest = (char *) malloc(length+1);
  strncpy(dest,src,length);
  dest[length]='\0'; // since strncpy would not handle this
  return dest;
}
#endif

#endif//__STRNDUP_HH__

