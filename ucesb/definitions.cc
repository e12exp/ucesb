
#include "definitions.hh"
#include "parse_error.hh"
#include "unpack_code.hh"

#include "signal.hh"

def_node_list *all_definitions;

named_structure_map all_named_structures;
named_structure_map all_subevent_structures;

signal_map all_signals;
signal_multimap all_signals_no_ident;

signal_info_map all_signal_infos;

event_definition *the_event;

struct_definition *find_named_structure(const char *name)
{
  named_structure_map::iterator i;

  i = all_named_structures.find(name);

  if (i == all_named_structures.end())
    return NULL;

  return i->second; 
}

struct_definition *find_subevent_structure(const char *name)
{
  named_structure_map::iterator i;

  i = all_subevent_structures.find(name);

  if (i == all_subevent_structures.end())
    return NULL;

  return i->second; 
}

void insert_signal_to_all(signal_spec *signal)
{
  if (all_signals.find(signal->_name) !=
      all_signals.end())
    ERROR_LOC_PREV(signal->_loc,
		   all_signals.find(signal->_name)->second->_loc,
		   "Several signals with name: %s\n",
		   signal->_name);
  
  all_signals.insert(signal_map::value_type(signal->_name,
					    signal));
}

void insert_signal_to_no_ident(signal_spec *signal)
{
  all_signals_no_ident.insert(signal_multimap::value_type(signal->_name,
							  signal));
}

void insert_signal_info(signal_info *sig_info)
{
  all_signal_infos.insert(signal_info_map::value_type(sig_info->_name,
						      sig_info));
}

void map_definitions()
{
  if (!all_definitions)
    return;

  def_node_list::iterator i;

  for (i = all_definitions->begin(); i != all_definitions->end(); ++i)
    {
      def_node* node = *i;

      struct_definition *structure = 
	dynamic_cast<struct_definition *>(node);

      if (structure)
	{
	  const struct_header_subevent *subevent = 
	    dynamic_cast<const struct_header_subevent *>(structure->_header);

	  if (subevent)
	    {
	      if (all_subevent_structures.find(subevent->_name) !=
		  all_subevent_structures.end())
		ERROR_LOC_PREV(structure->_loc,
			       all_subevent_structures.find(subevent->_name)->second->_loc,
			       "Several subevent structures with name: %s\n",
			       subevent->_name);
	      
	      all_subevent_structures.insert(named_structure_map::value_type(subevent->_name,
									     structure));
	    }
	  else
	    {
	      const struct_header_named *named = 
		dynamic_cast<const struct_header_named *>(structure->_header);
	      
	      if (named)
		{
		  if (all_named_structures.find(named->_name) !=
		      all_named_structures.end())
		    ERROR_LOC_PREV(structure->_loc,
				   all_named_structures.find(named->_name)->second->_loc,
				   "Several structures with name: %s\n",
				   named->_name);
		  
		  all_named_structures.insert(named_structure_map::value_type(named->_name,
									      structure));
		}
	    }
	}

      signal_spec *signal = 
	dynamic_cast<signal_spec *>(node);

      if (signal)
	{
	  signal_spec_range *signal_range = 
	    dynamic_cast<signal_spec_range *>(signal);

	  if (signal_range)
	    expand_insert_signal_to_all(signal_range);
	  else if (signal->_ident)	    
	    insert_signal_to_all(signal);
	  else
	    insert_signal_to_no_ident(signal);
	}

      signal_info *sig_info = 
	dynamic_cast<signal_info *>(node);

      if (sig_info)
	{
	  insert_signal_info(sig_info);
	}

      event_definition *event =
	dynamic_cast<event_definition *>(node);

      if (event)
	{
	  if (the_event)
	    ERROR_LOC_PREV(event->_loc,the_event->_loc,"Several event defintions.\n");
	  the_event = event;
	}
    }
  
  if (!the_event)
    {
      // please add an EVENT to your .spec file!
      ERROR_LOC(file_line(_first_lineno_map ? _first_lineno_map->_internal : 0),"There is no event defintion (EVENT { }) in the specification."); 
    }
}

void generate_unpack_code()
{
  named_structure_map::iterator i;

  printf ("/**********************************************************\n"
          " * Generating unpacking code...\n"
          " */\n\n");

  for (i = all_named_structures.begin(); i != all_named_structures.end(); ++i)
    generate_unpack_code(i->second);

  for (i = all_subevent_structures.begin(); i != all_subevent_structures.end(); ++i)
    generate_unpack_code(i->second);

  if (the_event)
    generate_unpack_code(the_event);

  printf ("/**********************************************************/\n");
}

void check_consistency()
{

}

/* Just so we can see that we parsed everything...
 */

void dump_definitions()
{
  print_header("INPUT_DEFINITION","All specifications as seen by the parser.");
  printf ("/**********************************************************\n"
          " * Dump of all structures:\n"
          " */\n\n");

  dumper_dest_file d_dest(stdout);
  dumper d(&d_dest);

  {
    named_structure_map::iterator i;
    
    for (i = all_named_structures.begin(); i != all_named_structures.end(); ++i)
      {
	i->second->dump(d);
	d.nl();
      }
    
    for (i = all_subevent_structures.begin(); i != all_subevent_structures.end(); ++i)
      {
	i->second->dump(d);
	d.nl();
      }
  }

  printf ("/**********************************************************\n"
          " * The event definition:\n"
          " */\n\n");

  if (the_event)
    {
      the_event->dump(d);
      d.nl();
    }

  printf ("/**********************************************************\n"
          " * Signal name mappings:\n"
          " */\n\n");

  {
    signal_map::iterator i;

    for (i = all_signals.begin(); i != all_signals.end(); ++i)
      i->second->dump(d);
  }

  {
    signal_multimap::iterator i;

    for (i = all_signals_no_ident.begin(); i != all_signals_no_ident.end(); ++i)
      i->second->dump(d);
  }

  {
    signal_info_map::iterator i;

    for (i = all_signal_infos.begin(); i != all_signal_infos.end(); ++i)
      i->second->dump(d);
  }

  printf ("/**********************************************************/\n");
  print_footer("INPUT_DEFINITION");
}
