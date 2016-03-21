
#ifndef __SYNC_CHECK_INFO_HH__
#define __SYNC_CHECK_INFO_HH__

#include "bitsone.hh"

template<int bits>
struct sync_check_info_item
{
  int _index;
  int _counter;
};

template<int n,int bits>
class sync_check_info
{
public:
  sync_check_info()
  {
    clear();
  }

public:
  sync_check_info_item<bits> _items[n];
  int                        _num_items;

  bitsone<n>                 _seen;

public:
  void clear()
  {
    _num_items = 0; 
    _seen.clear();
  }

public:
  bool add(int index,int counter)
  {
    // since we have no overflow check, we instead also keep track of
    // seen modules.  They should only appear once!

    bool before = _seen.get_set(index);

    if (before)
      return before;

    sync_check_info_item<bits> &item = _items[_num_items++];
    
    item._index   = index;
    item._counter = counter;

    return 0;
  }


};

#endif//__SYNC_CHECK_INFO_HH__
