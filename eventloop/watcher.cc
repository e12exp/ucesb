
#include "watcher.hh"

#define __LAND02_CODE__

#include "typedef.hh"
#include "watcher_window.hh"

#include "structures.hh"
#include "signal_id_map.hh"

#include "bitsone.hh"

#include "raw_data.hh"
#include "raw_data_watcher.hh"

#include "event_base.hh"

#include "../common/strndup.hh"

#include "user.hh"

#define STRUCT_MIRROR_TEMPLATE template<typename Twatcher_channel>
#define STRUCT_MIRROR_FCNS_DECL(name)           \
 public:                                        \
  bool enumerate_watchers(const signal_id &id,enumerate_watchers_info *info); \
  void watch_members(const class name &src,watcher_event_info *watch_info WATCH_MEMBERS_PARAM) const; \
  template<typename T,typename T_map> \
  void watch_members(const multi_chunks<T,T_map> &src,watcher_event_info *watch_info WATCH_MEMBERS_PARAM) const \
  { src.watch_members(*this,watch_info WATCH_MEMBERS_ARG); }
#define STRUCT_MIRROR_TYPE(type)         type##_watcher
#define STRUCT_MIRROR_TYPE_TEMPLATE      Twatcher_channel,
#define STRUCT_MIRROR_TYPE_TEMPLATE_FULL < Twatcher_channel >
#define STRUCT_MIRROR_NAME(name)         name
#define STRUCT_MIRROR_STRUCT(type)       STRUCT_MIRROR_TYPE(type)
#define STRUCT_MIRROR_BASE(type)         STRUCT_MIRROR_TYPE(type)<Twatcher_channel>
#define STRUCT_MIRROR_TEMPLATE_ARG(arg)  arg##_watcher<Twatcher_channel>,arg
#define STRUCT_MIRROR_TEMPLATE_ARG_N(arg,array)  arg##_watcher<Twatcher_channel> array,arg array
#define STRUCT_MIRROR_ITEM_CTRL_BASE(name) bool name##_active
#define STRUCT_MIRROR_ITEM_CTRL(name)      bool name##_active
#define STRUCT_MIRROR_ITEM_CTRL_ARRAY(name,non_last_index,last_index) bitsone<last_index> name##_active non_last_index
#define STRUCT_ONLY_LAST_UNION_MEMBER    1

#include "gen/struct_mirror.hh"
#include "gen/raw_struct_mirror.hh"
#include "gen/cal_struct_mirror.hh"
#ifdef USER_STRUCT
#include "gen/user_struct_mirror.hh"
#endif

#undef  STRUCT_MIRROR_TEMPLATE
#undef  STRUCT_MIRROR_FCNS_DECL
#undef  STRUCT_MIRROR_TYPE
#undef  STRUCT_MIRROR_TYPE_TEMPLATE
#undef  STRUCT_MIRROR_TYPE_TEMPLATE_FULL
#undef  STRUCT_MIRROR_NAME
#undef  STRUCT_MIRROR_STRUCT
#undef  STRUCT_MIRROR_BASE
#undef  STRUCT_MIRROR_TEMPLATE_ARG
#undef  STRUCT_MIRROR_TEMPLATE_ARG_N
#undef  STRUCT_MIRROR_ITEM_CTRL_BASE
#undef  STRUCT_MIRROR_ITEM_CTRL
#undef  STRUCT_MIRROR_ITEM_CTRL_ARRAY
#undef  STRUCT_ONLY_LAST_UNION_MEMBER

//#define APPEND_WATCHER(x) x##_watcher
//#define XXX(x) x

unpack_event_watcher<watcher_channel>         the_unpack_event_watcher;
raw_event_watcher<watcher_channel>            the_raw_event_watcher;
cal_event_watcher<watcher_channel>            the_cal_event_watcher;
#ifdef USER_STRUCT
//APPEND_WATCHER()   the_user_event_watcher;
#endif

unpack_event_watcher<watcher_present_channel> the_unpack_event_watcher_present;
raw_event_watcher<watcher_present_channel>    the_raw_event_watcher_present;
cal_event_watcher<watcher_present_channel>    the_cal_event_watcher_present;

