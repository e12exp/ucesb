
#include "detector_requests.hh"
#include "error.hh"

void detector_requests::add_detector_request(const char *request,
					     int level)
{
  signal_id_range id;
  int exclude = false;

  if (request[0] == '!')
    {
      exclude = true;
      request++;
    }
  
  dissect_name_range(request,id);
  _requests.push_back(detector_request(id,request,level,exclude));
}

void detector_requests::prepare()
{
  bool global_exclude = false;
  bool global_include = false;
  
  for (unsigned int i = 0; i < _requests.size(); i++)
    {
      detector_request* request = &(_requests[i]);

      if (request->_level) // detailed
	{

	}
      else // global
	{
	  _global_requests = true;
	  if (request->_exclude)
	    global_exclude = true;
	  else
	    global_include = true;
	}
    }

  if (global_exclude && global_include)
    ERROR("Bad channel request list, "
	  "both global includes and global excludes.");
  
  _global_is_exclude = global_exclude;
}

bool detector_requests::is_channel_requested(const signal_id& id,
					     bool sub_channel,
					     int level,
					     bool detailed_only)
{
#define DETAILED           2

#define ACCEPTED           0
#define REJECTED           1
#define ACCEPTED_DETAILED  (ACCEPTED | DETAILED)
#define REJECTED_DETAILED  (REJECTED | DETAILED)

  bool match[4] = { false, false, false, false };
  
  for (unsigned int i = 0; i < _requests.size(); i++)
    {
      detector_request* request = &(_requests[i]);
	  
      // Now we need to compare the id to the request.

      if (request->_level &&
	  request->_level != level)
	continue; // level match failure

      if (request->_id.encloses(id,sub_channel))
	{
	  request->_checked = true;

	  int detailed = request->_level ? DETAILED : 0;

	  if (request->_exclude)
	    match[REJECTED | detailed] = true;
	  else
	    match[ACCEPTED | detailed] = true;
	}
    }

  // A detailed request trumps anything else
  if (match[ACCEPTED_DETAILED])
    return true;

  // If this level only has detailed requests, we cannot be accepted
  if (detailed_only)
    return false;

  if (_global_is_exclude)
    {
      // Anything which is not expressly rejected is included
      if (match[REJECTED])
	return false;

      assert(!match[ACCEPTED]); // or we have both accept and reject requests!

      return true;
    }
  else
    {
      // If it is detailed rejected, that trumps normal inclusion
      if (match[REJECTED_DETAILED])
	return false;

      // If it accepted
      if (match[ACCEPTED])
	return true;

      assert(!match[REJECTED]); // or we have both accept and reject requests!

      // With no global request list, any item on levels requested is accepted.
      if (!_global_requests)
	return true;

      return false;
    }
}

