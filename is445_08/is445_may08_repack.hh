// This file comes from CVS: isodaq/checker/is430_aug05_unpack.hh
// Name has been changed sine it will undergo major changes/adaptions.

#ifndef __IS445_MAY08_UNPACK_HH__
#define __IS445_MAY08_UNPACK_HH__

#include "../is430_05/scaler_timer.hh"

#define DATA_OVERFLOW_MARKER 0x1000

struct event_te
{
public:
  uint16 e;
  uint16 t;
  
public:
  void add_e(uint32 value) { e = (uint16) value; }
  void add_t(uint32 value) { t = (uint16) value; }
};

struct event_dsssd_strip : 
  public event_te
{
public:
  uint16 strip;
};

struct event_dsssd_strips
{
public:
  int                 n;
  event_dsssd_strip   data[32];
  
public:
  void add_e(uint32 channel,uint32 value)
  {
    /* First see if we know the strip already.
     */
    
    for (int i = 0; i < n; i++)
      if (data[i].strip == channel)
	{
	  data[i].add_e(value);
	  return;
	}
    
    /* We do now know the strip. */
    
    data[n].strip = (uint16) channel;
    data[n].add_e(value);
    n++;
  }
  
  void add_t(uint32 channel,uint32 value)
  {
    /* First see if we know the strip already.
     */
    
    for (int i = 0; i < n; i++)
      if (data[i].strip == channel)
	{
	  data[i].add_t(value);
	  return;
	}
    
    /* We do now know the strip. */
    
    data[n].strip = (uint16) channel;
    data[n].add_t(value);
    n++;
  }
};

struct event_dsssd
{
public:
  event_dsssd_strips   front;
  event_dsssd_strips   back;
};

struct event_is445_may08
{
public:
  /* Trigger.
   */

  uint32        trigger;

  /* Event counter.
   */
  
  uint32        ievent;
  
  /* From converters.
   */
  
  event_dsssd   dsssd[1];  
  event_te      back[1];
  event_te      mon[1];

  uint16        t_trig;
  uint16        t_dsssd_front;
  uint16        t_dsssd_back;

  /* From scaler.
   */
  
  uint32        scaler[32];
  uint32        scaler_mask;

  /* Calculated.
   */

  uint32        time_t2;   /* Time since last proton pulse. */
  uint32        time_ebis; /* Time since last ebis pulse. */  
};

/* ntuple names:
 *
 * DSSSDx2:
 *
 * Block: 'STRIPn'
 *
 * NFnE[0:32]:I
 * NBnE[0:32]:I
 * NFnT[0:32]:I
 * NBnT[0:32]:I
 *
 * NSFnE(NFnE)[0:32]:I
 * NSBnE(NBnE)[0:32]:I
 * NSFnT(NFnE)[0:32]:I
 * NSBnT(NBnE)[0:32]:I
 *
 * EnF(NFnE)[-999/0,4095]:I
 * EnB(NBnE)[-999/0,4095]:I
 * TnF(NFnE)[-999/0,4095]:I
 * TnB(NBnE)[-999/0,4095]:I
 *
 * Block: 'SINGLES'
 * 
 * E1
 * E2
 * T1
 * T2
 *
 * MON_DE_E
 * MON_E_E
 * MON_DE_T
 * MON_E_T
 *
 * Block: 'OTHERS'
 * 
 * TSHORT
 * TEBIS
 * IEVENT
 */

typedef int   int32;
typedef short int16;

struct ntuple_strips
{
  int32  nfe;
  int32  nbe;
  int32  nft;
  int32  nbt;

  int32  nsfe[32]; /* nfe */
  int32  nsbe[32]; /* nbe */
  int32  nsft[32]; /* nft */
  int32  nsbt[32]; /* nbt */

  int32  fe[32];   /* nfe */
  int32  be[32];   /* nbe */
  int32  ft[32];   /* nft */
  int32  bt[32];   /* nbt */
};

struct ntuple_singles
{
  int32  e1back;
  int32  t1back;

  int32  emon;

  int32  ttrigger;

  int32  f1ct;
  int32  b1ct;
};


struct ntuple_others
{
  int32  ievent;
  int32  tshort;
  int32  tebis;
  int32  trigger;
};

struct ntuple_event
{
  ntuple_strips  strips[1];
  ntuple_singles singles;
  ntuple_others  others;
  // int32 dummy[0x1000];
};

class is445_may08_unpack
{
public:
  is445_may08_unpack();
  
public:
  event_is445_may08 event;

  ntuple_event      ntup_event;

public:
  scaler_timer tshort;
  scaler_timer tebis;

  uint32 last_num_t2;
  uint32 last_num_ebis;
  uint32 last_time;

public:
  int    _conf_quiet;
  int    _warn_ebis_missed;

public:
  bool file_open;
  
public:
  void clear();
  void invalidate_scaler_state();
  
public:
  void set_trigger(uint32 trig);

  void set_scaler_value(uint32 channel,uint32 value);
  void set_adc(uint32 n_adc,uint32 channel,uint32 value);
  void set_tdc(uint32 n_tdc,uint32 channel,uint32 value);

public:
  void process();
  void output_event();

public:
  void open_output(const char *filename);
  void close_output();
};

#endif/*__IS445_MAY08_UNPACK_HH__*/
