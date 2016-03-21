#ifndef __ONLINE_CHAIN_HH__
#define __ONLINE_CHAIN_HH__

#include "TChain.h"

class online_chain
{
public:

public:
  static TChain *since(const char *basename,int seconds);
  static TChain *span(const char *basename,const char *from,const char *to);
};

#endif//__ONLINE_CHAIN_HH__
