
#include "lmd_event.hh"

#include "util.hh"

#include <string.h>

///////////////////////////////////////////////////////
// swap between little and big endian formats

void byteswap_32(s_bufhe_host& buf_header)
{
  // byte swap members of s_bufhe struct

  byteswap ((uint32*) &buf_header,sizeof (buf_header));
}

#define mymin(a,b) ((a)<(b)?(a):(b))

void byteswap_32(s_filhe_extra_host& file_extra)
{
  // byte swap members of s_filhe struct

  bswap_16(file_extra.filhe_label.len);    
  bswap_16(file_extra.filhe_file.len);     
  bswap_16(file_extra.filhe_user.len);     
  bswap_16(file_extra.filhe_run.len);      
  bswap_16(file_extra.filhe_exp.len);      
  bswap_32(file_extra.filhe_lines);

  // But only as many as are used.
  
  for (uint i = 0; i < mymin(countof(file_extra.s_strings),
			     file_extra.filhe_lines); i++)
    bswap_16(file_extra.s_strings[i].len);
}

template<int maxlen>
void zero_padding(s_l_string<maxlen> &l_str)
{
  if (l_str.len > maxlen)
    l_str.len = maxlen; // normalize

  if (l_str.len < maxlen)
    memset(l_str.string+l_str.len,0,maxlen-l_str.len);
}

void zero_padding(s_filhe_extra_host &file_extra)
{
  zero_padding(file_extra.filhe_label);    
  zero_padding(file_extra.filhe_file);     
  zero_padding(file_extra.filhe_user);     
  zero_padding(file_extra.filhe_run);      
  zero_padding(file_extra.filhe_exp);

  if (file_extra.filhe_lines > countof(file_extra.s_strings))
    file_extra.filhe_lines = (uint32) countof(file_extra.s_strings);
  
  for (uint i = 0; i < countof(file_extra.s_strings); i++)
    zero_padding(file_extra.s_strings[i]);

  for (uint i = file_extra.filhe_lines; i < countof(file_extra.s_strings); i++)
    memset(&file_extra.s_strings[i],0,sizeof(file_extra.s_strings[i]));
}