//XXX(USER_STRUCT)
//USER_STRUCT


watcher_event_info _event_info;

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(DATA8 value,
						     watcher_event_info *watch_info)
{
  // All this to avoid type-punning warnings... it seems

  union value_ref_t
  {
    DATA8 _DATA8;
    uint8 _uint8;
  };

  value_ref_t value_ref;

  value_ref._DATA8 = value;

  _data.collect_raw((uint) value_ref._uint8,_event_info._type,watch_info);
}

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(DATA12 value,
						     watcher_event_info *watch_info)
{
  // All this to avoid type-punning warnings... it seems

  union value_ref_t
  {
    DATA12 _DATA12;
    uint16 _uint16;
  };

  value_ref_t value_ref;

  value_ref._DATA12 = value;

  _data.collect_raw((uint) value_ref._uint16,_event_info._type,watch_info);
}

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(DATA16 value,
						     watcher_event_info *watch_info)
{

}

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(DATA24 value,
						     watcher_event_info *watch_info)
{

}

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(DATA32 value,
						     watcher_event_info *watch_info)
{

}

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(DATA64 value,
						     watcher_event_info *watch_info)
{

}

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(uint8 value,
						     watcher_event_info *watch_info)
{

}

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(uint16 value,
						     watcher_event_info *watch_info)
{

}

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(uint32 value,
						     watcher_event_info *watch_info)
{

}

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(uint64 value,
						     watcher_event_info *watch_info)
{

}

template<typename T,typename Twatcher_channel>
void watcher_channel_wrap<T,Twatcher_channel>::event(float value,
						     watcher_event_info *watch_info)
{

}

/*
template<typename T>
void watcher_channel_wrap<T>::event(uint32 value)
{

}
*/
template<typename T,typename Twatcher_channel>
void watch_members(const T &src,const data_watcher<T,Twatcher_channel> &watch,watcher_event_info *watch_info WATCH_MEMBERS_PARAM)
{
  if (watch._watch)
    {
      watch._watch->event(src,watch_info);
    }
  /*
  if (watch._dest)
    {
      //WARNING("%d",watch._zzp_info._type);
      switch (watch._zzp_info->_type)
	{
	case ZZP_INFO_NONE: // no zero supress item
	case ZZP_INFO_FIXED_LIST: // part of fixed list
	  break; 
	case ZZP_INFO_CALL_ARRAY_INDEX:
	  (*watch._zzp_info->_array._call)(watch._zzp_info->_array._item,
					 watch._zzp_info->_array._index);
	  break;
	default:
	  ERROR("Internal error in data watching!");
	  break;
	}
      *watch._dest = src;
    }
  //char buf[256];
  //id.format(buf,sizeof(buf));
  //printf ("%s\n",buf);
  */
}

