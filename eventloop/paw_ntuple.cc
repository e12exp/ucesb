
#include "paw_ntuple.hh"

#include "event_base.hh"

#include "hbook.hh"
#include "staged_ntuple.hh"
#include "staging_ntuple.hh"
#include "writing_ntuple.hh"
#include "ntuple_item.hh"

#include "lmd_output.hh"

#include "detector_requests.hh"

#include "../common/strndup.hh"

#include <map>

#ifdef __APPLE__
#include <libgen.h>
#endif

struct ntuple_limit_item
{
  ntuple_item *_item;
  size_t       _omit_index;
};

typedef std::map<const void *,ntuple_limit_item> map_ntuple_limits;

struct enumerate_ntuple_info
{
  vect_ntuple_items *_listing;
  detector_requests *_requests;
  bool               _cwn;
  map_ntuple_limits  _limits;
  int                _level;
  bool               _detailed_only;
  const char        *_block;
  const char        *_block_prefix;
};

void enumerate_member_paw_ntuple(const signal_id &id,
				 const enumerate_info &info,
				 void *extra)
{
  enumerate_ntuple_info *ntuple = (enumerate_ntuple_info *) extra;

  if (!(ntuple->_requests->
	is_channel_requested(id,info._type & (ENUM_IS_LIST_LIMIT |
					      ENUM_IS_ARRAY_MASK),
			     ntuple->_level, ntuple->_detailed_only)))
    return;

  const zero_suppress_info *zzp_info = NULL;

  const void *const* ptr_offset =
    info._type & ENUM_HAS_PTR_OFFSET ? info._ptr_offset : NULL;

  if (ptr_offset)
    {
      WARNING("Cowardly refusing paw_ntuple item with ptr_offset.");
      // The ntuple stager/writer needs to be fixed to also handle
      // variables that may have an offset to their pointers...
      // For the time being, refuse those...
      return;
    }

  if (!(info._type & (ENUM_IS_LIST_LIMIT |
		      ENUM_IS_ARRAY_MASK)))
    zzp_info = get_ptr_zero_suppress_info((void*) info._addr,
					  ptr_offset,
					  false);
  
  // printf ("paw_ntuple item: %s (%p)  (zzp:%d)\n",
  //         name,info._addr,zzp_info ? zzp_info->_type : -1);

  const void  *limit_addr = NULL;
  uint         limit_index = (uint) -1;
  ntuple_item *limit_item = NULL;

  const uint32 *limit2_addr = NULL;
  uint          limit2_index = (uint) -1;
  ntuple_item  *limit_item2 = NULL;
  
  if (ntuple->_cwn && zzp_info)
    {
      // Item is part of an array, that has an controlling item (count
      // or bitmask)

      switch (zzp_info->_type)
	{
	case ZZP_INFO_NONE:
	  break; // actually not indexed (or no controlling item)
	case ZZP_INFO_FIXED_LIST:
	  ERROR("Unimplemented.");
	  limit_index = zzp_info->_fixed_list._index;
	  break;
	case ZZP_INFO_CALL_ARRAY_INDEX:
	  limit_addr = zzp_info->_array._limit_mask;
	  limit_index = zzp_info->_array._index;
	  break;
	case ZZP_INFO_CALL_LIST_INDEX:
	  // Find the name of the controlling variable!
	  // item->_index_var = ;
	  limit_addr = zzp_info->_list._limit;
	  limit_index = zzp_info->_list._index;
	  break;
	case ZZP_INFO_CALL_LIST_II_INDEX:
	  limit_addr = zzp_info->_list_ii._limit;
	  limit_index = zzp_info->_list_ii._index;
	  break;
        case ZZP_INFO_CALL_ARRAY_MULTI_INDEX:
	  limit_addr = zzp_info->_array._limit_mask;
	  limit_index = zzp_info->_array._index;
	  goto limit2_list_ii_index;
	case ZZP_INFO_CALL_ARRAY_LIST_II_INDEX:
	  limit_addr = zzp_info->_array._limit_mask;
	  limit_index = zzp_info->_array._index;
	  goto limit2_list_ii_index;
	case ZZP_INFO_CALL_LIST_LIST_II_INDEX:
	  limit_addr = zzp_info->_list._limit;
	  limit_index = zzp_info->_list._index;
	  goto limit2_list_ii_index;
	limit2_list_ii_index:
	  limit2_addr = zzp_info->_list_ii._limit;
	  limit2_index = zzp_info->_list_ii._index;
	  if (zzp_info->_type != ZZP_INFO_CALL_ARRAY_MULTI_INDEX)
	    ERROR("Unimplemented - no support for "
		  "zero suppression + multi entry in ntuples (%d).",
		  zzp_info->_type);
	  break;
	default:
	  ERROR("Unimplemented");
	  break;
	}
    }

  size_t omit_index = (size_t) -1;

  if (limit_addr)
    {
      // Find limiting item

      map_ntuple_limits::const_iterator i;

      i = ntuple->_limits.find(limit_addr);

      if (i == ntuple->_limits.end())
	ERROR("(Internal error?) Failed to find limiting item...");

      omit_index = i->second._omit_index;
      limit_item = i->second._item;
    }

  size_t omit_index2 = (size_t) -1;

  if (limit2_addr)
    {
      // Find limiting item

      map_ntuple_limits::const_iterator i;

      i = ntuple->_limits.find(limit2_addr);

      if (i == ntuple->_limits.end())
	ERROR("(Internal error?) Failed to find limiting item 2...");

      omit_index2 = i->second._omit_index;
      limit_item2 = i->second._item;
    }

  char name[256];
  {
    signal_id id_fmt(id);

    // Rewrite the name to get rid of unwanted parts (_u32 and data)

    const sig_part &last = id._parts.back();

    if (last._type == SIG_PART_NAME &&
	strcmp(last._id._name,"u32") == 0)
      id_fmt._parts.pop_back();
    else if (last._type == SIG_PART_NAME &&
	     strcmp(last._id._name,"data") == 0)
      {
	if (zzp_info && zzp_info->_type != ZZP_INFO_NONE)
	  {
	    // It is in some kind of vector
	    id_fmt._parts.pop_back();
	  }
	else if (info._type & (ENUM_IS_LIST_LIMIT | ENUM_IS_ARRAY_MASK))
	  {
	    id_fmt._parts.pop_back();
	    id_fmt.push_back("n");
	  }
      }
    if (info._type & ENUM_IS_LIST_INDEX)
      id_fmt.push_back("i");

    strcpy(name,ntuple->_block_prefix);
    size_t off = strlen(name);

    id_fmt.format_paw(name+off,sizeof(name)-off,omit_index,omit_index2);

    //printf ("--- %s --- (%p, %d)\n", name, limit2_addr, limit2_index);
  }

  ntuple_item *item;
  int type = info._type & ENUM_TYPE_MASK;

  switch (type)
    {
    case ENUM_TYPE_DOUBLE:
      item = new ntuple_item(name,(double*) info._addr,ptr_offset);
      break;
    case ENUM_TYPE_FLOAT:
      item = new ntuple_item(name,(float*)  info._addr,ptr_offset);
      break;
    case ENUM_TYPE_DATA12:
      // needs full 16 bits, as it sometimes really is 12 bits+overflow+range
    case ENUM_TYPE_DATA16:
    case ENUM_TYPE_USHORT:
      item = new ntuple_item(name,(ushort*) info._addr,ptr_offset);
      break;
    case ENUM_TYPE_DATA8:
    case ENUM_TYPE_UCHAR:
      item = new ntuple_item(name,(unsigned char*) info._addr,ptr_offset);
      break;
    case ENUM_TYPE_DATA24:
      // could be cut down to 24 bits limits...
    case ENUM_TYPE_DATA32:
    case ENUM_TYPE_UINT:
      item = new ntuple_item(name,(uint*)   info._addr,ptr_offset);
      break;
    case ENUM_TYPE_INT:
      item = new ntuple_item(name,(int*)    info._addr,ptr_offset);
      break;
    case ENUM_TYPE_ULINT:
      item = new ntuple_item(name,(int*)    info._addr,ptr_offset); 
      item->_flags |= NTUPLE_ITEM_IS_ARRAY_MASK;
      break;
    default:
      ERROR("Unhandled ENUM_TYPE (0x%x).",type);
    }

  if (info._type & ENUM_IS_LIST_INDEX)
    {
      assert(item->_type == ntuple_item::INT);
      item->_type = ntuple_item::INT_INDEX_CUT;
    }

  //printf ("paw_ntuple item: %s (%p)  (zzp:%d)\n",
  //	  name,info._addr,zzp_info ? zzp_info->_type : -1);
  
  if (info._type & (ENUM_IS_LIST_LIMIT | ENUM_IS_ARRAY_MASK))
    {
      //printf ("paw_ntuple item limit: %s (%p) (%d)\n",
      //name,info._addr,id._parts.size());

      ntuple_limit_item limit_item;

      limit_item._item = item;
      limit_item._omit_index = id._parts.size();      

      ntuple->_limits.insert(map_ntuple_limits::value_type(info._addr,
							   limit_item));
    }

  if (limit_addr)
    {
      // item->_index_var = ;
      item->_index_var = limit_item->_name;
      item->_index     = (uint) limit_index;

      if (limit2_addr)
       {
         item->_ptr_limit2 = (const int*) limit2_addr; // <- const uint32 *
	 item->_max_limit2 = limit_item2->_limits._int._max;
         item->_index2     = limit2_index;
       }
    }

  /*
  if (info._cwn && cwn_index_postfix)
    {
      item->_index_var = ;
      item->_index     = id.get_index(0);
    }
  */
  if (info._type & ENUM_HAS_INT_LIMIT)
    {
      item->_flags |= NTUPLE_ITEM_HAS_LIMIT;
      item->_limits._int._min = info._min;
      item->_limits._int._max = info._max;
    }

  item->_block = ntuple->_block;

  if (info._type & ENUM_IS_LIST_LIMIT2)
    item->_flags |= NTUPLE_ITEM_OMIT;

  ntuple->_listing->push_back(item);
}

