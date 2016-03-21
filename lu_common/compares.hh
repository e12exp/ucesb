
#ifndef __COMPARES_HH__
#define __COMPARES_HH__

template<typename T>
struct compare_ptr_less
{
  bool operator()(const T *s1,const T *s2) const
  {
    return *s1 < *s2;
  }
};

template<class T>
struct compare_ptrs_strcmp
{
  bool operator()(const T& s1,const T& s2) const
  {
    return strcmp(s1,s2) < 0;
  }
};

typedef compare_ptrs_strcmp<const char *> compare_str_less;
/*
struct compare_str_less
{
  bool operator()(const char *s1,const char *s2) const
  {
    return strcmp(s1,s2) < 0;
  }
};
*/

#endif/*__COMPARES_HH__*/