template<typename T,typename Twatcher_channel>
void data_watcher<T,Twatcher_channel>::watch_members(const T &src,watcher_event_info *watch_info WATCH_MEMBERS_PARAM) const
{
  ::watch_members(src,*this,watch_info WATCH_MEMBERS_ARG);
}

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n>
void raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>::watch_item(const T &src,const T_watcher &watch,watcher_event_info *watch_info 
									  WATCH_MEMBERS_PARAM)
{
  const Tsingle *p_src     = (Tsingle *) &src;
  const Tsingle_watcher *p = (Tsingle_watcher *) &watch;
  
  for (size_t i = sizeof(T)/sizeof(Tsingle); i; --i, ++p_src, ++p)
    {
      // ::watch_members(*p_src,*p WATCH_MEMBERS_ARG);
      p->watch_members(*p_src,watch_info WATCH_MEMBERS_ARG);
    }
}

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n>
void raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>::watch_members(const raw_array<Tsingle,T,n> &src,watcher_event_info *watch_info 
									     WATCH_MEMBERS_PARAM) const
{
  for (int i = 0; i < n; i++)
    {
      // ::watch_members(src._items[i],_items[i] WATCH_MEMBERS_ARG);
      watch_item(src._items[i],_items[i],watch_info WATCH_MEMBERS_ARG);
    }
}

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n>
void raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>::watch_members(const raw_array_zero_suppress<Tsingle,T,n> &src,watcher_event_info *watch_info 
									     WATCH_MEMBERS_PARAM) const
{
  bitsone_iterator iter;
  ssize_t i;
  
  while ((i = src._valid.next(iter)) >= 0)
    {
      // ::watch_members(src._items[i],_items[i] WATCH_MEMBERS_ARG);
      watch_item(src._items[i],_items[i],watch_info WATCH_MEMBERS_ARG);
    }
}

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n>
template<int max_entries>
void raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>::watch_members(const raw_array_multi_zero_suppress<Tsingle,T,n,max_entries> &src,watcher_event_info *watch_info 
									     WATCH_MEMBERS_PARAM) const
{
  bitsone_iterator iter;
  ssize_t i;
  
  while ((i = src._valid.next(iter)) >= 0)
    {
      // ::watch_members(src._items[i],_items[i] WATCH_MEMBERS_ARG);
      for (uint j = 0; j < src._num_entries[i]; j++)
	watch_item(src._items[i][j],_items[i],watch_info WATCH_MEMBERS_ARG);
    }
}

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n>
void raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>::watch_members(const raw_list_zero_suppress<Tsingle,T,n> &src,watcher_event_info *watch_info 
									     WATCH_MEMBERS_PARAM) const
{
  for (unsigned int i = 0; i < src._num_items; i++)
    {
      // ::watch_members(src._items[i]._item,_items[_items[i]._index] WATCH_MEMBERS_ARG);
      watch_item(src._items[i]._item,_items[src._items[i]._index],watch_info WATCH_MEMBERS_ARG);
    }
}

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n>
void raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>::watch_members(const raw_list_ii_zero_suppress<Tsingle,T,n> &src,watcher_event_info *watch_info 
									     WATCH_MEMBERS_PARAM) const
{
  for (unsigned int i = 0; i < src._num_items; i++)
    {
      // ::watch_members(src._items[i]._item,_items[_items[i]._index] WATCH_MEMBERS_ARG);
      watch_item(src._items[i],_items[i],watch_info WATCH_MEMBERS_ARG);
    }
}

#define FCNCALL_TEMPLATE template<typename Twatcher_channel>
#define FCNCALL_CLASS_NAME(name) name##_watcher<Twatcher_channel>
#define FCNCALL_NAME(name) \
  watch_members(const name &__src,watcher_event_info *watch_info WATCH_MEMBERS_PARAM) const
#define FCNCALL_CALL_BASE() watch_members(__src,watch_info WATCH_MEMBERS_ARG)
#define FCNCALL_CALL(member) watch_members(__src.member,watch_info WATCH_MEMBERS_ARG) // show_members(signal_id(id,#member))
#define FCNCALL_CALL_TYPE(type,member) ::watch_members<type>(__src.member,member,watch_info WATCH_MEMBERS_ARG) // ::show_members<type>(signal_id(id,#member))
#define FCNCALL_FOR(index,size) for (int index = 0; index < size; ++index)
#define FCNCALL_SUBINDEX(index) ;
#define FCNCALL_SUBNAME(name)   ;
//#define FCNCALL_SUBINDEX(index) const signal_id &shadow_id = id; signal_id id(shadow_id,index);
//#define FCNCALL_SUBNAME(name)   const signal_id &shadow_id = id; signal_id id(shadow_id,name);
#define FCNCALL_MULTI_MEMBER(name) name
#define FCNCALL_MULTI_ARG(name) multi_##name
#define FCNCALL_CALL_CTRL_WRAP(ctrl,call) { if (ctrl##_active || 1) { call; } }
#define FCNCALL_CALL_CTRL_WRAP_ARRAY(ctrl_name,ctrl_non_last_index,ctrl_last_index,call) (call)
#define STRUCT_ONLY_LAST_UNION_MEMBER 1

#include "gen/struct_fcncall.hh"
#include "gen/raw_struct_fcncall.hh"
#include "gen/cal_struct_fcncall.hh"
#ifdef USER_STRUCT
#include "gen/user_struct_fcncall.hh"
#endif

