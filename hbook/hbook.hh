
#ifndef __HBOOK_H__
#define __HBOOK_H__



class hbook
{
public:
  hbook();

public:
  char _chtop[64]; // TODO: what is actual max length??? (secure code against buffer overflow!)
  bool _open;

public:
  void open(const char *filename,const char* top,
	    int record_length = 8191);
  void close();

};

class hbook_ntuple
{
public:
  int _hid;

public:
  void hbookn(int hid,
	      const char *title,
	      const char *rzpa,
	      char tags[][9],
	      int entries);
  void hfn(float *array);

};

class hbook_ntuple_cwn
{
public:
  int _hid;

public:
  void hbset_bsize(int bsize);
  void hbnt(int hid,
	    const char *title,
	    const char *opt);
  void hbname(const char *block,
	      void *structure,
	      const char *vars);    
  void hfnt();

  /*
  void hbookn(int hid,
	      const char *title,
	      const char *rzpa,
	      char tags[][9],
	      int entries);
  void hfn(float *array);
  */

};

class hbook_histogram
{
public:
  int _hid;

public:
  void hbook1(int hid,
	      const char *content,
	      int bins,
	      float min,
	      float max);
  void hfill1(float x,float w);
  void hrout();

};


#endif// __HBOOK_H__
