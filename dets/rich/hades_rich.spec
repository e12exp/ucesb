// -*- C++ -*-

external EXT_HADES_RICH();

SUBEVENT(RICH_SUBEV)
{
  // It does not make much sense to subdivide all these into their own
  // data structures (one for each 64 channels)...  Use an external
  // unpacker.

  external data = EXT_HADES_RICH();

  /*
  UINT32 data NOENCODE
    {
      0_9:   value;
      10_15: channel;
      16_18: module;
      19_21: port;
      22:    controller;
      23_25: sector;
    };
  */
}

