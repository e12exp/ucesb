
#include "ext_data_struct_info.hh"
#include "ext_data_clnt.hh"
#include "ext_data_clnt_stderr.hh"
#include "ext_data_client.h"

#include <stdio.h>
#include <new>

/* */

ext_data_struct_info::ext_data_struct_info()
{
  _info = ext_data_struct_info_alloc();
  if (!_info)
    throw std::bad_alloc();
}

ext_data_struct_info::~ext_data_struct_info()
{
  struct ext_data_structure_info *struct_info =
    (struct ext_data_structure_info *) _info;

  ext_data_struct_info_free(struct_info);
}

ext_data_struct_info::operator ext_data_structure_info*()
{
  return (struct ext_data_structure_info *) _info;
}

/* */

ext_data_clnt::ext_data_clnt() { _client = NULL; }
ext_data_clnt::~ext_data_clnt() { if (_client) { close(); } }

bool ext_data_clnt::connect(const char *server)
{
  _client = ext_data_connect(server);
  return _client != NULL;
}

bool ext_data_clnt::connect(const char *server,int port)
{
  char tmp[1024];
  sprintf(tmp,"%s:%d",server,port);
  _client = ext_data_connect(tmp);
  return _client != NULL;
}

bool ext_data_clnt::connect(int fd)
{
  _client = ext_data_from_fd(fd);
  return _client != NULL;
}

int ext_data_clnt::setup(const void *struct_layout_info,
			 size_t size_info,
			 ext_data_struct_info *struct_info,
			 size_t size_buf)
{
  struct ext_data_structure_info *si =
    struct_info ?
    (struct ext_data_structure_info *) struct_info->_info : NULL;

  return ext_data_setup((ext_data_client *) _client,
			struct_layout_info,size_info,si,size_buf);
}

int ext_data_clnt::fetch_event(void *buf,size_t size)
{
  return ext_data_fetch_event((ext_data_client *) _client,buf,size);
}

int ext_data_clnt::get_raw_data(const void **raw, ssize_t *raw_words)
{
  return ext_data_get_raw_data((ext_data_client *) _client,
			       raw, raw_words);
}

int ext_data_clnt::close()
{
  int ret = ext_data_close((ext_data_client *) _client);
  _client = NULL;
  return ret;
}

void ext_data_clnt::rand_fill(void *buf,size_t size)
{
  ext_data_rand_fill(buf,size);
}

const char *ext_data_clnt::last_error()
{
	return ext_data_last_error((ext_data_client *) _client);
}

/* */

ext_data_clnt_stderr::ext_data_clnt_stderr() { _client = NULL; }
ext_data_clnt_stderr::~ext_data_clnt_stderr() { if (_client) { close(); } }

bool ext_data_clnt_stderr::connect(const char *server)
{
  _client = ext_data_connect_stderr(server);
  return _client != NULL;
}

bool ext_data_clnt_stderr::connect(const char *server,int port)
{
  char tmp[1024];
  sprintf(tmp,"%s:%d",server,port);
  _client = ext_data_connect_stderr(tmp);
  return _client != NULL;
}

int ext_data_clnt_stderr::setup(const void *struct_layout_info,
				size_t size_info,
				ext_data_struct_info *struct_info,
				size_t size_buf)
{
  struct ext_data_structure_info *si =
    struct_info ?
    (struct ext_data_structure_info *) struct_info->_info : NULL;

  return ext_data_setup_stderr((ext_data_client *) _client,
			       struct_layout_info,size_info,si,size_buf);
}

int ext_data_clnt_stderr::fetch_event(void *buf,size_t size)
{
  return ext_data_fetch_event_stderr((ext_data_client *) _client,buf,size);
}

int ext_data_clnt_stderr::get_raw_data(const void **raw, ssize_t *raw_words)
{
  return ext_data_get_raw_data_stderr((ext_data_client *) _client,
				      raw, raw_words);
}

void ext_data_clnt_stderr::close()
{
  ext_data_close_stderr((ext_data_client *) _client);
  _client = NULL;
}

void ext_data_clnt_stderr::rand_fill(void *buf,size_t size)
{
  ext_data_rand_fill(buf,size);
}

/* */

#if 0
void dummy_test_get_ptr()
{
  ext_data_struct_info wrapped;
  ext_data_structure_info *ptr;

  ptr = wrapped;  
}
#endif
