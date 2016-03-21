// -*- C++ -*-

// Definitions for EBYE format with 16 bit data entities

// Specification for EBYE data items, as used by the Daresbury MIDAS

#include "spec/midas_caen.spec"

SIMPLE_DATA(group)
{
  MEMBER(DATA16 data[64] ZERO_SUPPRESS);

  UINT16 entry NOENCODE
    {
      // 0_13: address;
      0_7:   group = MATCH(group);
      8_13:  item;
      14_15: 0b00;
    }

  UINT16 value NOENCODE;

  ENCODE(data[entry.item],(value=value));
}

GROUP_DATA(group)
{
  MEMBER(DATA16 data[64] NO_INDEX_LIST);

  UINT16 header NOENCODE
    {
      // 0_13: address;
      0_7:   group = MATCH(group);
      8_13:  item_count;
      14_15: 0b01;
    }

  list (0 <= index < header.item_count)
    {
      UINT16 value NOENCODE;

      ENCODE(data APPEND_LIST,(value=value));      
    }

  if (!(header.item_count & 1))
    {
      // even humber of data words, padding needed to keep 32-bit alignment

      UINT16 pad NOENCODE
	{
	  0_15: 0;
	}
    }
}

EXTENDED_GROUP_DATA(group)
{
  // This is really a monster array, and most likely not needed.
  // This is anyhow only an example, and should not really be used for production...
  MEMBER(DATA16 data[0x4000] NO_INDEX_LIST); // 64 * 256

  UINT16 header NOENCODE
    {
      // 0_13: address;
      0_13:  item_count;
      14_15: 0b10;
    }

  // We've anyhow run into a major problem...  Since the match item is
  // in the _second_ data word, ucesb cannot generate proper matching
  // code for it, as the matcher stops at the first word...  This
  // would need to be fixed...

  UINT16 grp NOENCODE
    {
      0_15:   group = MATCH(group);
    }

  MATCH_END;

  list (0 <= index < header.item_count)
    {
      UINT16 value NOENCODE;

      ENCODE(data APPEND_LIST,(value=value));      
    }

  if (header.item_count & 1)
    {
      // even humber of data words, padding needed to keep 32-bit alignment

      UINT16 pad NOENCODE
	{
	  0_15: 0;
	}
    }
}
