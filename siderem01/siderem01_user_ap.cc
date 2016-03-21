#if 0
#include "auto_pedestal_adjust.hh"

#include <stdarg.h>
#include <stdio.h>

void C_F_ERROR(INTS4 errnum,INTU4 mask,const char *str)
{
  fprintf(stderr,"AUTO: %s\n",str);
}

void C_F_ERROR_FMT(INTS4 errnum,INTU4 mask,const char *str,...)
{
  va_list ap;

  char message[1024];

  va_start(ap, str);

  // vsnprintf(message,sizeof(message),str,ap);
  vsprintf(message,str,ap); // This is insecure (unsafe), we would prefere vsnprintf

  va_end(ap);

  fprintf(stderr,"AUTO: %s\n",message);

}

#define NUM_CHANNELS 1024
#define NUM_SST_DETECTORS 4

auto_pedestal_adjust_channel apac[4][1024];

int did_setup = 0;

int apac_ch(char *str,size_t size,apfc_info info)
{
  int channel = info._data;

  // return snprintf(str,size,"c%ds%d/%d%s",crate,slot,channel,range);
  return sprintf(str,"%d",channel); // This is insecure (unsafe), we would prefere vsnprintf
}

void setup()
{
  for (int det=0; det<NUM_SST_DETECTORS; det++)
    {
      for (int i = 0; i < NUM_CHANNELS; i++)
	{
	  apac[det][i]._control._max_expect_ped_std = 5.0;
	  apac[det][i]._control._ch_no_fmt  = apac_ch;
	  apac[det][i]._control._info._data = i;
	}
    }
  did_setup = 1;
}

int count[2]={0};

void user_pedestals(int det_number, int raw[NUM_CHANNELS])
{
  if (!did_setup)
    setup();

  for (int i = 0; i < NUM_CHANNELS; i++)
    apac[det_number][i].collect_data(raw[i]);

  // apac.analyse();

  count[det_number]++;
  //  printf("detector = %dcount = %d\n\n",det_number, count[det_number]);
  //uglystuff for the december test

  if ((count[det_number] & (NUM_PEDESTAL_DATA-1)) == 0)
    {
      FILE* ped;
      ped = fopen("sidped.txt","a");
      FILE* sig;
      sig = fopen("sidsig.txt","a");
      
      //ugly stuff for the december test. 
      fprintf (ped,"30%1d20000\n",det_number);
      fprintf (sig,"30%1d20000\n",det_number);
      
      for (int i = 0; i < NUM_CHANNELS; i++)
	{
	  {
	    uint32 sum_x  = 0;
	    uint32 sum_x2 = 0;
	    uint16 sorted[NUM_PEDESTAL_DATA];
	    
	    for (int j = 0; j < NUM_PEDESTAL_DATA; j++)
	      {
		sum_x  += apac[det_number][i]._data[j];
		sum_x2 += apac[det_number][i]._data[j] * apac[det_number][i]._data[j];
		sorted[j] = apac[det_number][i]._data[j];
	      }
	    
	    qsort(sorted,NUM_PEDESTAL_DATA,sizeof(sorted[0]),compare_uint16);
	    
	    for (int j = 0; j < NUM_PEDESTAL_DATA; j++)
	      {
		int count = 1;
		while (j+1 < NUM_PEDESTAL_DATA && sorted[j+1] == sorted[j])  { j++; count++; }
		//fprintf (stderr,"%d:%d:%d ",sorted[j],count,det_number);
	      }
	    /*fprintf (stderr,"\n");*/
	    
	    uint32 sum = NUM_PEDESTAL_DATA;
	    
	    double mean = ((double) sum_x) / sum;
	    double var  = ((double) sum_x2 - ((double) sum_x)*sum_x / sum)/(sum-1);
	    
	    //fprintf (stderr,"%6.1f %6.1f\n",mean,sqrt(var));
	  }
      
	  apac[det_number][i].analyse();
	  //printing for the test of December to provide the files 
	  //which will be load on the DAQ code
	  int pedestal = (int)(apac[det_number][i]._status._pedestal_mean*8); //pedestal multiply by 8
	  fprintf (ped,"%4x ",pedestal);
	  if((i+1)%16==0) fprintf(ped,"\n");
	  int sigma = (int)(apac[det_number][i]._status._pedestal_std*12); //sigma multiply by 12
	  fprintf (sig,"%4x ",sigma);
	  if((i+1)%16==0) fprintf(sig,"\n");
	  

	  //fprintf (stdout,"detector = %d, strip = %d: %6.1f+/-%4.1f , +/-%4.1f (%04x)\n",
	  //   det_number,i,
	  //   apac[det_number][i]._status._pedestal_mean,
	  //   sqrt(apac[det_number][i]._status._pedestal_mean_err2),
	  //   apac[det_number][i]._status._pedestal_std,
	  //   apac[det_number][i]._status._flags);
	}
      fclose(ped);
      fclose(sig);
    }
}


/*
int ilog2(unsigned int x)
{
  int l = 0;

  while (x & 0xffffff00)
    {
      x >>= 8;
      l += 8;
    }

  while (x >>= 1)
    l++;
  return l;
}

char hexilog2(unsigned int x)
{
  int il;

  if (!x)
    return '.';

  il = ilog2(x);

  if (il < 10)
    return il + '0';
  return il - 10 + 'A';
}

*/

#endif
