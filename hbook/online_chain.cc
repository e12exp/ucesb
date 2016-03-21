#include "online_chain.hh"

#if 0
void online_chain::select_files()
{
  // There were two choices, either let this function find the files
  // by traversing a directory structure, or to let it find the files
  // by consulting a file list (current option).
  
  // Traversing the directory structure would remove the need for a
  // file list, but would also likely be a bit slower when many files
  // come into play (perhaps not so important).

  // It is however very important to create intermediate summary files
  // that span stretches of the original small files, as the
  // performance of TChain for a large number of small files is rather
  // bad.  On a 2 GHz opteron, it seems to have an additional constant
  // of handling of about 35 files/s in addition to the actual
  // processing.  So the additional small program XXX should be run to
  // make combination files then usable by us.  (We will whenever
  // possible select the combined files).  Fortunately, merging files
  // is very easy with the CopyTree function.



}
#endif

TChain *online_chain::since(const char *basename,int seconds)
{
  
  return NULL;
}

TChain *online_chain::span(const char *basename,
			   const char *from,const char *to)
{
  




  
  return NULL;
}
