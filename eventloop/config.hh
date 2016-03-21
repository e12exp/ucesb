
#ifndef __CONFIG_HH__
#define __CONFIG_HH__

#include <vector>

#include <stdint.h>

// Data source
struct config_input
{
  int _type;
  const char *_name;  
};

struct config_output
{
  int _type;
  const char *_name;
};

typedef std::vector<config_input> config_input_vect;

typedef std::vector<config_output> config_output_vect;

typedef std::vector<char *> config_calib_vect;

typedef std::vector<char *> config_command_vect;

struct config_opts
{
  // General info control
  int _debug;
  int _quiet;

  int _io_error_fatal;

  int _allow_errors;
  int _broken_files;

  int _print_buffer; 
  int _print; // Print events
  int _data;  // Print event data
  int _reverse;

  int _show_members;
  int _event_sizes;
  int _show_calib;
  int _ts_align_hist_mode;

  uint64_t _max_events; 
  int _skip_events; 
  int _first_event; 
  int _last_event; 
  int _downscale;

#ifdef USE_LMD_INPUT
  int _scramble;
#endif

#ifdef USE_LMD_INPUT
  int _event_stitch_mode;
  int _event_stitch_value;
#endif

#ifdef USE_LMD_INPUT
  // Data dest
  struct
  {
    const char *_name; // NULL if inactive 
  } _file_output_bad;
#endif

  struct
  {
    const char *_command; // NULL if inactive
  } _paw_ntuple;

  struct
  {
    const char *_command; // NULL if inactive
  } _watcher;

  struct
  {
    const char *_command; // NULL if inactive
  } _dump;

  int _num_threads;
  int _progress;

  int _files_open_ahead;

#ifdef USE_MERGING
  int _merge_concurrent_files;
  int _merge_event_mode;
#endif

  int _no_mmap;
};

extern config_opts _conf;

extern config_input_vect _inputs;

extern config_output_vect _outputs;

extern config_calib_vect _conf_calib;

extern config_command_vect _corr_commands;

#endif /* __CONFIG_HH__ */