#define NTUPLE_WRITER_UNPACK 0x0001
#define NTUPLE_WRITER_RAW    0x0002
#define NTUPLE_WRITER_CAL    0x0004
#define NTUPLE_WRITER_USER   0x0008

const char *request_level_str(int level)
{
  switch (level)
    {
    case NTUPLE_WRITER_UNPACK: return "UNPACK";
    case NTUPLE_WRITER_RAW:    return "RAW";
    case NTUPLE_WRITER_CAL:    return "CAL";
    case NTUPLE_WRITER_USER:   return "USER";
    case 0:                    return "any";
    default: assert (false);   return "*error*";
    }
}

void paw_ntuple_usage()
{
  printf ("\n");
  printf ("HBOOK/ROOT/STRUCT output (--ntuple) options:\n");
  printf ("\n");
  printf ("UNPACK,RAW,CAL,USER         Include requested level data.\n");
  printf ("[UNPACK|RAW|CAL|USER]:name  Include 'name'd member of level data.\n");
  printf ("UR,URC,URCUS        Prefix UNPACK,RAW,(CAL),(USER) level data with U,R,C,U.\n");
  printf ("CWN                 Produce columnwise HBOOK ntuple (default with .nt*/.hb*).\n");
  printf ("ROOT                Produce ROOT tree (default with .root).\n");
  printf ("STRUCT|SERVER       Run STRUCT server to send data.\n");
  printf ("STRUCT_HH           Produce header file for STRUCT server data.\n");
  printf ("port=N              Run STRUCT server on port N.\n");
  printf ("NOSHM               Do not use shared memory communication.\n");
  printf ("UPPER               Make all variable names upper case.\n");
  printf ("LOWER               Make all variable names lower case.\n");
  printf ("H2ROOT              Make all variable names like h2root.\n");
  printf ("id=ID               Set the ntuple ID.\n");
  printf ("title=TITLE         Set the tree TITLE.\n");
  printf ("ftitle=FTITLE       Set the File TITLE.\n");
  printf ("timeslice=N[:SUB]   Timeslice data, divide in SUBdirectories.\n");
  printf ("autosave[=N]        Autosave root file.\n");
#if defined(USE_LMD_INPUT)
  printf ("rawdata=N           Include raw data, max size N [ki|Mi|Gi].\n");
  printf ("incl=               Subevent inclusion.\n");
  printf ("excl=               Subevent exclusion.\n");
#endif
  printf ("\n");
}

