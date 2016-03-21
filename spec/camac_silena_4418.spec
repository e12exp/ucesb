// -*- C++ -*-

#define CAMAC_SILENA_4418T  CAMAC_SILENA_4418
#define CAMAC_SILENA_4418Q  CAMAC_SILENA_4418
#define CAMAC_SILENA_4418A  CAMAC_SILENA_4418


CAMAC_SILENA_4418(channels,mark_channel_no)
{
  MEMBER(DATA12_OVERFLOW data[8] ZERO_SUPPRESS);

  list(0<=index<channels)
    {
      if (mark_channel_no) {
	UINT16 ch_data NOENCODE 
	  {
	    0_11:  value;
	    12_14: channel = CHECK(index);
	    15:    overflow;

	    ENCODE(data[index],(value=value,overflow=overflow)); 
	  }
      } else {
	UINT16 ch_data NOENCODE 
	  {
	    0_11:  value;
	    15:    overflow;

	    ENCODE(data[index],(value=value,overflow=overflow)); 
	  }
      }
    }
}



