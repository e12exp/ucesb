
#ifndef __SIGNAL_ID_MAP_HH__
#define __SIGNAL_ID_MAP_HH__

#include "../common/signal_id.hh"

#include "enumerate.hh"

#define SID_MAP_UNPACK     0x01
#define SID_MAP_RAW        0x02
#define SID_MAP_CAL        0x03
#define SID_MAP_USER       0x04
#define SID_MAP_CALIB      0x05

#define SID_MAP_MIRROR_MAP 0x08
#define SID_MAP_REVERSE    0x10

#define SID_MAP_MAX_NUM    0x20

class prefix_units_exponent;

struct signal_id_info
{

public:
  int         _type;
  const void *_addr;
  const prefix_units_exponent *_unit;

public:
  set_dest_fcn _set_dest; // only declared when part of a SID_MAP_MIRROR_MAP
};

struct signal_id_zzp_info
{
  size_t _zzp_part;
};

struct enumerate_msid_info
{
  int _map_no;
};

struct sid_leaf;
struct sid_node;

template<int create_missing>
sid_leaf *find_leaf(const signal_id &id,
		    sid_node **base_ptr);

signal_id_info *find_signal_id_info(const signal_id &id,
				    enumerate_msid_info *extra);
const signal_id_info *get_signal_id_info(const signal_id &id,
					 int map_no);

const signal_id_zzp_info *get_signal_id_zzp_info(const signal_id &id,
						 int map_no);

void setup_signal_id_map();

void setup_signal_id_map_unpack_map(void *extra);
void setup_signal_id_map_raw_map(void *extra);

void setup_signal_id_map_raw_reverse_map(void *extra);

#endif//__SIGNAL_ID_MAP_HH__