#undef  FCNCALL_TEMPLATE
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
#undef  FCNCALL_CALL_CTRL_WRAP
#undef  FCNCALL_CALL_CTRL_WRAP_ARRAY
#undef STRUCT_ONLY_LAST_UNION_MEMBER

void watcher_one_event(WATCH_MEMBERS_SINGLE_PARAM)
{
  memset(&_event_info,0,sizeof(_event_info));

#ifdef USE_LMD_INPUT
  //_event_info._time     = ;
  _event_info._event_no = _static_event._unpack.event_no;

  _event_info._info |= 
    //WATCHER_DISPLAY_INFO_TIME | 
    WATCHER_DISPLAY_INFO_EVENT_NO;
#endif


#if defined(WATCHER_EVENT_INFO_USER_FUNCTION)
  WATCHER_EVENT_INFO_USER_FUNCTION(&_event_info,
				   &_static_event._unpack);
#endif
  
  the_unpack_event_watcher.watch_members(_static_event._unpack,&_event_info
					 WATCH_MEMBERS_ARG);
  the_raw_event_watcher   .watch_members(_static_event._raw,&_event_info
					 WATCH_MEMBERS_ARG);
  //the_cal_event_watcher   .watch_members(_event._cal    WATCH_MEMBERS_ARG);
#ifdef USER_STRUCT
  //the_user_event_watcher  .watch_members(_event._user   WATCHER_MEMBERS_ARG);
#endif

  the_unpack_event_watcher_present.watch_members(_static_event._unpack,&_event_info
						 WATCH_MEMBERS_ARG);
  the_raw_event_watcher_present   .watch_members(_static_event._raw,&_event_info
						 WATCH_MEMBERS_ARG);
  //the_cal_event_watcher_present   .watch_members(_event._cal    WATCH_MEMBERS_ARG);
#ifdef USER_STRUCT
  //the_user_event_watcher_present  .watch_members(_event._user   WATCHER_MEMBERS_ARG);
#endif
}










template<typename T>
bool enumerate_watchers(data_watcher<T,watcher_channel> &watcher,const signal_id &id,enumerate_watchers_info *info)
{
  if (!info->_requests->is_channel_requested(id,false,0,false) ||
      watcher._watch)
    return watcher._watch != NULL; // already seen before?
  
  char name[256];
  {
    signal_id id_fmt(id);
    id_fmt.format_paw(name,sizeof(name));
  }
  
  // fprintf (stderr,"Enum: %s\n",name);

  // We'd like to add this item!!!
  
  watcher._watch = new watcher_channel_wrap<T,watcher_channel>;
  // watcher._watch = new __typeof__(*watcher._watch);

  watcher._watch->_data._name = name;
  if (info->_rescale_min != (uint) -1)
    watcher._watch->_data.set_rescale_min(info->_rescale_min);
  if (info->_rescale_max != (uint) -1)
    watcher._watch->_data.set_rescale_max(info->_rescale_max);

  info->_watcher->_display_channels.push_back(&(watcher._watch->_data));

  return true;
}

