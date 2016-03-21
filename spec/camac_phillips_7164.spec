// -*- C++ -*-

#define CAMAC_PHILLIPS_7164H  CAMAC_PHILLIPS_7164
#define CAMAC_PHILLIPS_7186H  CAMAC_PHILLIPS_7164

CAMAC_PHILLIPS_7164(channels,mark_channel_no)
{
  MEMBER(DATA12 data[16] ZERO_SUPPRESS);

  list(0<=index<channels)
    {
      if (mark_channel_no) {
	UINT16 ch_data NOENCODE
	  {
	    0_11:  value;
	    12_15: channel = CHECK(index);

	    ENCODE(data[index],(value=value));
	  }
      } else {
	UINT16 ch_data NOENCODE
	  {
	    0_11:  value;

	    ENCODE(data[index],(value=value));
	  }
      }
    }
}
