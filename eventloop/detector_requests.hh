
#ifndef __DETECTOR_REQUESTS_HH__
#define __DETECTOR_REQUESTS_HH__

#include "signal_id_range.hh"

#include <vector>

struct detector_request
{
public:
  detector_request(const signal_id_range &id,const char *str,
		   int level,bool exclude)
  {
    _id = id;
    _level = level;
    _exclude = exclude;
    _checked = false;
    _str = strdup(str);
  }

public:
  signal_id_range _id;
  const char *_str;
  int _level;
  int _exclude;

public:
  bool        _checked; ///< Where we at all considered?
};

typedef std::vector<detector_request> vect_detector_requests;

struct detector_requests
{
public:
  detector_requests()
  {
    _global_requests     = false;
    _global_is_exclude   = false;
  }

public:
  vect_detector_requests  _requests;
  bool                    _global_requests;
  bool                    _global_is_exclude;

public:
  void add_detector_request(const char *request,
			    int level);

  void prepare(); // call after list filled, before channel selection

public:
  bool is_channel_requested(const signal_id& id,bool sub_channel,
			    int level, bool detailed_only);
};


#endif//__DETECTOR_REQUESTS_HH__
