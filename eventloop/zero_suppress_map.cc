
#include "structures.hh"

#include "zero_suppress_map.hh"

#include "event_base.hh"

#include <map>

struct zzp_ptr
{
public:
  zzp_ptr(void *ptr,const void *const *ptr_offset = NULL)
  {
    _ptr        = ptr;
    _ptr_offset = ptr_offset;
  }

public:
  void              *_ptr;
  const void *const *_ptr_offset;

public:
  bool operator<(const zzp_ptr &rhs) const
  {
    if (_ptr_offset < rhs._ptr_offset)
      return true;
    if (_ptr_offset > rhs._ptr_offset)
      return false;
    return (_ptr < rhs._ptr);    
  };
};

typedef std::map<zzp_ptr,const zero_suppress_info *> map_ptr_zero_suppress_info;

map_ptr_zero_suppress_info _map_ptr_zero_suppress_info;

const zero_suppress_info *get_ptr_zero_suppress_info(void *us,const void *const *ptr_offset,
						     bool allow_missing)
{
  map_ptr_zero_suppress_info::iterator iter;

  iter = _map_ptr_zero_suppress_info.find(zzp_ptr(us,ptr_offset));

  if (iter == _map_ptr_zero_suppress_info.end())
    {
      if (!allow_missing)
	ERROR("Could not find zero_suppress_info for pointer %p.",us);
      return NULL;
    }

  return iter->second;
}

struct used_zero_suppress_info
{
public:
  used_zero_suppress_info(const zero_suppress_info *info)
  {
    _info = info;
    _used = false;
  }

  ~used_zero_suppress_info()
  {
    if (!_used)
      delete _info;
  }
  
public:
  const zero_suppress_info *_info;
  bool                      _used;
};

void insert_zero_suppress_info_ptrs(void *us,used_zero_suppress_info &used_info)
{
  // So we have a pointer (us),
  // whenever that is used, one should use the info!

  zzp_ptr p(us,used_info._info->_ptr_offset);

  std::pair<map_ptr_zero_suppress_info::iterator,bool> known_ptr = 
    _map_ptr_zero_suppress_info.insert(map_ptr_zero_suppress_info::value_type(p,used_info._info));
  
  if (!known_ptr.second)
    ERROR("Ptr %p->%p already known by zero-suppression map.",p._ptr_offset,p._ptr);
  // If you got error here, did you call the setting up of the
  // zero-suppression maps twice??

  used_info._used |= true; // we have used the info object
}

//template<typename T>
//void zero_suppress_info_ptrs(T* us,used_zero_suppress_info &used_info)
void zero_suppress_info_ptrs(void* us,used_zero_suppress_info &used_info)
{
  ::insert_zero_suppress_info_ptrs(us,used_info);
}

template<typename Tsingle,typename T,int n>
void raw_array<Tsingle,T,n>::zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
{
  if ((used_info._info->_type & ZZP_INFO_MASK) != ZZP_INFO_NONE)
    ERROR("Two levels of zero suppression not supported!");
#if 0
  for (int i = 0; i < n; ++i)
    {
      zero_suppress_info *info = new zero_suppress_info(used_info._info);
      zzp_on_insert_index(i,*info);
      used_zero_suppress_info sub_used_info(info);

      _items[i].zero_suppress_info_ptrs(sub_used_info);
    }
#endif
  for (int i = 0; i < n; ++i)
    _items[i].zero_suppress_info_ptrs(used_info);
}

template<typename Tsingle,typename T,int n>
void raw_array_zero_suppress<Tsingle,T,n>::zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
{
  if ((used_info._info->_type & ZZP_INFO_MASK) != ZZP_INFO_NONE)
    ERROR("Two levels of zero suppression not supported!");
  
  for (int i = 0; i < n; ++i)
    {
      zero_suppress_info *info = new zero_suppress_info(used_info._info);
      zzp_on_insert_index(i,*info);
      used_zero_suppress_info sub_used_info(info);

      _items[i].zero_suppress_info_ptrs(sub_used_info);
    }
}

template<typename Tsingle,typename T,int n,int n1>
void raw_array_zero_suppress_1<Tsingle,T,n,n1>::zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
{
  if ((used_info._info->_type & ZZP_INFO_MASK) != ZZP_INFO_NONE)
    ERROR("Two levels of zero suppression not supported!");
  
  for (int i = 0; i < n; ++i)
    {
      zero_suppress_info *info = new zero_suppress_info(used_info._info);
      raw_array_zero_suppress<Tsingle,T,n>::zzp_on_insert_index(i,*info);
      used_zero_suppress_info sub_used_info(info);

      for (int i1 = 0; i1 < n1; ++i1)
	(raw_array_zero_suppress<Tsingle,T,n>::_items[i])[i1].zero_suppress_info_ptrs(sub_used_info);
    }
}

