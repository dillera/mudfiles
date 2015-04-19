/*
 * This is a dummy file, that will force the standard malloc
 * to be used.
 */

#include <malloc.h>

#define SMALL_BLOCK_SIZE  128
#define GRANULARITY         4


init_malloc()
{
#if 0
  /* no such thing in solaris */
  if(mallopt(M_MXFAST, SMALL_BLOCK_SIZE) != 0)
  {
    fatal("Mallopt(M_MXFAST..) failed!");
  }
  if(mallopt(M_GRAIN, GRANULARITY) != 0)
  {
    fatal("Mallopt(M_GRAIN..) failed!");
  }
#endif
}
  
