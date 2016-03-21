// -*- C++ -*-

FASTBUS_LECROY_1875(geom,channels)
{
  MEMBER(DATA12_RANGE data[64] ZERO_SUPPRESS);

  UINT32 ch_data NOENCODE
    {
      0_11:  value;
      // 12_15: 0;
      16_21: channel = RANGE(0,channels-1);
      // 22:    0;
      23:    range;
      24_26: n = 0;
      27_31: geom = MATCH(geom);

      ENCODE(data[channel],(value=value,range=range));
    }
}

FASTBUS_LECROY_1885(geom,channels)
{
  MEMBER(DATA12_RANGE data[96] ZERO_SUPPRESS);

  UINT32 ch_data NOENCODE
    {
      0_11:  value;
      // 12_15: 0;
      16_22: channel = RANGE(0,channels-1);
      23:    range;
      24_26: n = 0;
      27_31: geom = MATCH(geom);

      ENCODE(data[channel],(value=value,range=range));
    }
}
#if 0
FASTBUS_DATA(geom)
{
  select several
    {
      tdc0 = FASTBUS_LECROY_1875(geom=4,channels=64);
      tdc1 = FASTBUS_LECROY_1875(geom=5,channels=64);
      tdc2 = FASTBUS_LECROY_1875(geom=6,channels=64);
      qdc0 = FASTBUS_LECROY_1885(geom=7,channels=96);
      qdc1 = FASTBUS_LECROY_1885(geom=8,channels=96);
      // qdc1 = FASTBUS_LECROY_1895(geom=8,channels=96);
    }
}
#endif

