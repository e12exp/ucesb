
#ifndef __CROS3_REWRITE_CORRELATION_HH__
#define __CROS3_REWRITE_CORRELATION_HH__

#define USER_DEF_CROS3_REWRITE // prevent generated definition

class CROS3_REWRITE_correlation
{
public:
  CROS3_REWRITE_correlation()
  {
    _index = -1;
  }

public:
  int _index;

public:
  bool enumerate_correlations(const signal_id &id,enumerate_correlations_info *info) 
  { 
    if (!info->_requests->is_channel_requested(id,false))
      return false;

    _index = info->_next_index;
    info->_next_index += 256;
    
    return true; 
  }

  void add_corr_members(const CROS3_REWRITE &src,correlation_list *list) const 
  {
    if (_index == -1)
      return;

    // We need to go through the data we have, find out what channels
    // have fired, and add them to the list.  This exercise is
    // necessary, as they may be both in disorder and have multiple
    // hits.

    // bitsone<256> wires;

    


    
  }
};

#endif//__CROS3_REWRITE_CORRELATION_HH__
