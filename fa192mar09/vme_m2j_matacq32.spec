// -*- C++ -*-

VME_M2J_MATACQ32(mod,ch)
{
  MEMBER(DATA16 data[2560] NO_INDEX_LIST);

  UINT32 header
    {
      0_15:  data_length = RANGE(0,(2560+4)/2);
      16_19: ch = MATCH(ch);
      20_23: mod = MATCH(mod);
      24_31: id = 0xdf;
    }

  UINT16 trig_rec;
  UINT16 first_sample;
  UINT16 vernier;
  UINT16 reset_baseline;

  list(0<=index<(static_cast<uint32>(header.data_length)-2)*2)
    {
      UINT16 value;
      
      ENCODE(data APPEND_LIST,(value=value));
    }
}

external EXTERNAL_DATA32(length);

VME_M2J_MATACQ32_PACK_EXT(mod,ch)
{
  UINT32 header
    {
      0_15:  data_length = RANGE(0,(2560+4)/2);
      16_19: ch = MATCH(ch);
      20_23: mod = MATCH(mod);
      24_31: id = 0xde;
    }

  UINT16 trig_rec;
  UINT16 first_sample;
  UINT16 vernier;
  UINT16 reset_baseline;

  external data32 = 
    EXTERNAL_DATA32(length=(static_cast<uint32>(header.data_length)-2));
}
