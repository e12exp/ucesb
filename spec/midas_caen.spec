// -*- C++ -*-

// Definitions for EBYE format with 16 bit data entities

MIDAS_CAEN_V785(group)
{
  MEMBER(DATA12 data[32] ZERO_SUPPRESS);

  UINT16 entry NOENCODE
    {
      // 0_13: address;
      0_7:   group = MATCH(group);
      8_13:  item;
      14_15: 0b00;
    }

  UINT16 value NOENCODE
    {
      0_11:  value;
      12_15: 0;
    }

  ENCODE(data[entry.item],(value=value.value));
}

MIDAS_CAEN_V1190(group)
{
  MEMBER(DATA16 data[128] ZERO_SUPPRESS);

  UINT16 entry NOENCODE
    {
      // 0_13: address;
      0_7:   grp = RANGE(group,group+1);
      8_13:  item;
      14_15: 0b00;
    }

  UINT16 value NOENCODE;

  ENCODE(data[entry.item + ((entry.grp - group) << 6)],(value=value));
}

MIDAS_CAEN_V830(group)
{
  MEMBER(DATA32 data[32] ZERO_SUPPRESS);

  UINT16 entry1 NOENCODE
    {
      // 0_13: address;
      0_7:   group = MATCH(group);
      8:     item_low = 0;
      9_13:  item;
      14_15: 0b00;
    }

  UINT16 value1 NOENCODE;

  UINT16 entry2 NOENCODE
    {
      // 0_13: address;
      0_7:   group = MATCH(group);
      8:     item_low = 1;
      // With |1, it becomes necessary for it to be one above (or the same (bad))
      // With +1, it becomes necessary for it to be the next (can start odd (bad))
      9_13:  item = CHECK(entry1.item);
      14_15: 0b00;
    }

  UINT16 value2 NOENCODE;

  ENCODE(data[entry1.item],(value=(static_cast<uint32>(value1)) | (static_cast<uint32>(value2) << 16)));
}