template<typename Tsingle,typename T,int n,int n1,int n2>
void raw_array_zero_suppress_2<Tsingle,T,n,n1,n2>::zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
{
  if ((used_info._info->_type & ZZP_INFO_MASK) != ZZP_INFO_NONE)
    ERROR("Two levels of zero suppression not supported!");
  
  for (int i = 0; i < n; ++i)
    {
      zero_suppress_info *info = new zero_suppress_info(used_info._info);
      raw_array_zero_suppress<Tsingle,T,n>::zzp_on_insert_index(i,*info);
      used_zero_suppress_info sub_used_info(info);
      
      for (int i1 = 0; i1 < n1; ++i1)
	for (int i2 = 0; i2 < n2; ++i2)
	  (raw_array_zero_suppress<Tsingle,T,n>::_items[i])[i1][i2].zero_suppress_info_ptrs(sub_used_info);
    }
}

template<typename Tsingle,typename T,int n,int max_entries>
void raw_array_multi_zero_suppress<Tsingle,T,n,max_entries>::zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
{
  if ((used_info._info->_type & ZZP_INFO_MASK) != ZZP_INFO_NONE)
    ERROR("Two levels of zero suppression not supported!");
  
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < max_entries; j++)
      {
	zero_suppress_info *info = new zero_suppress_info(used_info._info);
	zzp_on_insert_index(i,j,*info);
	used_zero_suppress_info sub_used_info(info);
	
	_items[i][j].zero_suppress_info_ptrs(sub_used_info);
      }
}

template<typename Tsingle,typename T,int n>
void raw_list_zero_suppress<Tsingle,T,n>::zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
{
  if ((used_info._info->_type & ZZP_INFO_MASK) != ZZP_INFO_NONE)
    ERROR("Two levels of zero suppression not supported!");
  
  for (int i = 0; i < n; ++i)
    {
      zero_suppress_info *info = new zero_suppress_info(used_info._info);
      zzp_on_insert_index(i,*info);
      used_zero_suppress_info sub_used_info(info);
      
      _items[i].zero_suppress_info_ptrs(sub_used_info);
    }
}

template<typename Tsingle,typename T,int n,int n1>
void raw_list_zero_suppress_1<Tsingle,T,n,n1>::zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
{
  if ((used_info._info->_type & ZZP_INFO_MASK) != ZZP_INFO_NONE)
    ERROR("Two levels of zero suppression not supported!");
  
  for (int i = 0; i < n; ++i)
    {
      zero_suppress_info *info = new zero_suppress_info(used_info._info);
      raw_list_zero_suppress<Tsingle,T,n>::zzp_on_insert_index(i,*info);
      used_zero_suppress_info sub_used_info(info);
      
      for (int i1 = 0; i1 < n1; ++i1)
	(raw_list_zero_suppress<Tsingle,T,n>::_items[i])._item[i1].zero_suppress_info_ptrs(sub_used_info);
    }
}

template<typename Tsingle,typename T,int n,int n1,int n2>
void raw_list_zero_suppress_2<Tsingle,T,n,n1,n2>::zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
{
  if ((used_info._info->_type & ZZP_INFO_MASK) != ZZP_INFO_NONE)
    ERROR("Two levels of zero suppression not supported!");
  
  for (int i = 0; i < n; ++i)
    {
      zero_suppress_info *info = new zero_suppress_info(used_info._info);
      raw_list_zero_suppress<Tsingle,T,n>::zzp_on_insert_index(i,*info);
      used_zero_suppress_info sub_used_info(info);
      
      for (int i1 = 0; i1 < n1; ++i1)
	for (int i2 = 0; i2 < n2; ++i2)
	  (raw_list_zero_suppress<Tsingle,T,n>::_items[i])._item[i1][i2].zero_suppress_info_ptrs(sub_used_info);
    }
}

template<typename Tsingle,typename T,int n>
void raw_list_ii_zero_suppress<Tsingle,T,n>::zero_suppress_info_ptrs(used_zero_suppress_info &used_info)
{
  int type_mask_away = ~0;
  int new_type = 0;
  
  switch (used_info._info->_type & ZZP_INFO_MASK)
    {
    case ZZP_INFO_CALL_ARRAY_INDEX:
      type_mask_away = ZZP_INFO_CALL_ARRAY_INDEX;
      new_type       = ZZP_INFO_CALL_ARRAY_LIST_II_INDEX;
      break;
    case ZZP_INFO_CALL_LIST_INDEX:
      type_mask_away = ZZP_INFO_CALL_ARRAY_INDEX;
      new_type       = ZZP_INFO_CALL_LIST_LIST_II_INDEX;
      break;
    case ZZP_INFO_NONE:
      new_type       = ZZP_INFO_CALL_LIST_II_INDEX;
      break;
    default:
      ERROR("Two levels of zero suppression not supported!");
    }
  
  for (int i = 0; i < n; ++i)
    {
      zero_suppress_info *info = new zero_suppress_info(used_info._info,type_mask_away);
      zzp_on_insert_index(i,*info,new_type);
      used_zero_suppress_info sub_used_info(info);
      
      _items[i].zero_suppress_info_ptrs(sub_used_info);
    }
}


// void zero_suppress_info(const zero_suppress_info &info); 

#define FCNCALL_CLASS_NAME(name) name
#define FCNCALL_NAME(name) \
  zero_suppress_info_ptrs(used_zero_suppress_info &__used_info)
