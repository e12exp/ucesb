// -*- C++ -*-

// Definitions for EBYE format with 32 bit data entities

// Specification for EBYE data items, as used by the Daresbury MIDAS

#include "spec/midas32_caen.spec"

SIMPLE_DATA(group)
{
  MEMBER(DATA16 data[64] ZERO_SUPPRESS);

  UINT32 entry NOENCODE
    {
      0_15:  value;
      // 16_29: address;
      16_23: group = MATCH(group);
      24_29: item;
      30_31: 0b00;

      ENCODE(data[item],(value=value));
    }
}