paw_ntuple::paw_ntuple()
{
  _staged = NULL;
#if defined(USE_LMD_INPUT)
  _raw_select = NULL;
  _raw_event = NULL;
#endif
}

paw_ntuple::~paw_ntuple()
{
  delete _staged;
#if defined(USE_LMD_INPUT)
  delete _raw_select;
  delete _raw_event;
#endif
}

paw_ntuple *paw_ntuple_open_stage(const char *command,bool reading)
{
  /* First parse the command.
   *
   * It is a comma separated list of:
   * - Signal id requests, i.e. TFW, N1-5, DSSSD3, etc
   * - Level requests.
   * - Filename (has to be the last one, i.e. no comma after it.
   */

  detector_requests requests;
  int request_level = 0;
  int request_level_detailed = 0;
  int prefix_level = 0;
#if defined(USE_LMD_INPUT)
  uint64_t max_raw_size = 0;
#endif

  paw_ntuple *ntuple = new paw_ntuple;

  if (!ntuple)
    ERROR("Memory allocation error (ntuple: ntuple).");

  ntuple->_staged = new staged_ntuple;

  if (!ntuple->_staged)
    ERROR("Memory allocation error (ntuple: staged).");

#if defined(USE_LMD_INPUT)
  ntuple->_raw_select = new select_event;

  if (!ntuple->_raw_select)
    ERROR("Memory allocation error (ntuple: select).");
#endif

  if (reading)
    ntuple->_staged->_ntuple_type |= NTUPLE_READER_INPUT;   

  const char *req_end;

  while ((req_end = strchr(command,',')) != NULL)
    {
      char *request = strndup(command,(size_t) (req_end-command));

      char *post;

#define MATCH_PREFIX(prefix,post) (strncmp(request,prefix,strlen(prefix)) == 0 && *(post = request + strlen(prefix)) != '\0')
#define MATCH_ARG(name) (strcmp(request,name) == 0)

      // printf ("Request: %s\n",request);

      if (MATCH_ARG("HELP") || MATCH_ARG("help"))
	{
	  paw_ntuple_usage();	
	  exit(0);
	}
      else if (MATCH_ARG("UNPACK"))
	request_level |= NTUPLE_WRITER_UNPACK;
      else if (MATCH_ARG("RAW"))
	request_level |= NTUPLE_WRITER_RAW;
      else if (MATCH_ARG("CAL"))
	request_level |= NTUPLE_WRITER_CAL;
      else if (MATCH_ARG("USER"))
	request_level |= NTUPLE_WRITER_USER;
      else if (MATCH_PREFIX("UNPACK:",post))
	{
	  request_level_detailed |= NTUPLE_WRITER_UNPACK;
	  requests.add_detector_request(post,NTUPLE_WRITER_UNPACK);
	}
      else if (MATCH_PREFIX("RAW:",post))
	{
	  request_level_detailed |= NTUPLE_WRITER_RAW;
	  requests.add_detector_request(post,NTUPLE_WRITER_RAW);
	}
      else if (MATCH_PREFIX("CAL:",post))
	{
	  request_level_detailed |= NTUPLE_WRITER_CAL;
	  requests.add_detector_request(post,NTUPLE_WRITER_CAL);
	}
      else if (MATCH_PREFIX("USER:",post))
	{
	  request_level_detailed |= NTUPLE_WRITER_USER;
	  requests.add_detector_request(post,NTUPLE_WRITER_USER);
	}
      else if (MATCH_ARG("UR"))
	prefix_level |= (NTUPLE_WRITER_UNPACK | NTUPLE_WRITER_RAW);
      else if (MATCH_ARG("URC"))
	prefix_level |= (NTUPLE_WRITER_UNPACK | NTUPLE_WRITER_RAW | 
			 NTUPLE_WRITER_CAL);
      else if (MATCH_ARG("URCUS"))
	prefix_level |= (NTUPLE_WRITER_UNPACK | NTUPLE_WRITER_RAW | 
			 NTUPLE_WRITER_CAL |    NTUPLE_WRITER_USER);
      else if (MATCH_ARG("RWN")) {
	ERROR("Support for row-wise ntuples (RWN) has been removed.");
      }
      else if (MATCH_ARG("CWN"))
	ntuple->_staged->_ntuple_type |= NTUPLE_TYPE_CWN;
      else if (MATCH_ARG("ROOT"))
	ntuple->_staged->_ntuple_type |= NTUPLE_TYPE_ROOT;
      else if (MATCH_ARG("STRUCT_HH"))
	ntuple->_staged->_ntuple_type |= NTUPLE_TYPE_STRUCT_HH;
      else if (MATCH_ARG("STRUCT") || MATCH_ARG("SERVER"))
	ntuple->_staged->_ntuple_type |= NTUPLE_TYPE_STRUCT;
      else if (MATCH_PREFIX("PORT=",post) || MATCH_PREFIX("port=",post))
	ntuple->_staged->_struct_server_port = atoi(post);
      else if (MATCH_ARG("NOEXTERNAL")) {
	ERROR("Support for internal ntuple writing has been removed.");
      }
      else if (MATCH_ARG("NOSHM"))
	ntuple->_staged->_ntuple_type |= NTUPLE_WRITER_NO_SHM;
      else if (MATCH_ARG("UPPER"))
	ntuple->_staged->_ntuple_type |= NTUPLE_CASE_UPPER;
      else if (MATCH_ARG("LOWER"))
	ntuple->_staged->_ntuple_type |= NTUPLE_CASE_LOWER;
      else if (MATCH_ARG("H2ROOT"))
	ntuple->_staged->_ntuple_type |= NTUPLE_CASE_H2ROOT;
      else if (MATCH_PREFIX("ID=",post) || MATCH_PREFIX("id=",post))
	ntuple->_staged->_id = strdup(post);
      else if (MATCH_PREFIX("TITLE=",post) || MATCH_PREFIX("title=",post))
	ntuple->_staged->_title = strdup(post);
      else if (MATCH_PREFIX("FTITLE=",post) || MATCH_PREFIX("ftitle=",post))
	ntuple->_staged->_ftitle = strdup(post);
      else if (MATCH_PREFIX("TIMESLICE=",post) ||
	       MATCH_PREFIX("timeslice=",post))
	{
	  ntuple->_staged->_timeslice = atoi(post);
	  char *colon = strchr(post,':');
	  if (colon)
	    ntuple->_staged->_timeslice_subdir = atoi(colon+1);
	} 
      else if (MATCH_ARG("AUTOSAVE") || MATCH_ARG("autosave"))
	ntuple->_staged->_autosave = 1;
      else if (MATCH_PREFIX("AUTOSAVE=",post) ||
	       MATCH_PREFIX("autosave=",post))
	ntuple->_staged->_autosave = atoi(post);
#if defined(USE_LMD_INPUT)
      else if (MATCH_PREFIX("rawdata=",post))
	max_raw_size = parse_size_postfix(post,"kMG","Rawdata",true);
      else if (MATCH_PREFIX("incl=",post))
	ntuple->_raw_select->parse_request(post,true);
      else if (MATCH_PREFIX("excl=",post))
	ntuple->_raw_select->parse_request(post,false);
#endif
      else
	requests.add_detector_request(request,0);

      free(request);
      command = req_end+1;      
    }

  const char *filename = command;

  if (strcmp(filename,"help") == 0)
    {
      paw_ntuple_usage();	
      exit(0);
    }

  if (!(ntuple->_staged->_ntuple_type & NTUPLE_TYPE_MASK))
    {
      const char *last_slash = strrchr(filename,'/');
      if (!last_slash)
	last_slash = filename;

      if (strstr(last_slash,".nt") || strstr(last_slash,".hb"))
	ntuple->_staged->_ntuple_type |= NTUPLE_TYPE_CWN;
      if (strstr(last_slash,".root"))
	ntuple->_staged->_ntuple_type |= NTUPLE_TYPE_ROOT;

      if (strcmp(last_slash+strlen(last_slash)-2,".h") == 0 ||
	  strcmp(last_slash+strlen(last_slash)-3,".hh") == 0)
	ntuple->_staged->_ntuple_type |= NTUPLE_TYPE_STRUCT_HH;
    }
  if (!(ntuple->_staged->_ntuple_type & NTUPLE_TYPE_MASK))
    ERROR("Cannot determine / no ntuple type given.");

  if (!ntuple->_staged->_ftitle)
    ntuple->_staged->_ftitle = strdup("ucesb");
  if (!ntuple->_staged->_title)
    ntuple->_staged->_title = strdup("CWNtuple");

#if defined(USE_LMD_INPUT)
  if (ntuple->_raw_select->has_selection() &&
      !max_raw_size)
    ERROR("(sub)event selection in ntuple, but no maximum size given.");

  if (max_raw_size)
    {
      ntuple->_raw_event = new lmd_event_out;
      if (!ntuple->_raw_event)
	ERROR("Memory allocation error (ntuple: event_out).");
    }
  else
    {
      delete ntuple->_raw_select;
      ntuple->_raw_select = NULL;
    }
#endif

  requests.prepare();

  vect_ntuple_items listing;

  enumerate_ntuple_info extra;

  extra._listing = &listing;
  extra._requests = &requests;
  extra._block = "";
  extra._level = 0;

  if (!request_level && !request_level_detailed)
    {
#ifdef USER_STRUCT
      request_level |= NTUPLE_WRITER_USER;
#else
      request_level |= NTUPLE_WRITER_RAW;
#endif
    }

  extra._cwn = !!(ntuple->_staged->_ntuple_type & (NTUPLE_TYPE_CWN | 
						   NTUPLE_TYPE_ROOT | 
						   NTUPLE_TYPE_STRUCT_HH | 
						   NTUPLE_TYPE_STRUCT));
  
  if ((request_level | request_level_detailed) & NTUPLE_WRITER_UNPACK)
    {
      extra._level = NTUPLE_WRITER_UNPACK;
      extra._detailed_only = !(request_level & NTUPLE_WRITER_UNPACK);
      extra._block = "UNPACK";
      extra._block_prefix = (prefix_level & NTUPLE_WRITER_UNPACK) ? "U" : "";
      _static_event._unpack.enumerate_members(signal_id(),enumerate_info(),
					      enumerate_member_paw_ntuple,
					      &extra);
    }
  if ((request_level | request_level_detailed) & NTUPLE_WRITER_RAW)
    {
      extra._level = NTUPLE_WRITER_RAW;
      extra._detailed_only = !(request_level & NTUPLE_WRITER_RAW);
      extra._block = "RAW";
      extra._block_prefix = (prefix_level & NTUPLE_WRITER_RAW) ? "R" : "";
      _static_event._raw.enumerate_members(signal_id(),enumerate_info(),
					   enumerate_member_paw_ntuple,
					   &extra);
    }
  if ((request_level | request_level_detailed) & NTUPLE_WRITER_CAL)
    {
      extra._level = NTUPLE_WRITER_CAL;
      extra._detailed_only = !(request_level & NTUPLE_WRITER_CAL);
      extra._block = "CAL";
      extra._block_prefix = (prefix_level & NTUPLE_WRITER_CAL) ? "C" : "";
      _static_event._cal.enumerate_members(signal_id(),enumerate_info(),
					   enumerate_member_paw_ntuple,
					   &extra);
    }
#ifdef USER_STRUCT
  if ((request_level | request_level_detailed) & NTUPLE_WRITER_USER)
    {
      extra._level = NTUPLE_WRITER_USER;
      extra._detailed_only = !(request_level & NTUPLE_WRITER_USER);
      extra._block = "USER";
      extra._block_prefix = (prefix_level & NTUPLE_WRITER_USER) ? "US" : "";
      _static_event._user.enumerate_members(signal_id(),enumerate_info(),
					    enumerate_member_paw_ntuple,
					    &extra);
    }
#endif

  for (uint i = 0; i < requests._requests.size(); i++)
    if (!requests._requests[i]._checked)
      ERROR("NTuple request for item %s (level %s) was not considered.  "
	    "Does that detector exist?",
	    requests._requests[i]._str,
	    request_level_str(requests._requests[i]._level));

  int hid = 101;

  if (ntuple->_staged->_ntuple_type & NTUPLE_TYPE_CWN)
    {
      if (ntuple->_staged->_id)
	{
	  // The id must be numeric
	  
	  char *end;
	  
	  hid = (int) strtol(ntuple->_staged->_id,&end,10);
	  
	  if (end == ntuple->_staged->_id || *end != 0)
	    ERROR("HBOOK ntuple ID (%s) must be numeric.",
		  ntuple->_staged->_id);
	}
      if (strlen(ntuple->_staged->_title) > 255)
	ERROR("HBOOK ntuple title (%s) too long (max 255 chars).",
	      ntuple->_staged->_title);
      if (strlen(ntuple->_staged->_ftitle) > 16)
	ERROR("HBOOK file title (%s) too long (max 16 chars).",
	      ntuple->_staged->_ftitle);
    }

  if (!ntuple->_staged->_id)
    ntuple->_staged->_id = strdup("h101");

  ntuple->_staged->open(filename); // open file/start the writer

  ntuple->_staged->stage(listing,hid,&_static_event
#if defined(USE_LMD_INPUT)
		    ,1
		    ,(uint) ((max_raw_size + sizeof(uint)-1) / sizeof(uint))
#endif
		    );

  return ntuple;
}

