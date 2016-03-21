
#include "str_set.hh"

#include <set>

/* We keep a set of all strings that are in use.  This because they are
 * duplicated at so many places.  We do not have to constantly allocate
 * them.  We only have to store pointers.  Which in turn leads to less
 * memory use, better cache hit rate, etc...
 *
 * The set as such need to store pointers to the strings, as otherwise
 * the set may reallocate itself moving the strings.
 */

typedef std::set<const char*,
                 compare_str_less > set_strings;

/* If you want to compare two strings for equality, as long as they
 * are in the same set, a simple pointer comparison is enough, as
 * those are unique.  This however perhaps is an invitation to
 * trouble...  So beware. */

/* We keep two sets, one for IDENTIFIERS and one for STRINGS.  (see lexer.lex) */

set_strings _str_set_identifiers;
set_strings _str_set_strings;

// For identifiers, we expect them to not be unique, i.e.
// To already be in the set

const char *find_str_identifiers(const char *str)
{
  set_strings::iterator i = _str_set_identifiers.lower_bound(str);

  if (i == _str_set_identifiers.end() ||
      strcmp(*i,str) != 0)
    {
      // We need to be inserted.

      size_t len = strlen (str);

      char *s = new char[len+1];
      
      memcpy(s,str,len);
      s[len] = '\0';

      _str_set_identifiers.insert(i,s);

      return s;
    }
  else
    {
      // We were already there

      return *i;
    }
}

// For the strings, we expect them to be unique...

const char *find_str_strings(const char *str,size_t len)
{
  if (len == (size_t) -1)
    len = strlen(str);

  char *s = new char[len+1];

  memcpy(s,str,(size_t) len);
  s[len] = '\0';

  std::pair<set_strings::iterator,bool> result = _str_set_strings.insert(s);

  if (result.second == false)
    {
      // We were already there

      delete[] s;
    }
  else
    {
      // We were newly inserted.

      
    }

  return *result.first;
}

const char *find_concat_str_strings(const char *s1,const char *s2,const char *s3)
{
  size_t l1 = strlen(s1);
  size_t l2 = strlen(s2);
  size_t l3 = strlen(s3);

  size_t len = l1+l2+l3;

  char *s = new char[len+1];

  memcpy(s,s1,l1);
  memcpy(s+l1,s2,l2);
  memcpy(s+l1+l2,s3,l3);
  s[len] = '\0';

  std::pair<set_strings::iterator,bool> result = _str_set_strings.insert(s);

  if (result.second == false)
    delete[] s;
  
  return *result.first;
}
