
#ifndef __EXT_DATA_CLNT_HH__
#define __EXT_DATA_CLNT_HH__

#ifndef __CINT__
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#endif

class ext_data_struct_info;

class ext_data_clnt
{
public:
  ext_data_clnt();
  ~ext_data_clnt();

public:
  bool connect(const char *server);
  bool connect(const char *server,int port);
  bool connect(int fd);

  int setup(const void *struct_layout_info,size_t size_info,
	    ext_data_struct_info *struct_info,
	    size_t size_buf);

  int fetch_event(void *buf,size_t size);
  int get_raw_data(const void **raw, ssize_t *raw_words);
  const char *last_error();

  int close();

  static void rand_fill(void *buf,size_t size);

protected:
  void *_client; // void such that cint does not care about the type

};

#endif//__EXT_DATA_CLNT_HH__
