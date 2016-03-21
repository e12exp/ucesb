#ifndef __RAW_DATA_CORRELATION_HH__
#define __RAW_DATA_CORRELATION_HH__

#include "typedef.hh"

#include "detector_requests.hh"

struct enumerate_correlations_info
{
  int                _next_index;
  detector_requests *_requests;
};

struct correlation_list;

template<typename T>
class data_correlation
{
public:
  data_correlation()
  {
    _index = -1;
  }

public:
  int _index;

public:
  // void set_dest(T *dest);

public:
  void add_corr_members(const T &src,correlation_list *list WATCH_MEMBERS_PARAM) const;
  bool enumerate_correlations(const signal_id &id,enumerate_correlations_info *info);
};

typedef data_correlation<uint8>  uint8_correlation;
typedef data_correlation<uint16> uint16_correlation;
typedef data_correlation<uint32> uint32_correlation;
typedef data_correlation<uint64> uint64_correlation;

typedef data_correlation<DATA8>  DATA8_correlation;
typedef data_correlation<DATA12> DATA12_correlation;
typedef data_correlation<DATA12_RANGE> DATA12_RANGE_correlation;
typedef data_correlation<DATA12_OVERFLOW> DATA12_OVERFLOW_correlation;
typedef data_correlation<DATA16> DATA16_correlation;
typedef data_correlation<DATA24> DATA24_correlation;
typedef data_correlation<DATA32> DATA32_correlation;
typedef data_correlation<DATA64> DATA64_correlation;
typedef data_correlation<rawdata8>  rawdata8_correlation;
typedef data_correlation<rawdata12> rawdata12_correlation;
typedef data_correlation<rawdata16> rawdata16_correlation;
typedef data_correlation<rawdata24> rawdata24_correlation;
typedef data_correlation<rawdata32> rawdata32_correlation;
typedef data_correlation<rawdata64> rawdata64_correlation;
typedef data_correlation<float>  float_correlation;
typedef data_correlation<double> double_correlation;

// TODO: Make sure that the user cannot specify source array indices
// in SIGNAL which are outside the available items.  Bad names get
// caught by the compiler, array indices not.

template<typename Tsingle_correlation,typename Tsingle,typename T_correlation,typename T,int n>
class raw_array_correlation
{
public:
  T_correlation _items[n];

public:
  T_correlation &operator[](size_t i) 
  { 
    // This function is used by the setting up of the arrays, i.e. we
    // can have checks here
    if (i < 0 || i >= n) 
      ERROR("Correlation index outside bounds (%d >= %d)",i,n); 
    return _items[i]; 
  }
  const T_correlation &operator[](size_t i) const 
  {
    // This function is used by the mapping operations (since that one
    // needs a const function), no checks here (expensive, since
    // called often)
    return _items[i]; 
  }

public:
  static void add_corr_item(const T &src,const T_correlation &watch,correlation_list *list WATCH_MEMBERS_PARAM);

public:
  void add_corr_members(const raw_array<Tsingle,T,n> &src,correlation_list *list WATCH_MEMBERS_PARAM) const;
  void add_corr_members(const raw_array_zero_suppress<Tsingle,T,n> &src,correlation_list *list WATCH_MEMBERS_PARAM) const;
  template<int max_entries>
  void add_corr_members(const raw_array_multi_zero_suppress<Tsingle,T,n,max_entries> &src,correlation_list *list WATCH_MEMBERS_PARAM) const;
  void add_corr_members(const raw_list_zero_suppress<Tsingle,T,n> &src,correlation_list *list WATCH_MEMBERS_PARAM) const;
  void add_corr_members(const raw_list_ii_zero_suppress<Tsingle,T,n> &src,correlation_list *list WATCH_MEMBERS_PARAM) const;

public:
  bool enumerate_correlations(const signal_id &id,enumerate_correlations_info *info);

};

template<typename Tsingle_correlation,typename Tsingle,typename T_correlation,typename T,int n,int n1>
class raw_array_correlation_1 :
  public raw_array_correlation<Tsingle_correlation,Tsingle,T_correlation,T,n>
{
public:
  bool enumerate_correlations(const signal_id &id,enumerate_correlations_info *info);
};

template<typename Tsingle_correlation,typename Tsingle,typename T_correlation,typename T,int n,int n1,int n2>
class raw_array_correlation_2 :
  public raw_array_correlation<Tsingle_correlation,Tsingle,T_correlation,T,n>
{
public:
  bool enumerate_correlations(const signal_id &id,enumerate_correlations_info *info);
};


// For the mirror data structures, which will use the
// raw_array_zero_suppress_correlation name
#define raw_array_zero_suppress_correlation   raw_array_correlation
#define raw_list_zero_suppress_correlation    raw_array_correlation
#define raw_list_ii_zero_suppress_correlation raw_array_correlation

#define raw_array_zero_suppress_1_correlation raw_array_correlation_1
#define raw_array_zero_suppress_2_correlation raw_array_correlation_2
#define raw_list_zero_suppress_1_correlation  raw_array_correlation_1
#define raw_list_zero_suppress_2_correlation  raw_array_correlation_2

// The multi-entry array should eat one index

template<typename Tsingle_correlation,typename Tsingle,typename T_correlation,typename T,int n,int max_entries>
class raw_array_multi_correlation :
  public raw_array_correlation<Tsingle_correlation,Tsingle,T_correlation,T,n>
{
};

#define raw_array_multi_zero_suppress_correlation raw_array_multi_correlation

class unpack_subevent_base_correlation
{
public:
  void add_corr_members(const unpack_subevent_base &src,correlation_list *list WATCH_MEMBERS_PARAM) const { }
  bool enumerate_correlations(const signal_id &id,enumerate_correlations_info *info) { return false; }

};

class unpack_event_base_correlation
{
public:
  void add_corr_members(const unpack_event_base &src,correlation_list *list WATCH_MEMBERS_PARAM) const { }
  bool enumerate_correlations(const signal_id &id,enumerate_correlations_info *info) { return false; }

};

class raw_event_base_correlation
{
public:
  void add_corr_members(const raw_event_base &src,correlation_list *list WATCH_MEMBERS_PARAM) const { }
  bool enumerate_correlations(const signal_id &id,enumerate_correlations_info *info) { return false; }

};

class cal_event_base_correlation
{
public:
  void add_corr_members(const cal_event_base &src,correlation_list *list WATCH_MEMBERS_PARAM) const { }
  bool enumerate_correlations(const signal_id &id,enumerate_correlations_info *info) { return false; }

};

#endif//__RAW_DATA_CORRELATION_HH__