template<typename T>
bool enumerate_watchers(data_watcher<T,watcher_present_channel> &watcher,const signal_id &id,enumerate_watchers_info *info)
{
  if (!info->_requests->is_channel_requested(id,false,0,false) ||
      watcher._watch)
    return watcher._watch != NULL; // already seen before?

  // Find out parent (watcher_present_channels) item.
  // It is the item with its last index chopped down to an multiple of 32
  // In case some index dimension is zero suppressed, we use that instead
  // of the last.

  signal_id zzp_id(id);

  for (sig_part_vector::iterator part = zzp_id._parts.begin();
       part != zzp_id._parts.end(); ++part)
    if (part->_type & SIG_PART_INDEX)
      part->_id._index = 0;

  const signal_id_zzp_info *zzp_info =
    get_signal_id_zzp_info(zzp_id,info->_map_no);

  assert(zzp_info);

  signal_id parent_id(id);

  // std::map<signal_id,watcher_present_channels> _present_channels_map;

  int our_index = 0;
  sig_part *cut_part = NULL;

  if (zzp_info && zzp_info->_zzp_part != (size_t) -1)
    {
      assert(zzp_info->_zzp_part < parent_id._parts.size());
      cut_part = &parent_id._parts[zzp_info->_zzp_part];
    }
  else
    for (sig_part_vector::reverse_iterator part = parent_id._parts.rbegin();
	 part != parent_id._parts.rend(); ++part)
      {
	if (part->_type & SIG_PART_INDEX)
	  {
	    cut_part = &(*part);
	    break;
	  }
      }

  if (cut_part)
    {
      assert(cut_part->_type & SIG_PART_INDEX);
      our_index = cut_part->_id._index & (NUM_WATCH_PRESENT_CHANNELS-1);
      cut_part->_id._index &= ~(NUM_WATCH_PRESENT_CHANNELS-1);
    }

  watcher_present_channels *parent = NULL;

  id_present_channels_map::iterator parent_item =
    info->_id_present_channels.find(parent_id);

  if (parent_item == info->_id_present_channels.end())
    { 
      // We need to create the parent item
 
      char name[256];
      {
	signal_id id_fmt(parent_id);
	id_fmt.format_paw(name,sizeof(name));
      }

      parent = new watcher_present_channels;

      parent->_name = name;

      info->_watcher->_display_channels.push_back(parent);
      info->_watcher->_present_channels.push_back(parent);
      /*
      fprintf (stderr,"---> %s (%d)\n",name,
	       (int) info->_watcher->_display_channels.size());
      */
      info->_id_present_channels.insert(id_present_channels_map::value_type(parent_id,parent));

      // fprintf (stderr,"Enum: %s\n",name);
    }
  else
    parent = parent_item->second;
  
  // We'd like to add this item!!!
  
  watcher._watch = new watcher_channel_wrap<T,watcher_present_channel>;

  parent->_channels[our_index] = &watcher._watch->_data;
  watcher._watch->_data._container = parent;

  if (info->_rescale_min != (uint) -1)
    watcher._watch->_data.set_cut_min(info->_rescale_min);
  if (info->_rescale_max != (uint) -1)
    watcher._watch->_data.set_cut_max(info->_rescale_max);

  /*
  info->_watcher->_channels.push_back(&(watcher._watch->_data));
  */

  return true;
}

template<typename T,typename Twatcher_channel>
bool data_watcher<T,Twatcher_channel>::enumerate_watchers(const signal_id &id,enumerate_watchers_info *info)
{
  // ::watch_members(src,_items[i] WATCH_MEMBERS_ARG);
  return ::enumerate_watchers(*this,id,info);
}

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n>
bool raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>::enumerate_watchers(const signal_id &id,
										  enumerate_watchers_info *info)
{
  bool active = false;

  for (int i = 0; i < n; i++)
    active |= _items[i].enumerate_watchers(signal_id(id,i),info);

  return active;
}

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n,int n1>
bool raw_array_watcher_1<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n,n1>::enumerate_watchers(const signal_id &id,
										       enumerate_watchers_info *info)
{
  bool active = false;

  for (int i = 0; i < n; i++)
    for (int i1 = 0; i1 < n1; ++i1)
      active |= (raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>::_items[i])[i1].
	enumerate_watchers(signal_id(signal_id(id,i),i1),info);

  return active;
}

template<typename Twatcher_channel,typename Tsingle_watcher,typename Tsingle,typename T_watcher,typename T,int n,int n1,int n2>
bool raw_array_watcher_2<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n,n1,n2>::enumerate_watchers(const signal_id &id,
										       enumerate_watchers_info *info)
{
  bool active = false;

  for (int i = 0; i < n; i++)
    for (int i1 = 0; i1 < n1; ++i1)
      for (int i2 = 0; i2 < n2; ++i2)
	active |= (raw_array_watcher<Twatcher_channel,Tsingle_watcher,Tsingle,T_watcher,T,n>::_items[i])[i1][i2].
	  enumerate_watchers(signal_id(signal_id(signal_id(id,i),i1),i2),info);

  return active;
}




