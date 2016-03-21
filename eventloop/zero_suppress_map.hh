#ifndef __ZERO_SUPPRES_MAP_HH__
#define __ZERO_SUPPRES_MAP_HH__

#include "zero_suppress.hh"

#define ZZP_INFO_NONE                           0x00
#define ZZP_INFO_FIXED_LIST                     0x01
#define ZZP_INFO_CALL_ARRAY_INDEX               0x02
#define ZZP_INFO_CALL_ARRAY_MULTI_INDEX         0x03
#define ZZP_INFO_CALL_LIST_INDEX                0x04
#define ZZP_INFO_CALL_LIST_II_INDEX             0x05
#define ZZP_INFO_CALL_ARRAY_LIST_II_INDEX       0x06 // ARRAY and LIST_II
#define ZZP_INFO_CALL_LIST_LIST_II_INDEX        0x07 // LIST  and LIST_II
#define ZZP_INFO_MASK                           0x07

#define ZZP_INFO_PTR_OFFSET                     0x08 // bitmask

typedef void(*call_on_array_insert_fcn)(void *,uint);
typedef size_t(*call_on_list_insert_fcn)(void *,uint);
typedef size_t(*call_on_list_ii_insert_fcn)(void *);

struct zero_suppress_info
{
public:
  zero_suppress_info(const zero_suppress_info* src,int type_mask_away = ~0)
  {
    _type       = src->_type;
    _ptr_offset = src->_ptr_offset;

    if ((_type & ZZP_INFO_MASK) != ZZP_INFO_NONE)
      {
	// If you reach this point, then you have not protected the
	// code enough!  This is a consequence of us not being able to
	// handle several levels or zero suppress info
	ERROR("Internal error in zero_suppress_info constructor.");
      }
  }

public:
  zero_suppress_info()
  {
    _type       = ZZP_INFO_NONE;
    _ptr_offset = NULL;
  }

public:
  int   _type;

  union
  {
    struct
    {
      union
      {
	call_on_array_insert_fcn  _call;
	call_on_list_insert_fcn   _call_multi;
      };
      void                     *_item;
      uint                      _index;

      unsigned long            *_limit_mask;
    } _array;
    struct
    {
      call_on_list_insert_fcn    _call;

      void                    *_item;
      uint                     _index;

      size_t                   _dest_offset; // to be subtracted from _dest before use/insert

      uint32                  *_limit;
    } _list;
    struct
    {
      uint                     _index;
    } _fixed_list;
  };
  struct
  {
    call_on_list_ii_insert_fcn _call_ii;

    void                    *_item;
    uint                     _index;

    size_t                   _dest_offset; // to be subtracted from _dest before use/insert
    
    uint32                  *_limit;
  } _list_ii;

  const void *const *_ptr_offset;
};

//template<typename T>
//void zero_suppress_info_ptrs(T* us,used_zero_suppress_info &used_info);
void zero_suppress_info_ptrs(void* us,used_zero_suppress_info &used_info);

void insert_zero_suppress_info_ptrs(void *us,used_zero_suppress_info &used_info);

template<typename Tsingle,typename T,int n>
void raw_array<Tsingle,T,n>::zzp_on_insert_index(/*int loc,*/uint32 i,zero_suppress_info &info)
{
  if (i >= n)
    // ERROR_U_LOC(loc,"Attempt to index outside list (%d>=%d)",i,n);
    ERROR("Attempt to index outside list (%d>=%d)",i,n);

  assert (!(info._type & ZZP_INFO_MASK));
  info._type |= ZZP_INFO_FIXED_LIST;
  info._fixed_list._index = i;
}

template<typename T>
void call_on_insert_array_index(void *container,uint i)
{
  ((T *)container)->on_insert_index(i);
}

template<typename Tsingle,typename T,int n>
void raw_array_zero_suppress<Tsingle,T,n>::zzp_on_insert_index(/*int loc,*/uint32 i,zero_suppress_info &info)
{
  if (i >= n)
    // ERROR_U_LOC(loc,"Attempt to index outside list (%d>=%d)",i,n);
    ERROR("Attempt to index outside list (%d>=%d)",i,n);
  /*
    _valid.get_set_ptr(i,
    &zero_suppress_info.addr,
    &zero_suppress_info.mask);
    
    zero_suppress_info._item = _items[i];
    
    item_t &item = _items[i];
  */

  assert (!(info._type & ZZP_INFO_MASK));
  info._type |= ZZP_INFO_CALL_ARRAY_INDEX;
  info._array._call = call_on_insert_array_index< raw_array_zero_suppress<Tsingle,T,n> >;
  info._array._item = this;
  info._array._index = i;

  info._array._limit_mask = _valid._bits;
}

