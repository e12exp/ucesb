#include "convert_picture.hh"

#include "forked_child.hh"

#include <string.h>
#include <stdio.h>

void convert_picture(const char *filename,
		     const char *pict,
		     int width,int height)
{
  forked_child writer;
  int fd;
  char header[128];

  const char *argv[4] = { "convert", "-", filename, NULL };
  
  writer.fork(argv[0],argv,NULL,&fd);
    
  sprintf (header,
	   "P5\n" 
	   "%d %d\n" 
	   "255\n",
	   width,height);

  size_t sz = sizeof(char) * (size_t) width * (size_t) height;

  full_write(fd,header,strlen(header));
  full_write(fd,pict,sz);

  writer.wait(false);
}