#define FCNCALL_TEMPLATE template<typename Twatcher_channel>
#define FCNCALL_CLASS_NAME(name) name##_watcher<Twatcher_channel>
#define FCNCALL_NAME(name) \
  enumerate_watchers(const signal_id &__id,enumerate_watchers_info *__info)
#define FCNCALL_CALL_BASE() enumerate_watchers(__id,__info)
#define FCNCALL_CALL(member) enumerate_watchers(__id,__info)
#define FCNCALL_CALL_TYPE(type,member) ::enumerate_watchers<type>(member,__id,__info)
#define FCNCALL_FOR(index,size) for (int index = 0; index < size; ++index)
#define FCNCALL_SUBINDEX(index) const signal_id &__shadow_id = __id; signal_id __id(__shadow_id,index); bool &__shadow_active = active; bool active = false;
#define FCNCALL_SUBINDEX_END(index) __shadow_active |= active;
#define FCNCALL_SUBNAME(name)   const signal_id &__shadow_id = __id; signal_id __id(__shadow_id,name);
//#define FCNCALL_SUBINDEX(index) const signal_id &shadow_id = id; signal_id id(shadow_id,index);
//#define FCNCALL_SUBNAME(name)   const signal_id &shadow_id = id; signal_id id(shadow_id,name);
#define FCNCALL_MULTI_MEMBER(name) name
#define FCNCALL_MULTI_ARG(name) name
#define FCNCALL_CALL_CTRL_WRAP(ctrl,call) ctrl##_active = (call)
#define FCNCALL_CALL_CTRL_WRAP_ARRAY(ctrl_name,ctrl_non_last_index,ctrl_last_index,call) active |= ((call) ? (ctrl_name##_active ctrl_non_last_index.set(ctrl_last_index)) , true : false)
#define FCNCALL_RET_TYPE bool
#define FCNCALL_INIT     bool active = false;
#define FCNCALL_RET      return active;
#define STRUCT_ONLY_LAST_UNION_MEMBER 1

#include "gen/struct_fcncall.hh"
#include "gen/raw_struct_fcncall.hh"
#include "gen/cal_struct_fcncall.hh"
#ifdef USER_STRUCT
#include "gen/user_struct_fcncall.hh"
#endif

#undef  FCNCALL_TEMPLATE
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
#undef  FCNCALL_RET_TYPE
#undef  FCNCALL_INIT
#undef  FCNCALL_RET
#undef  FCNCALL_CALL_CTRL_WRAP
#undef  FCNCALL_CALL_CTRL_WRAP_ARRAY
#undef STRUCT_ONLY_LAST_UNION_MEMBER















watcher_window _watcher;

