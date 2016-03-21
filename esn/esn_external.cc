
#include "esn_external.hh"
#include "error.hh"

#include "external_data.hh"
/*
#define GET_BUFFER_UINT16(dest) {                         \
  if (!__buffer.get_uint16(&dest)) {                      \
    ERROR("Error while reading " #dest " from buffer.");  \
  }                                                       \
}
*/
/*---------------------------------------------------------------------------*/

void EXT_FIFO::__clean()
{
  header = 0;
  event_counter = 0;
}

EXT_DECL_DATA_SRC_FCN(void,EXT_FIFO::__unpack_header)
{
  uint16 dummy1;
  uint16 dummy2;

  GET_BUFFER_UINT16(dummy1);
  GET_BUFFER_UINT16(header);
  GET_BUFFER_UINT16(dummy2);

  // int fifo_wc = header & 0x1fff;

  GET_BUFFER_UINT16(event_counter);

  //printf ("%04x %04x  %04x %04x\n",dummy1,header,dummy2,event_counter);
  //fflush(stdout);
}
EXT_FORCE_IMPL_DATA_SRC_FCN(void,EXT_FIFO::__unpack_header);

EXT_DECL_DATA_SRC_FCN(void,EXT_FIFO::__unpack_footer)
{
  uint16 dummy;
  
  GET_BUFFER_UINT16(dummy);

  uint16 align;

  if (header & 1)
    GET_BUFFER_UINT16(align);

  // printf ("Fifo dummy: %04x\n",dummy);
}
EXT_FORCE_IMPL_DATA_SRC_FCN(void,EXT_FIFO::__unpack_footer);

/*---------------------------------------------------------------------------*/

void EXT_SCI::__clean()
{
  fifo.__clean();
}

EXT_DECL_DATA_SRC_FCN(void,EXT_SCI::__unpack)
{
  fifo.__unpack_header(__buffer);

  // printf ("sci wc: %d",fifo.get_wc());
  // printf ("sci: ");

  for (int i = fifo.get_wc(); i; --i)
    {
      uint16 data;

      GET_BUFFER_UINT16(data);
      
      // printf (" %04x%c",data,data & 0x8000 ? '*' : ' ');
      // printf ("%2d , %1d , %4d\n",data >> 10,(data >> 9) & 1,data & 0x1ff);
    }
  // printf ("\n");

  fifo.__unpack_footer(__buffer);
}
EXT_FORCE_IMPL_DATA_SRC_FCN(void,EXT_SCI::__unpack);

/*---------------------------------------------------------------------------*/