#if defined(USE_LMD_INPUT)
void paw_ntuple_copy_raw(fill_raw_info *fill_raw)
{
  // printf ("FILL RAW\n");
  // for (uint32_t i = 0; i < fill_raw->_words; i++)
  //   fill_raw->_ptr[i] = 0xaa0000bb | (i << 8);

  char *ptr = (char *) fill_raw->_ptr;

  paw_ntuple *ntuple = (paw_ntuple *) fill_raw->_extra;  
  lmd_event_out *event = ntuple->_raw_event;

  const buf_chunk_swap *chunk_cur = event->_chunk_start;

  // We always write in big-endican = network order
  const bool write_native = (__BYTE_ORDER == __BIG_ENDIAN);

  for ( ; chunk_cur < event->_chunk_end; chunk_cur++)
    {
      copy_to_buffer(ptr, chunk_cur->_ptr,
		     chunk_cur->_length,
                     !(write_native ^ chunk_cur->_swapping));

      ptr += chunk_cur->_length;
    }

  assert (ptr == (char *) (fill_raw->_ptr + fill_raw->_words));
}
#endif

void paw_ntuple::event()
{
#if defined(USE_LMD_INPUT)
  fill_raw_info fill_raw;

  if (_raw_event)
    {
      size_t length = _raw_event->get_length();

      length = (length + (sizeof (uint32_t) - 1)) / sizeof (uint32_t);

      if (length > _staged->_ext->_max_raw_words)
	ERROR("Too many words (%zd bytes > %zd bytes) "
	      "in raw data for ntuple (struct) output.",
	      length * sizeof (uint32_t),
	      _staged->_ext->_max_raw_words * sizeof (uint32_t));

      fill_raw._words = (uint32_t) length;
      fill_raw._ptr = NULL;
      fill_raw._callback = paw_ntuple_copy_raw;
      fill_raw._extra = this;

      // printf ("RAW: %d\n",fill_raw._words);
    }
#endif

  _staged->event(&_static_event
#if defined(USE_LMD_INPUT)
		 ,&_static_event._unpack.event_no
		 ,_raw_event ? &fill_raw : NULL
#endif
		 );
}

bool paw_ntuple::get_event()
{
  return _staged->get_event();
}

void paw_ntuple::unpack_event()
{
  _staged->unpack_event(&_static_event);
}

void paw_ntuple::close()
{
  _staged->close();
  delete _staged;
  _staged = NULL;
}
