// -*- C++ -*-

VME_MESYTEC_MADC32(geom)
{
  MEMBER(DATA12_OVERFLOW data[32] ZERO_SUPPRESS);

  UINT32 header NOENCODE
  {
    0_11:  word_number; // includes end_of_event
    12_14: adc_resol;
    15:	   out_form;
    16_23: geom = MATCH(geom);
    24_29: 0b000000;
    30_31: 0b01;
  }

  list(0 <= index < static_cast<uint32>(header.word_number - 1))
  {
    UINT32 ch_data NOENCODE
    {
      0_11:  value;
      14:    outofrange;
      16_20: channel;
      21_29: 0b000100000;
      30_31: 0b00;

      ENCODE(data[channel], (value = value, overflow = outofrange));
    }
  }

  UINT32 end_of_event NOENCODE
  {
    0_29:  counter;
    30_31: 0b11;
  }
}
