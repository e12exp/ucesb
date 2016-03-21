#include "encode.hh"
#include "dump_list.hh"

void encode_spec::dump(dumper &d) const
{
  d.text("ENCODE(");
  _name->dump(d);
  if (_flags & ES_APPEND_LIST)
    d.text(" APPEND_LIST");
  d.text(",");
  dump_list_paren(_args,d,"()");
  d.text(");");
}
