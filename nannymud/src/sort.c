

#include <stdlib.h>
#include <string.h>
#define PROT(xx) ()
#include "interpret.h"


#if 0
void msort(base, elms, elmSize, cmpfunc)
void *base;
long elms;
long elmSize;
int (*cmpfunc)(const void *, const void *);
{
  long i, j, i1, i2;
  void *tmp = malloc(elmSize);

  for (i = 2; (i >> 1) < elms; i *= 2)        /*  merge sets of 2,4,8.. */
  {
    for (j = 0; j < elms; j += i)
    {
      long i1 = i >> 1;
      long i2 = i1;
      char *p1;
      char *p2;

      p1 = (char *)base + j * elmSize;
      p2 = p1 + i1 * elmSize;

      if (j + i1 + i2 > elms) i2 = elms - i1 - j;

      /*
       *  Merge p2/i2 into p1/i1 until p2/i2 is exausted.  Note that
       *  i2 might start out negative (in which case i1 is invalid,
       *  but the segment is already sorted anyway)
       */

      while (i2 > 0 && i1 > 0)
      {
        /*
         *  compare current p1/i1 with next p2/i2.  Skip p1 while
         *  p1 < p2.  Otherwise, merge the p2/i2 element into
         *  p1/i1.
         */

        if ((*cmpfunc)(p1, p2) <= 0)
        {
          --i1;
          p1 += elmSize;
        }else{
          movmem(p2, tmp, elmSize);
          movmem(p1, p1 + elmSize, i1 * elmSize);
          movmem(tmp, p1, elmSize);
          --i2;
          p2 += elmSize;
          p1 += elmSize;   /*  shifted p1, i1 stays    */
        }
      }
    }
  }
  free(tmp);
}

#endif

void sswap(p1, p2)
     struct svalue *p1;
     struct svalue *p2;
{
  struct svalue tmp;


  memcpy(&tmp, p1,   sizeof(struct svalue));
  memcpy(p1,   p2,   sizeof(struct svalue));
  memcpy(p2,   &tmp, sizeof(struct svalue));
}

void pushdown(base, cmpfunc, first, last)
     struct svalue *base;
     int   (*cmpfunc)(struct svalue *, struct svalue *);
     int   first;
     int   last;
{
  int r;

  r = first;
  while(r <= last/2)
  {
    if(last == 2*r)
    {
      if ((*cmpfunc)(&base[r], &base[2*r]) > 0)
      {
	sswap(&base[r], &base[2*r]);
	r = last;
      }
    }
    else 
    {
      if (((*cmpfunc)(&base[r], &base[2*r]) > 0) &&
	  ((*cmpfunc)(&base[2*r],&base[2*r+1]) <= 0))
      {
	sswap(&base[r], &base[2*r]);
	r = 2*r;
      }
      else if ((*cmpfunc)(&base[r], &base[2*r+1]) > 0 &&
	       (*cmpfunc)(&base[2*r+1], &base[2*r]) < 0)
      {
	sswap(&base[r], &base[2*r+1]);
	r = 2*r+1;
      }
      else
	r = last;
    }
  }
}
      
	

	
      
  

void hsort(base, elms, elmSize, cmpfunc)
     struct svalue *base;
     long elms;
     long elmSize;
     int (*cmpfunc)(struct svalue *, struct svalue *);
{
  int   i;

  for(i = (elms-1)/2; i; i--)
  {
    pushdown(base,cmpfunc, i, elms-1);
  }
  for(i = elms-1; i > 1; i--)
  {
    sswap(&base[0], &base[i]);
    pushdown(base,cmpfunc, 0, i-1);
  }
}
