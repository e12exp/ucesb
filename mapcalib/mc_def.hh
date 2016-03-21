
#ifndef __MC_DEF_HH__
#define __MC_DEF_HH__

#include <vector>

#include "../common/signal_id.hh"
#include "../common/node.hh"

extern def_node_list *all_mc_defs;

#include "map_info.hh"
#include "calib_info.hh"

char *argv0_replace(const char *filename);

void read_map_calib_info();

void process_map_calib_info();

const signal_id_info *get_signal_id_info(signal_id *id,
					 int map_no);

void verify_units_match(const prefix_units_exponent *dest,
			const units_exponent *src,
			const file_line &loc,
			const char *descr,double &factor);

#endif//__MC_DEF_HH__
