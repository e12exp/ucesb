





uint32 *
compact(const uint32 *src,
	const uint32 *base,
	int n,
	uint32 *dest)
{


  // We store all data.  First we need compute an optimal offset to add
  // to the base.  For storing 12 bit data values, which hopefully
  // usually stays within 16 bits of the offset, we'll use an approch
  // with three 'streams' of data.

  // The first one is a bit pattern stream which selects if an value is
  // stored only with 4 bits, or the full 4+8=12 bits are needed

  // By using a separate bit stream for the selection of storage method,
  // the unpacker has that information available well in advance

  int free_bits = 32;
  int used = 0;
  uint32* p_bucket = dest;
  uint bucket = 0;

  for (i = n; i; i -= 32)
    {
      uint32* pmask = dest++;
      uint32 mask = 0;
      uint32 bit = 0;

      for (int j = 32; j; --j)
	{
	  uint32 value = (*(src++) - *(base) - offset) & 0xfff; // only 12 bits
	  
	  if (value < 16)
	    {
	      bits = 12;	      
	    }
	  else
	    {
	      mask |= bit;
	      bits = 4;	      
	    } 

	  bit <<= 1;

	  // Then store the value

	  bucket |= value << used; // used must not be >= 32
	  free_bits -= bits;
	  used += bits;

	  if (free_bits <= 0) // we do <= such that used never becomes >= 32
	    {
	      *p_bucket = bucket;
	      p_bucket = dest++;

	      free_bits += 32;
	      used -= 32;
	      bucket = value >> (used - bits);
	    }
	}
      *pmask = mask;
    }

  *p_bucket = bucket;

  // p_bucket was the last added item, so if we were not needed, then do not store...

  if (!used)
    dest++;
}





















