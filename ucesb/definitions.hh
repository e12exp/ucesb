
#ifndef __DEFINITIONS_HH__
#define __DEFINITIONS_HH__

#include "file_line.hh"

#include <vector>
#include <map>

#include "node.hh"

#include "bits_spec.hh"

#include "structure.hh"
#include "signal.hh"

#include "str_set.hh"

typedef std::map<const char *,struct_definition *,
		 compare_str_less> named_structure_map;
typedef std::map<const char *,signal_spec *,
		 compare_str_less>       signal_map;
typedef std::multimap<const char *,signal_spec *,
		      compare_str_less>       signal_multimap;

typedef std::multimap<const char *,signal_info *,
		      compare_str_less>       signal_info_map;

extern named_structure_map all_named_structures;
extern named_structure_map all_subevent_structures;

extern signal_map all_signals;
extern signal_multimap all_signals_no_ident;

extern signal_info_map all_signal_infos;

extern event_definition *the_event;

void map_definitions();
void check_consistency();
void dump_definitions();
void generate_unpack_code();
void generate_signals();

struct_definition *find_named_structure(const char *name);
struct_definition *find_subevent_structure(const char *name);

#endif//__DEFINITIONS_HH__