template<typename T>
size_t call_on_insert_list_index(void *container,uint i)
{
  size_t offset = ((T *)container)->on_insert_index(i);
  return offset;
}

template<typename Tsingle,typename T,int n>
void raw_list_zero_suppress<Tsingle,T,n>::zzp_on_insert_index(/*int loc,*/uint32 i,zero_suppress_info &info)
{
  if (i >= n)
    // ERROR_U_LOC(loc,"Attempt to index outside list (%d>=%d)",i,n);
    ERROR("Attempt to index outside list (%d>=%d)",i,n);
  
  assert (!(info._type & ZZP_INFO_MASK));
  info._type |= ZZP_INFO_CALL_LIST_INDEX;
  info._list._call = call_on_insert_list_index< raw_list_zero_suppress<Tsingle,T,n> >;
  info._list._item = this;
  info._list._index = i;
  info._list._limit = &_num_items;

  void *ptr   = (void *) &_items[i];
  void *first = (void *) &_items[0];

  info._list._dest_offset = (size_t) (((char*) ptr) - ((char*) first));
}

template<typename Tsingle,typename T,int n,int max_entries>
void raw_array_multi_zero_suppress<Tsingle,T,n,max_entries>::zzp_on_insert_index(/*int loc,*/uint32 i,uint32 j,zero_suppress_info &info)
{
  if (i >= n)
    // ERROR_U_LOC(loc,"Attempt to index outside list (%d>=%d)",i,n);
    ERROR("Attempt to index outside list (%d>=%d)",i,n);
  if (j >= max_entries)
    // ERROR_U_LOC(loc,"Attempt to index outside list (%d>=%d)",i,n);
    ERROR("Attempt to index outside list (%d>=%d)",j,max_entries);
  /*
    _valid.get_set_ptr(i,
    &zero_suppress_info.addr,
    &zero_suppress_info.mask);
    
    zero_suppress_info._item = _items[i];
    
    item_t &item = _items[i];
  */

  assert (!(info._type & ZZP_INFO_MASK));
  info._type |= ZZP_INFO_CALL_ARRAY_MULTI_INDEX;
  info._array._call_multi = call_on_insert_list_index< raw_array_multi_zero_suppress<Tsingle,T,n,max_entries> >;
  info._array._item = this;
  info._array._index = i;

  info._array._limit_mask = _valid._bits;

  info._list_ii._index = j;
  info._list_ii._limit = &_num_entries[i];

  void *ptr   = (void *) &_items[i][j];
  void *first = (void *) &_items[i][0];

  info._list_ii._dest_offset = (size_t) (((char*) ptr) - ((char*) first));
}

template<typename T>
size_t call_on_insert_list_ii_item(void *container)
{
  size_t offset = ((T *)container)->on_append_item();
  return offset;
}

template<typename Tsingle,typename T,int n>
void raw_list_ii_zero_suppress<Tsingle,T,n>::zzp_on_insert_index(/*int loc,*/uint32 i,zero_suppress_info &info,int new_type)
{
  if (i >= n)
    // ERROR_U_LOC(loc,"Attempt to index outside list (%d>=%d)",i,n);
    ERROR("Attempt to index outside list (%d>=%d)",i,n);
  
  assert (!(info._type & ZZP_INFO_MASK));
  info._type |= new_type;
  info._list_ii._call_ii = call_on_insert_list_ii_item< raw_list_ii_zero_suppress<Tsingle,T,n> >;
  info._list_ii._item = this;
  info._list_ii._index = i;
  info._list_ii._limit = &_num_items;

  void *ptr   = (void *) &_items[i];
  void *first = (void *) &_items[0];

  info._list_ii._dest_offset = (size_t) (((char*) ptr) - ((char*) first));
}

const zero_suppress_info *get_ptr_zero_suppress_info(void *us,const void *const *ptr_offset,bool allow_missing);

void setup_zero_suppress_info_ptrs();

#endif//__ZERO_SUPPRES_MAP_HH__
