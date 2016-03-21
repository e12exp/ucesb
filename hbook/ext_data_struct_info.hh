
#ifndef __EXT_DATA_STRUCT_INFO_HH__
#define __EXT_DATA_STRUCT_INFO_HH__

class ext_data_clnt;
class ext_data_clnt_stderr;

struct ext_data_structure_info;

class ext_data_struct_info
{
public:
  ext_data_struct_info();
  ~ext_data_struct_info();

protected:
  void *_info; // void such that cint does not care about the type

  friend class ext_data_clnt;
  friend class ext_data_clnt_stderr;

public:
  // Giving this away is necessary due to the EXT_STR_ITEM_... macros.
  // Thos macros in turn benefit from being macros (and not functions),
  // as they wrap some use of offsetof and sizeof.
  operator ext_data_structure_info*();

};

#endif//__EXT_DATA_STRUCT_HH__
