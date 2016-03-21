
#ifndef __STR_SET_H__
#define __STR_SET_H__

#include <string.h>
#include <unistd.h>

/* Normal C strings and not std::string is used because of the
 * overhead of using std::string.  And it also seems to have a few
 * undesirable features.  See string_discussion.html in the SGI stl manual.
 *
 * Instead, I operate on new/delete C strings.
 */

struct compare_str_less
{
  bool operator()(const char *s1,const char *s2) const
  {
    return strcmp(s1,s2) < 0;
  }
};

const char *find_str_identifiers(const char *str);

const char *find_str_strings(const char *str,size_t len = (size_t) -1);

const char *find_concat_str_strings(const char *s1,const char *s2,const char *s3);



#endif// __STR_SET_H__