#define FCNCALL_CALL_BASE() zero_suppress_info_ptrs(__used_info)
#define FCNCALL_CALL(member) zero_suppress_info_ptrs(__used_info)
#define FCNCALL_CALL_TYPE(type,member) ::zero_suppress_info_ptrs/*<type>*/(&member,__used_info)
#define FCNCALL_FOR(index,size) for (int index = 0; index < size; ++index)
#define FCNCALL_SUBINDEX(index) ;
#define FCNCALL_SUBNAME(name)   ;
//#define FCNCALL_SUBINDEX(index) const signal_id &shadow_id = id; signal_id id(shadow_id,index);
//#define FCNCALL_SUBNAME(name)   const signal_id &shadow_id = id; signal_id id(shadow_id,name);
#define FCNCALL_MULTI_MEMBER(name) \
  if (__used_info._info->_type != ZZP_INFO_NONE)                   \
    ERROR("Cannot have zero-suppress info before multi-member!");  \
  const used_zero_suppress_info &__shadow_used_info = __used_info; \
  UNUSED(__shadow_used_info);                                      \
  zero_suppress_info *__info = new zero_suppress_info;             \
  __info->_type = ZZP_INFO_PTR_OFFSET;                             \
  __info->_ptr_offset = name.get_ptr_addr();                       \
  used_zero_suppress_info __used_info(__info);                     \
  name()
#define FCNCALL_MULTI_ARG(name) name
#define STRUCT_ONLY_LAST_UNION_MEMBER 1

// We are not sane for multi members anyhow!
// But we are also used by the ntuple dumper to figure out hwo the zero-suppression works...
#include "gen/struct_fcncall.hh" 
// For these we are sane
#include "gen/raw_struct_fcncall.hh"
#include "gen/cal_struct_fcncall.hh"
#ifdef USER_STRUCT
#include "gen/user_struct_fcncall.hh"
#endif

#undef  FCNCALL_CLASS_NAME
#undef  FCNCALL_NAME
#undef  FCNCALL_CALL_BASE
#undef  FCNCALL_CALL
#undef  FCNCALL_CALL_TYPE
#undef  FCNCALL_FOR
#undef  FCNCALL_SUBINDEX
#undef  FCNCALL_SUBNAME
#undef  FCNCALL_MULTI_MEMBER
#undef  FCNCALL_MULTI_ARG
#undef STRUCT_ONLY_LAST_UNION_MEMBER


void setup_zero_suppress_info_ptrs()
{
  used_zero_suppress_info used_info(new zero_suppress_info);

#ifndef USE_MERGING
  _static_event._unpack.zero_suppress_info_ptrs(used_info);
  _static_event._raw.zero_suppress_info_ptrs(used_info);
  _static_event._cal.zero_suppress_info_ptrs(used_info);
#ifdef USER_STRUCT
  _static_event._user.zero_suppress_info_ptrs(used_info);
#endif
#endif//!USE_MERGING
}

#define SNPRINTF_NAME(name) {			\
    int n = snprintf(dest,length,name);		\
    length -= (size_t) n; dest += n;		\
  } 

void get_enum_type_name(int type,char *dest,size_t length)
{
  if (type & ENUM_HAS_INT_LIMIT)  SNPRINTF_NAME("has_limit ");
  if (type & ENUM_IS_LIST_LIMIT)  SNPRINTF_NAME("list_limit ");
  if (type & ENUM_IS_LIST_LIMIT2) SNPRINTF_NAME("list_limit2 ");
  if (type & ENUM_IS_ARRAY_MASK)  SNPRINTF_NAME("array_mask ");
  if (type & ENUM_IS_LIST_INDEX)  SNPRINTF_NAME("list_index ");

  switch (type & ENUM_TYPE_MASK)
    {
    case ENUM_TYPE_FLOAT : SNPRINTF_NAME("float");  break;
    case ENUM_TYPE_DOUBLE: SNPRINTF_NAME("double"); break;
    case ENUM_TYPE_USHORT: SNPRINTF_NAME("ushort"); break;
    case ENUM_TYPE_UCHAR : SNPRINTF_NAME("uchar");  break;
    case ENUM_TYPE_INT   : SNPRINTF_NAME("int");    break;
    case ENUM_TYPE_UINT  : SNPRINTF_NAME("uint");   break;
    case ENUM_TYPE_ULINT : SNPRINTF_NAME("ulint");  break;
    case ENUM_TYPE_UINT64: SNPRINTF_NAME("uint64"); break;
    case ENUM_TYPE_DATA8 : SNPRINTF_NAME("DATA8");  break;
    case ENUM_TYPE_DATA12: SNPRINTF_NAME("DATA12"); break;
    case ENUM_TYPE_DATA16: SNPRINTF_NAME("DATA16"); break;
    case ENUM_TYPE_DATA24: SNPRINTF_NAME("DATA24"); break;
    case ENUM_TYPE_DATA32: SNPRINTF_NAME("DATA32"); break;
    case ENUM_TYPE_DATA64: SNPRINTF_NAME("DATA64"); break;
    default: SNPRINTF_NAME("*unknown*"); break;
    }
  assert(length); // or fix calling location!
}
