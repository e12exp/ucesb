
#ifndef __PAW_NTUPLE_HH__
#define __PAW_NTUPLE_HH__

#if defined(USE_CERNLIB) || defined(USE_ROOT) || defined(USE_EXT_WRITER)

class staged_ntuple;
class select_event;
class lmd_event_out;

class paw_ntuple
{
public:
  paw_ntuple();
  virtual ~paw_ntuple();

public:
  staged_ntuple *_staged;

#if defined(USE_LMD_INPUT)
  // For raw data output:
  select_event *_raw_select;
  // The accumulated raw output data
  lmd_event_out *_raw_event;
#endif

public:
  void open_stage(const char *command,bool reading);
  void event(); // write
  bool get_event(); // read I
  void unpack_event(); // read II
  void close();
};

paw_ntuple *paw_ntuple_open_stage(const char *command,bool reading);

#endif

#endif//__PAW_NTUPLE_HH__
