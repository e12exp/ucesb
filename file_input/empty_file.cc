#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <time.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define USE_EBYE_INPUT_32

#include "endian.hh"

#include "lmd_event.hh"
#include "ebye_event.hh"
#include "pax_event.hh"
#include "hld_event.hh"

#include "titris_stamp.hh"
#include "wr_stamp.hh"

#include "../hbook/example/test_caen_v775_data.h"
#include "../hbook/example/test_caen_v1290_data.h"

// This program creates a stream of data representing empty events on
// stdout.  It can be used to test the basic event processing of
// almost any unpacker, as empty events usually are valid (altough
// they perhaps shouldn't).  One can also test the processing overhead
// for an event as such.


#ifdef __MACH__
#include <sys/time.h>
//clock_gettime is not implemented on OSX
int clock_gettime(int /*clk_id*/, struct timespec* t) {
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
#define CLOCK_REALTIME 1
#endif


void usage(char *cmdname)
{
  printf ("Create empty buffers on stdout.\n\n");
  printf ("%s options\n\n",cmdname);

  printf ("  --buffer-size=N   Write buffers of size N.\n");
  printf ("  --buffers=N       Write at most N buffers.\n");
  printf ("  --events=N        Write at most N events.\n");
  printf ("  --event-size=N    Write events of size ~N (LMD only).\n");
  printf ("  --subevent-size=N  Write subevents of size ~N (LMD only).\n");
  printf ("  --random-size     Random event sizes (up to size given).\n");
  printf ("  --random-trig     Random trigger no (LMD only).\n");
  printf ("  --titris-stamp=N  Write titris stamp in first subevent, id=N (LMD only).\n");
  printf ("  --wr-stamp=N      Write WR stamp in first subevent, id=N (LMD only).\n");
  printf ("  --bad-stamp=N     Write bad stamps every so often.\n");
  printf ("  --caen-v775=N     Write CAEN V775 subevent.\n");
  printf ("  --caen-v1290=N    Write CAEN V1290 subevent.\n");
  printf ("  --empty-buffers   No events.\n");
  printf ("  --rate=N          At most, output N events/s.\n");

  printf ("  --lmd             Write LMD format data.\n");
  printf ("  --ebye            Write EBYE format data.\n");
  printf ("  --pax             Write PAX format data.\n");
  printf ("  --hld             Write HLD format data.\n");
}

#define WRITE_FORMAT_LMD   1
#define WRITE_FORMAT_EBYE  2
#define WRITE_FORMAT_PAX   3
#define WRITE_FORMAT_HLD   4 

struct config
{
  uint _buffer_size;
  int  _empty_buffers;

  uint _event_size;
  uint _subevent_size;

  int  _random_size;
  int  _random_trig;

  int  _titris_stamp;
  int  _wr_stamp;

  int  _bad_stamp;

  int  _caen_v775;
  int  _caen_v1290;

  uint64 _max_buffers;
  uint64 _max_events;

  int  _max_rate;

  uint _format;
};

config _conf;

void write_data_lmd();
void write_data_ebye();
void write_data_pax();
void write_data_hld();

int main(int argc,char *argv[])
{
  memset(&_conf,0,sizeof(_conf));

  for (int i = 1; i < argc; i++)
    {
      char *post;

#define MATCH_PREFIX(prefix,post) (strncmp(argv[i],prefix,strlen(prefix)) == 0 && *(post = argv[i] + strlen(prefix)) != '\0')
#define MATCH_ARG(name) (strcmp(argv[i],name) == 0)
      
      if (MATCH_ARG("--help")) {
	usage(argv[0]);
	exit(0);
      }
      else if (MATCH_PREFIX("--buffer-size=",post)) {
	_conf._buffer_size = atol(post);
      }	      
      else if (MATCH_PREFIX("--buffers=",post)) {
	_conf._max_buffers = atol(post);
      }	      
      else if (MATCH_PREFIX("--event-size=",post)) {
	_conf._event_size = atol(post);
      }	      
      else if (MATCH_PREFIX("--subevent-size=",post)) {
	_conf._subevent_size = atol(post);
      }	      
      else if (MATCH_ARG("--random-size")) {
	_conf._random_size = 1;
      }	      
      else if (MATCH_ARG("--random-trig")) {
	_conf._random_trig = 1;
      }	      
      else if (MATCH_PREFIX("--titris-stamp=",post)) {
	_conf._titris_stamp = atol(post);
      }	      
      else if (MATCH_PREFIX("--wr-stamp=",post)) {
	_conf._wr_stamp = atol(post);
      }	      
      else if (MATCH_PREFIX("--bad-stamp=",post)) {
	_conf._bad_stamp = atol(post);
      }	      
      else if (MATCH_PREFIX("--caen-v775=",post)) {
      _conf._caen_v775 = atol(post);
      }	      
      else if (MATCH_PREFIX("--caen-v1290=",post)) {
      _conf._caen_v1290 = atol(post);
      }	      
      else if (MATCH_PREFIX("--events=",post)) {
	_conf._max_events = atol(post);
      }	      
      else if (MATCH_PREFIX("--rate=",post)) {
	_conf._max_rate = atol(post);
      }	      
      else if (MATCH_ARG("--empty-buffers")) {
	_conf._empty_buffers = 1;
      }	      
      else if (MATCH_ARG("--lmd")) {
	_conf._format = WRITE_FORMAT_LMD;
      }	      
      else if (MATCH_ARG("--ebye")) {
	_conf._format = WRITE_FORMAT_EBYE;
      }	      
      else if (MATCH_ARG("--pax")) {
	_conf._format = WRITE_FORMAT_PAX;
      }	      
      else if (MATCH_ARG("--hld")) {
	_conf._format = WRITE_FORMAT_HLD;
      }	      
      else {
	fprintf (stderr,"Unrecognized or invalid option: %s\n",argv[i]);
	exit(1);
      }
    }

  if (_conf._empty_buffers &&
      _conf._max_events)
    {
      fprintf (stderr,
	       "Cannot limit on number of events with empty buffers.\n");
      exit(1);
    }

  if (_conf._max_buffers == 0)
    _conf._max_buffers = (sint64) -1;
  if (_conf._max_events == 0)
    _conf._max_events = (sint64) -1;

     
  switch (_conf._format)
    {
    case WRITE_FORMAT_LMD:
      write_data_lmd();
      break;
    case WRITE_FORMAT_EBYE:
      write_data_ebye();
      break;
    case WRITE_FORMAT_PAX:
      write_data_pax();
      break;
    case WRITE_FORMAT_HLD:
      write_data_hld();
      break;
    default:
      fprintf (stderr,"Output data format not specified.\n");
      exit(1);
    }


  return(0);
}

char *_buffer = NULL;
char *_buffer_end = NULL;

void alloc_buffer()
{
  _buffer = (char*) malloc(_conf._buffer_size);
  
  if (!_buffer)
    {
      fprintf (stderr,
	       "Failed to allocate (%d bytes) buffer.\n",_conf._buffer_size);
      exit(0);
    }

  _buffer_end = _buffer + _conf._buffer_size;

  fprintf (stderr,"Allocated %d bytes buffer...\n",_conf._buffer_size);
}

size_t full_write(int fd,const void *buf,size_t count)
{
  size_t total = 0;
  
  for ( ; ; )
    {
      int nwrite = write(fd,buf,count);

      if (!nwrite)
	{
	  fprintf(stderr,"Reached unexpected end of file while writing.");
	  exit(1);
	}
 
      if (nwrite == -1)
	{
	  if (errno == EINTR)
	    nwrite = 0;
	  else
	    {
	      perror("write");
	      exit(1);
	    }
	}

      count -= nwrite;
      total += nwrite;

      if (!count)
	break;
      buf = ((const char*) buf)+nwrite;
    }

  return total;
}

void write_buffer()
{
  full_write(STDOUT_FILENO,_buffer,_conf._buffer_size);
  //fprintf (stderr,"Wrote buffer...\n");
}

void sleep_timeslot(struct timeval *timeslot_start)
{
  struct timeval now;
  
  gettimeofday(&now, NULL);

  timeslot_start->tv_sec++; /* Time of next timeslot start. */

  /* If we have already used more than a second, then just
   * say that the next timeslot starts now.
   */

  struct timeval remain;

  remain.tv_sec  = timeslot_start->tv_sec  - now.tv_sec;
  remain.tv_usec = timeslot_start->tv_usec - now.tv_usec;

  if (remain.tv_usec < 0)
    {
      remain.tv_sec--;
      remain.tv_usec += 1000000;
    }

  if (remain.tv_sec < 0 ||
      remain.tv_sec > 1 /* should not be, just reset */)
    {
      *timeslot_start = now;
      return;
    }
  
  usleep(remain.tv_usec);
}

char *create_titris_stamp(char *data_write,
			  uint64_t *rstate_badtitris)
{
  uint32_t *titris_stamp = (uint32_t *) data_write;

  struct timespec now;

  clock_gettime(CLOCK_REALTIME, &now);

  // Lets write the time-stamp in units of
  // 10 ns...  This is arbitrary.

  uint64_t stamp = (100000000 * now.tv_sec +
		    (now.tv_nsec / 10));

  if (_conf._bad_stamp &&
      (rxs64s(rstate_badtitris) % _conf._bad_stamp) == 0)
    {
      stamp -= 0x1000000;
      fprintf (stderr,
	       "Inject BAD: %016" PRIx64 "  \n", stamp);
    }

  titris_stamp[0] = _conf._titris_stamp << 8;
  titris_stamp[1] = TITRIS_STAMP_L16_ID |
    (uint32_t) ( stamp        & 0xffff);
  titris_stamp[2] = TITRIS_STAMP_M16_ID |
    (uint32_t) ((stamp >> 16) & 0xffff);
  titris_stamp[3] = TITRIS_STAMP_H16_ID |
    (uint32_t) ((stamp >> 32) & 0xffff);

  data_write += 4 * sizeof(uint32_t);

  return (char *) data_write;
}

char *create_wr_stamp(char *data_write,
		      uint64_t *rstate_badwr)
{
  uint32_t *wr_stamp = (uint32_t *) data_write;

  struct timespec now;

  clock_gettime(CLOCK_REALTIME, &now);

  // Lets write the time-stamp in units of
  // 10 ns...  This is arbitrary.

  uint64_t stamp = (100000000 * now.tv_sec +
		    (now.tv_nsec / 10));

  if (_conf._bad_stamp &&
      (rxs64s(rstate_badwr) % _conf._bad_stamp) == 0)
    {
      stamp -= 0x1000000;
      fprintf (stderr,
	       "Inject BAD: %016" PRIx64 "  \n", stamp);
    }

  wr_stamp[0] = _conf._wr_stamp << 8;
  wr_stamp[1] = WR_STAMP_DATA_0_16_ID |
    (uint32_t) ( stamp        & 0xffff);
  wr_stamp[2] = WR_STAMP_DATA_1_16_ID |
    (uint32_t) ((stamp >> 16) & 0xffff);
  wr_stamp[3] = WR_STAMP_DATA_2_16_ID |
    (uint32_t) ((stamp >> 32) & 0xffff);
  wr_stamp[4] = WR_STAMP_DATA_3_16_ID |
    (uint32_t) ((stamp >> 48) & 0xffff);

  data_write += 5 * sizeof(uint32_t);

  return (char *) data_write;
}

void write_data_lmd()
{
  uint64_t rstate_trig = 1;
  uint64_t rstate_evsize = 2;
  uint64_t rstate_sevsize = 3;
  uint64_t rstate_badtitris = 4;
  uint64_t rstate_badwr = 5;
  uint64_t rstate_sim_caen = 6;

  uint64_t timeslot_nev = 0;
  struct timeval timeslot_start;
  
  // round the buffer size up to next multiple of 1024 bytes.
  // default size is 32k

  if (!_conf._buffer_size)
    _conf._buffer_size = 0x8000;

  _conf._buffer_size = (_conf._buffer_size + 0x3ff) & ~0x3ff;

  alloc_buffer();

  gettimeofday(&timeslot_start,NULL);  

  for (uint64 nb = 0, nev = 0; (nb < _conf._max_buffers &&
		       nev < _conf._max_events); nb++)
    {
      memset(_buffer,0,_conf._buffer_size);

      s_bufhe_host *bufhe = (s_bufhe_host *) _buffer;

      bufhe->l_dlen = DLEN_FROM_BUFFER_SIZE(_conf._buffer_size);

      bufhe->l_free[0] = 0x00000001;

      struct timeval tv;

      gettimeofday(&tv,NULL);

      bufhe->l_time[0] = tv.tv_sec;
      bufhe->l_time[1] = tv.tv_usec / 1000;
      
      bufhe->i_type    = LMD_BUF_HEADER_10_1_TYPE;
      bufhe->i_subtype = LMD_BUF_HEADER_10_1_SUBTYPE;

      bufhe->l_buf = nb;

      char *data_start = (char*) (bufhe+1);
      char *data_end = data_start;

      if (!_conf._empty_buffers)
	{
	  // Write as many events as fits...

	  // If we are to dump titris stamps, we require each event
	  // to have at least one subevent.

	  size_t min_event_total_size = sizeof(lmd_event_10_1_host);

	  if (_conf._titris_stamp || _conf._wr_stamp ||
	      _conf._caen_v775 || _conf._caen_v1290)
	    min_event_total_size += sizeof(lmd_subevent_10_1_host);
	  if (_conf._titris_stamp)
	    min_event_total_size += 4 * sizeof(uint32_t);
	  if (_conf._wr_stamp)
	    min_event_total_size += 5 * sizeof(uint32_t);

	  if (_conf._caen_v775 || _conf._caen_v1290)
	    min_event_total_size += 2 * sizeof(uint32_t);

	  if (_conf._caen_v775)
	    min_event_total_size +=
	      _conf._caen_v775 * (2 + 32) * sizeof(uint32_t);
	  if (_conf._caen_v1290)
	    min_event_total_size +=
	      _conf._caen_v1290 * (3 + 32 * 32) * sizeof(uint32_t);

	  while (_buffer_end - data_end >= min_event_total_size &&
		 nev < _conf._max_events)
	    {
	      nev++;
	      timeslot_nev++;

	      lmd_event_10_1_host *ev = (lmd_event_10_1_host *) data_end;

	      ev->_header.l_dlen    = 
		DLEN_FROM_EVENT_DATA_LENGTH(sizeof(lmd_event_info_host));
	      ev->_header.i_type    = LMD_EVENT_10_1_TYPE;
	      ev->_header.i_subtype = LMD_EVENT_10_1_SUBTYPE;
	      ev->_info.i_trigger   = 
		_conf._random_trig ? rxs64s(&rstate_trig) % 15 + 1 : 1;
	      ev->_info.l_count     = nev;	      

	      bufhe->l_evt++;

	      data_end = (char*) (ev + 1);

	      size_t min_subevent_total_size = sizeof(lmd_subevent_10_1_host);

	      if (_conf._titris_stamp)
		min_subevent_total_size += 4 * sizeof(uint32_t);
	      if (_conf._wr_stamp)
		min_subevent_total_size += 5 * sizeof(uint32_t);

	      if (_conf._caen_v775 || _conf._caen_v1290)
		min_subevent_total_size += 2 * sizeof(uint32_t);

	      if (_conf._caen_v775)
		min_subevent_total_size +=
		  _conf._caen_v775 * (2 + 32) * sizeof(uint32_t);
	      if (_conf._caen_v1290)
		min_subevent_total_size +=
		  _conf._caen_v1290 * (3 + 32 * 32) * sizeof(uint32_t);

	      bool write_titris_stamp = !!_conf._titris_stamp;
	      bool write_wr_stamp = !!_conf._wr_stamp;
	      bool write_caen_vxxx = !!_conf._caen_v775 || !!_conf._caen_v1290;

	      uint event_size = _conf._event_size;
	      if (_conf._random_size && event_size)
		event_size = rxs64s(&rstate_evsize) % (event_size+1);

	      while ((write_titris_stamp ||
		      write_wr_stamp ||
		      write_caen_vxxx ||
		      data_end - (char*) ev < event_size) &&
		     _buffer_end - data_end >= min_subevent_total_size)
		{
		  lmd_subevent_10_1_host *sev = 
		    (lmd_subevent_10_1_host *) data_end;

		  uint data_len =
		    _buffer_end - data_end - sizeof(lmd_subevent_10_1_host);

		  uint subevent_size = _conf._subevent_size;
		  if (_conf._random_size && subevent_size)
		    subevent_size = rxs64s(&rstate_sevsize) % (subevent_size+1);

		  if (data_len > subevent_size)
		    data_len = subevent_size;

		  size_t min_subevent_data_size =
		    min_subevent_total_size - sizeof(lmd_subevent_10_1_host);

		  if (data_len < min_subevent_data_size)
		    data_len = min_subevent_data_size;

		  data_len &= ~0x03; // align to 32-bit words
		  
		  sev->_header.i_type    = 1234; // dummy (I refuse 10/1: nuts)
		  sev->_header.i_subtype = 5678;
		  sev->h_control  = 0;
		  sev->h_subcrate = 0;
		  sev->i_procid   = 0;

		  char *data_start = (char*) (sev + 1);
		  char *data_write = data_start;
		  char *data_cut = NULL;

		  if (write_titris_stamp)
		    {
		      data_write = create_titris_stamp(data_write,
						       &rstate_badtitris);
		      write_titris_stamp = false;
		    }
		  if (write_wr_stamp)
		    {
		      data_write = create_wr_stamp(data_write,
						   &rstate_badwr);
		      write_wr_stamp = false;
		    }
		  if (write_caen_vxxx)
		    {
		      data_end = data_write;

		      sev->_header.i_type    = 0x0cae;
		      sev->_header.i_subtype = 0x0cae;

		      uint32 seed = (uint32) rxs64s(&rstate_sim_caen);

		      uint32_t *p = (uint32_t *) data_write;

		      *(p++) = seed;

		      data_write = (char *) p;

		      for (int geom = 1; geom <= _conf._caen_v775 &&
			     geom < 32; geom++)
			{
			  int crate = 0x80 - geom; // for fun!

			  caen_v775_data cev;

			  create_caen_v775_event(&cev, geom, crate,
						 nev + 0xdef, seed);

			  data_write = (char *)
			    create_caen_v775_data((uint32 *) data_write,
						  &cev, geom, crate);
			}

		      p = (uint32_t *) data_write;

		      *(p++) = 0;

		      data_write = (char *) p;

		      for (int geom = 1; geom <= _conf._caen_v1290 &&
			     geom < 32; geom++)
			{
			  caen_v1290_data cev;

			  create_caen_v1290_event(&cev, geom,
						  nev + 0xdef, seed);

			  data_write = (char *)
			    create_caen_v1290_data((uint32 *) data_write,
						  &cev, geom);
			}

		      data_cut = data_write;

		      write_caen_vxxx = false;
		    }

		  if (data_cut)
		    {
		      data_len = data_cut - data_start;
		    }

		  sev->_header.l_dlen    = 
		    SUBEVENT_DLEN_FROM_DATA_LENGTH(data_len);
		  ev->_header.l_dlen +=
		    (sizeof(lmd_subevent_10_1_host) + data_len) / 2;

		  data_end = data_start + data_len;

		  min_subevent_total_size = sizeof(lmd_subevent_10_1_host);
		}

	      if (_conf._max_rate &&
		  timeslot_nev >= _conf._max_rate)
		{
		  timeslot_nev = 0;
		  break;
		}
	    }
	}

      bufhe->l_free[2] = IUSED_FROM_BUFFER_USED(data_end - data_start);
      
      if (bufhe->l_dlen <= LMD_BUF_HEADER_MAX_IUSED_DLEN)
	bufhe->i_used = bufhe->l_free[2];

      write_buffer();

      if (!timeslot_nev)
	sleep_timeslot(&timeslot_start);
    }
}

void write_data_ebye()
{
  // round the buffer size up to next multiple of 4096 bytes.
  // default size is 32k

  if (!_conf._buffer_size)
    _conf._buffer_size = 0x8000;

  _conf._buffer_size = (_conf._buffer_size + 0xfff) & ~0xfff;

  alloc_buffer();

  for (uint64 nb = 0, nev = 0; (nb < _conf._max_buffers &&
		       nev < _conf._max_events); nb++)
    {
      memset(_buffer,0,_conf._buffer_size);

      ebye_record_header *rh = (ebye_record_header *) _buffer;

      memcpy(rh->_id,EBYE_RECORD_ID,8);
      
      rh->_sequence = nb;
      rh->_tape     = 1;
      rh->_stream   = 1;
      rh->_endian_tape = 0x0001;
      rh->_endian_data = htons(0x0001);

      char *data_start = (char*) (rh+1);
      char *data_end = data_start;

      if (!_conf._empty_buffers)
	{
	  while (_buffer_end - data_end >= 2*sizeof(ebye_event_header) &&
		 nev < _conf._max_events)
	    {
	      nev++;

	      ebye_event_header *eh = (ebye_event_header *) data_end;

	      eh->_start_end_token_length = 
		htonl(0xffff0000 | sizeof(ebye_event_header));

	      data_end = (char*) (eh + 1);
	    }
	}

      // write an end token
      
      ebye_event_header *eh = (ebye_event_header *) data_end;
      
      eh->_start_end_token_length = htonl(0xffff0000);
      
      data_end = (char*) (eh + 1);

      rh->_data_length = data_end - data_start;

      write_buffer();
    }
}

void write_data_pax()
{
  if (!_conf._buffer_size)
    _conf._buffer_size = 0x4000;
  if (_conf._buffer_size > PAX_RECORD_MAX_LENGTH)
    _conf._buffer_size = PAX_RECORD_MAX_LENGTH;

  _conf._buffer_size = (_conf._buffer_size + 0x01) & ~0x01;

  alloc_buffer();

  for (uint64 nb = 0, nev = 0; (nb < _conf._max_buffers &&
		       nev < _conf._max_events); nb++)
    {
      memset(_buffer,0,_conf._buffer_size);

      pax_record_header *rh = (pax_record_header *) _buffer;

      rh->_length      = _conf._buffer_size;
      rh->_type        = PAX_RECORD_TYPE_STD;
      rh->_seqno       = nb+1;
      rh->_off_nonfrag = sizeof(pax_record_header);

      char *data_start = (char*) (rh+1);
      char *data_end = data_start;

      if (!_conf._empty_buffers)
	{
	  while (_buffer_end - data_end >= 2*sizeof(pax_event_header) &&
		 nev < _conf._max_events)
	    {
	      nev++;

	      pax_event_header *eh = (pax_event_header *) data_end;

	      eh->_length = sizeof (pax_event_header);
	      eh->_type   = PAX_EVENT_TYPE_NORMAL_MAX;

	      data_end = (char*) (eh + 1);
	    }
	}

      // write an end token
      
      pax_event_header *eh = (pax_event_header *) data_end;
      
      eh->_length = 0;
      eh->_type   = 0;
      
      data_end = (char*) (eh + 1);

      write_buffer();
    }
}

void write_data_hld()
{
  _conf._buffer_size = sizeof(hld_event_header);

  alloc_buffer();

  for (uint64 nb = 0, nev = 0; (nb < _conf._max_buffers &&
		       nev < _conf._max_events); nb++)
    {
      memset(_buffer,0,_conf._buffer_size);

      hld_event_header *eh = (hld_event_header *) _buffer;

      eh->_size            = sizeof(hld_event_header);
      eh->_decoding._align = 3; // 64 bit alignment
      eh->_decoding._type  = 1; // type???  (non-0)
      eh->_id              = 1; // physics :-)
      eh->_seq_no          = nev++;
      eh->_file_no         = 1;

      // Hmm, whoever is crazy enough to store dates and times with
      // individual fields, _in binary_, ...

      eh->_date            = 0;
      eh->_time            = 0;

      write_buffer();
    }
}