void watcher_init(const char *command)
{
  enumerate_watchers_info info;

  info._rescale_min = (uint) -1;
  info._rescale_max = (uint) -1;

  info._watcher  = &_watcher;

  if (*command) 
    {
      for ( ; ; ) 
	{
	  detector_requests requests;
	  
	  const char *req_end;

	  bool show_present = false;
	  
	  for ( ; ; )
	    {
	      req_end = strpbrk(command,",:");
	      char *request;
	      if (req_end)
		request = strndup(command,(size_t) (req_end-command));
	      else
		request = strdup(command);
	      
	      char *post;
	      
#define MATCH_C_PREFIX(prefix,post) (strncmp(request,prefix,strlen(prefix)) == 0 && *(post = request + strlen(prefix)) != '\0')
#define MATCH_ARG(name) (strcmp(request,name) == 0)
	      
	      // printf ("Request: %s\n",request);
	      /*
		if (MATCH_ARG("UNPACK"))
		request_level |= NTUPLE_WRITER_UNPACK;
		else if (MATCH_ARG("RAW"))
		request_level |= NTUPLE_WRITER_RAW;
		else if (MATCH_ARG("CAL"))
		request_level |= NTUPLE_WRITER_CAL;
		else if (MATCH_ARG("USER"))
		request_level |= NTUPLE_WRITER_USER;
		else if (MATCH_ARG("RWN"))
		_paw_ntuple._ntuple_type = NTUPLE_TYPE_RWN;
		else if (MATCH_ARG("CWN"))
		_paw_ntuple._ntuple_type = NTUPLE_TYPE_CWN;
		else
	      */
	      if (MATCH_ARG("RANGE"))
		_watcher._show_range_stat = 1;
	      else if (MATCH_ARG("PRESENT"))
		show_present = true;
	      else if (MATCH_C_PREFIX("MIN=",post))
		info._rescale_min = (uint) atoi(post);
	      else if (MATCH_C_PREFIX("MAX=",post))
		info._rescale_max = (uint) atoi(post);
	      else if (MATCH_ARG("SPILL"))
		info._watcher->_display_at_mask = 
		  WATCHER_DISPLAY_SPILL_BOS | 
		  WATCHER_DISPLAY_SPILL_EOS;
	      else if (MATCH_ARG("BOS"))
		info._watcher->_display_at_mask =
		  WATCHER_DISPLAY_SPILL_BOS;
	      else if (MATCH_ARG("EOS"))
		info._watcher->_display_at_mask = 
		  WATCHER_DISPLAY_SPILL_EOS;
	      else if (MATCH_C_PREFIX("COUNT=",post))
		{
		  info._watcher->_display_at_mask = 
		    WATCHER_DISPLAY_COUNT;
		  info._watcher->_display_counts = (uint) atoi(post);
		}
	      else if (MATCH_C_PREFIX("TIMEOUT=",post))
		{
		  info._watcher->_display_at_mask = 
		    WATCHER_DISPLAY_TIMEOUT;
		  info._watcher->_display_timeout = (uint) atoi(post);
		}
	      else
		{
		  requests.add_detector_request(request,0);
		}
	      
	      free(request);
	      if (!req_end)
		break;
	      command = req_end+1;      
	      if (*req_end == ':')
		break;
	    }
	  
	  requests.prepare();

	  info._requests = &requests;

	  if (!show_present)
	    {	  
	      info._map_no = SID_MAP_UNPACK;
	      the_unpack_event_watcher.enumerate_watchers(signal_id(),&info);
	      info._map_no = SID_MAP_RAW;
	      the_raw_event_watcher   .enumerate_watchers(signal_id(),&info);
	      //info._map_no = SID_MAP_CAL;
	      //the_cal_event_watcher .enumerate_watchers(signal_id(),info);
#ifdef USER_STRUCT
	      //info._map_no = SID_MAP_USER;
	      //the_user_event_watcher.watch_members(_event._user WATCHER_MEMBERS_ARG);
#endif
	    }
	  else
	    {
	      
	      info._map_no = SID_MAP_UNPACK;
	      the_unpack_event_watcher_present.enumerate_watchers(signal_id(),
								  &info);
	      info._map_no = SID_MAP_RAW;
	      the_raw_event_watcher_present   .enumerate_watchers(signal_id(),
								  &info);
	      
	      //info._map_no = SID_MAP_CAL;
	      //the_cal_event_watcher .enumerate_present_watchers(signal_id(),
	      //                                                  &info);
#ifdef USER_STRUCT
	      //info._map_no = SID_MAP_USER;
	      //the_user_event_watcher.watch_present_members(_event._user WATCHER_MEMBERS_ARG);
#endif
	    }
	  
	  for (uint i = 0; i < requests._requests.size(); i++)
	    if (!requests._requests[i]._checked)
	      ERROR("Watcher request for item %s was not considered.  "
		    "Does that detector exist?",
		    requests._requests[i]._str);
	  
	  if (!req_end)
	    break;
	}
    }
  else
    {
      // No specific command line given, select all channels
      detector_requests requests;

      requests.prepare();
      
      info._requests = &requests;
      
      info._map_no = SID_MAP_UNPACK;
      the_unpack_event_watcher.enumerate_watchers(signal_id(),&info);
      info._map_no = SID_MAP_RAW;
      the_raw_event_watcher   .enumerate_watchers(signal_id(),&info);
    }
  
  _watcher.init();
}

void watcher_event()
{
  _watcher.event(_event_info);
}

