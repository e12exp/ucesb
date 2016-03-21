
#ifndef __EXT_DATA_CLNT_STDERR_HH__
#define __EXT_DATA_CLNT_STDERR_HH__

#ifndef __CINT__
#include <stdint.h>
#include <stdlib.h>
#endif

class ext_data_struct_info;

class ext_data_clnt_stderr
{
public:
  ext_data_clnt_stderr();
  ~ext_data_clnt_stderr();

public:
  bool connect(const char *server);
  bool connect(const char *server,int port);
  // bool connect(int fd);

  int setup(const void *struct_layout_info,size_t size_info,
	    ext_data_struct_info *struct_info,
	    size_t size_buf);

  int fetch_event(void *buf,size_t size);
  int get_raw_data(const void **raw, ssize_t *raw_words);

  void close();

  static void rand_fill(void *buf,size_t size);

protected:
  void *_client; // void such that cint does not care about the type

};

#endif//__EXT_DATA_CLNT_STDERR_HH__
