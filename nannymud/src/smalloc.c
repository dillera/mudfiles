/* Satoria's malloc intended to be optimized for lpmud.
** this memory manager distinguishes between two sizes
** of blocks: small and large.  It manages them separately
** in the hopes of avoiding fragmentation between them.
** It expects small blocks to mostly be temporaries.
** It expects an equal number of future requests as small
** block deallocations.
*/
#include <stdio.h>
#include "memory.h"
#include "lint.h"

/* #define MALLOC_DEBUG */
/* #define STATISTICS */

#ifdef MSDOS
#define char void
#endif

#define fake(s)
#define smalloc malloc
#define sfree free
#define srealloc realloc

#define SMALL_BLOCK_MAX_BYTES	128       /* Oros 930115  Old value: 32 */
#define SMALL_CHUNK_SIZE	0x4000
#define CHUNK_SIZE		0x40000

#define SINT sizeof(int)
#define SMALL_BLOCK_MAX (SMALL_BLOCK_MAX_BYTES/SINT)

#define PREV_BLOCK	0x80000000
#define THIS_BLOCK	0x40000000
#define MASK		0x0FFFFFFF

#define MAGIC		0x17952932

void sfree(char *ptr);

/* SMALL BLOCK info */

typedef unsigned int u;

static u *sfltable[SMALL_BLOCK_MAX];	/* freed list */
static u *next_unused;
static u unused_size;			/* until we need a new chunk */

/* LARGE BLOCK info */

#define NUM_FREE_LISTS 400 /* hashsize(MASK)+1 */
static u *free_lists[NUM_FREE_LISTS];
static u *start_next_block;

/* STATISTICS */

#ifdef STATISTICS
static int small_count[SMALL_BLOCK_MAX];
static int small_total[SMALL_BLOCK_MAX];
static int small_max[SMALL_BLOCK_MAX];
static int small_free[SMALL_BLOCK_MAX];
#endif

typedef struct { unsigned counter, size; } stat;
#define count(a,b) { a.size+=(b); if ((b)<0) --a.counter; else ++a.counter; }

int debugmalloc=0;	/* Only used when debuging malloc() */

/********************************************************/
/*  SMALL BLOCK HANDLER					*/
/********************************************************/

static char *large_malloc();
static void large_free();

#define s_size_ptr(p)	(p)
#define s_next_ptr(p)	((u **) (p+1))

stat small_alloc_stat;
stat small_free_stat;
stat small_chunk_stat;

#define SHIFT 4

static u hashsize(u size)
{
  static char bit[256]={
   -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  };
  u s;

  size&=MASK;
  if(s=size>>24) s=bit[s]+24;
  else if(s=size>>16) s=bit[s]+16;
  else if(s=size>>8) s=bit[s]+8;
  else s=bit[size];
  s-=SHIFT;
  if(s<0) s=0;

  return (s<<SHIFT)+(size>>s);
}


char *smalloc(size)
  u size;
{
  int i;
  u *temp;

  if (size == 0)
      fatal("Malloc size 0.\n");

#ifdef MALLOC_DEBUG
  verify_sfltable();
#endif

  if (size>SMALL_BLOCK_MAX_BYTES)
    return large_malloc(size,0);

  i = (size - 1) >> 2;
  size = i+2;				/* block size in ints */
  count(small_alloc_stat,size << 2);

#ifdef STATISTICS
  small_count[i] += 1;			/* update statistics */
  small_total[i] += 1;
  if (small_count[i] >= small_max[i])
    small_max[i] = small_count[i];
#endif

  if (sfltable[i]) 
    {					/* allocate from the free list */
      count(small_free_stat, -(int) (size << 2));
      temp = sfltable[i];
      sfltable[i] = * (u **) (temp+1);
#ifdef STATISTICS
      small_free[i] -= 1;
#endif
fake("From free list.");
      return (char *) (temp+1);
    }					/* else allocate from the chunk */

  if (unused_size<size)			/* no room in chunk, get another */
    {
      if (unused_size>1) {              /* Mats 960307 */
        temp = (u *) s_next_ptr(next_unused); 
        *s_size_ptr(next_unused) = unused_size;
        free((char *) temp);
      }
      fake("Allocating new small chunk.");
      next_unused = (u *) large_malloc(SMALL_CHUNK_SIZE,1);
      if (next_unused == 0)
	return 0;
      count(small_chunk_stat, SMALL_CHUNK_SIZE+SINT);
      unused_size = SMALL_CHUNK_SIZE / SINT;
    }
else fake("Allocated from chunk.");


  temp = (u *) s_next_ptr(next_unused); 

  *s_size_ptr(next_unused) = size;
  next_unused += size;
  unused_size -= size;

  return (char *) temp;
}

char *debug_free_ptr;

void sfree(ptr)
char *ptr;
{
  u *block;
  u i;

#ifdef MALLOC_DEBUG
  verify_sfltable();
  debug_free_ptr = ptr;  /* Why this?  Mats */
#endif

  block = (u *) ptr;
  block -= 1;
  if ((*s_size_ptr(block) & MASK) > SMALL_BLOCK_MAX + 1)
    { large_free(ptr); return; }

  i = *block - 2;
  count(small_alloc_stat, - (int) ((i+2) << 2));
  count(small_free_stat, (i+2) << 2);
#ifdef MALLOC_DEBUG
  {
    u *e;
    for(e=sfltable[i];e;e=*s_next_ptr(e))
    {
      if(e==block)
      {
        fprintf(stderr,"Freeing small block again.\n");
        apa();
      }
    }
  }
#endif
  *s_next_ptr(block) = sfltable[i];
  sfltable[i] = block;
#ifdef STATISTICS
  small_free[i] += 1;
  small_count[i] -= 1;			/* update statistics */
#endif
fake("Freed");
}

/************************************************/
/*	LARGE BLOCK HANDLER			*/
/************************************************/

#define BEST_FIT	0
#define FIRST_FIT	1
#define HYBRID		2
int fit_style =BEST_FIT;

#define l_size_ptr(p)		(p)
#define l_next_ptr(p)		(*((u **) (p+1)))
#define l_prev_ptr(p)		(*((u **) (p+2)))
#define l_next_block(p)		(p + (*(p)))
#define l_prev_block(p) 	(p - (*(p-1)))
#define l_prev_free(p)		(!(*p & PREV_BLOCK))
#define l_next_free(p)		(!(*l_next_block(p) & THIS_BLOCK))

void show_block(ptr)
u *ptr;
{
  printf("[%c%d: %d]  ",(*ptr & THIS_BLOCK ? '+' : '-'),
		(int) ptr, *ptr & MASK);
}

void show_free_list()
{
   u *p;
   int e;
   for(e=0;e<NUM_FREE_LISTS;e++)
   {
     p = free_lists[e];
     if (p) printf("%d = ",e);
     while (p) {
       show_block(p);
       p = l_next_ptr(p);
     }
     if (free_lists[e]) printf("\n");
   }
#ifdef STATISTICS
   printf("\nsize:   count      total       max       free\n");
   for(e=0;e<SMALL_BLOCK_MAX;e++) {
     printf("%2d: %8d   %8d   %8d   %8d\n",e,
	    small_count[e],small_total[e],small_max[e],small_free[e]);
   }
#else
   printf("\n");
#endif
   fflush(stdout);
}

stat large_free_stat;     
void remove_from_free_list(ptr)
u *ptr;
{
  u size, hval;
  size=*ptr & MASK;
  hval=hashsize(size);
  count(large_free_stat, - (int) (size << 2));

  if (l_prev_ptr(ptr))
    l_next_ptr(l_prev_ptr(ptr)) = l_next_ptr(ptr);
  else
    free_lists[hval] = l_next_ptr(ptr);

  if (l_next_ptr(ptr))
    l_prev_ptr(l_next_ptr(ptr)) = l_prev_ptr(ptr);
}

void add_to_free_list(ptr)
u *ptr;
{
  extern int puts();
  u size, hval;
  size=*ptr & MASK;
  hval=hashsize(size);

  count(large_free_stat, size << 2);

  if (free_lists[hval] && l_prev_ptr(free_lists[hval])) 
    puts("Free list consistency error.");

  l_next_ptr(ptr) = free_lists[hval];
  if (free_lists[hval]) 
    l_prev_ptr(free_lists[hval]) = ptr;
  l_prev_ptr(ptr) = 0;
  free_lists[hval] = ptr;
}

void build_block(ptr, size)	/* build a properly annotated unalloc block */
u *ptr;
u size;
{
  *(ptr) = (*ptr & PREV_BLOCK) | size;		/* mark this block as free */
  *(ptr+size-1) = size;
  *(ptr+size) &= (MASK | THIS_BLOCK); /* unmark previous block */
}

static void mark_block(ptr)		/* mark this block as allocated */
u *ptr;
{
  *l_next_block(ptr) |= PREV_BLOCK;
  *ptr |= THIS_BLOCK;
}

/*
 * It is system dependent how sbrk() aligns data, so we simpy use brk()
 * to insure that we have enough.
 */
stat sbrk_stat;
static char *current_break;
static char *esbrk(size)
u size;
{
  extern char *sbrk();
  extern int brk();

  if (current_break == 0)
    current_break = sbrk(0);
  if (brk(current_break + size) == -1)
    return 0;
  count(sbrk_stat,size);
  current_break += size;
  return current_break - size;
}

stat large_alloc_stat;
static char *large_malloc(size, force_more)
u size;
int force_more;
{
  u best_size, real_size;
  u *first, *best, *ptr;
  int hashval;
  u max_size,fit_size;

  size = (size + 7) >> 2; 		/* plus overhead */
  first = best = 0;
  best_size = MASK;

  max_size = size + 1;  /* Mats 960307 */
  if (size>SMALL_BLOCK_MAX*2) max_size += 2;
  if (size>SMALL_BLOCK_MAX*16) max_size += 4;
  if (size>SMALL_BLOCK_MAX*32) max_size += 8;

  fit_size = size * 2;
  if (size>SMALL_BLOCK_MAX*4) fit_size = size + SMALL_BLOCK_MAX*4;

  if (!force_more)
  {
    hashval=hashsize(size)-1;
    while(!first)
    {
      hashval++;
      if(hashval >= NUM_FREE_LISTS) break;
      for(ptr=free_lists[hashval];ptr;ptr = l_next_ptr(ptr))
      {
	u tempsize;
	/* Perfect fit? */
	tempsize = *ptr & MASK;
	if (tempsize == size)
	{
	  best = first = ptr;
	  break;		/* always accept perfect fit */
	}

	if (tempsize > size && tempsize <= max_size)  /* Mats 960307 */
	{
	  best = first = ptr;
	  continue;    /* there might be a better fit in list */
	}

	/* does it really even fit at all */
        if (tempsize >= fit_size)   /* Mats 960307 */
	{
	  /* try first fit */
	  if (!first) 
	  {
	    first = ptr;
	    if (fit_style == FIRST_FIT)
	      break;		/* just use this one! */
	  }
	  /* try best fit */
	  tempsize -= size;
	  if (tempsize>0 && tempsize<=best_size) 
	  {
	    best = ptr;
	    best_size = tempsize;
	  }
	}
      }
    }
  } 


  if (fit_style==BEST_FIT)
    ptr = best;
  else
    ptr = first;	/* FIRST_FIT and HYBRID both leave it in first */

  if (!ptr)		/* no match, allocate more memory */
  {
    u chunk_size, block_size;
    block_size = size*SINT;
    if (force_more || (block_size>CHUNK_SIZE))
      chunk_size = block_size;
    else
      chunk_size = CHUNK_SIZE;

    if (!start_next_block) {
      start_next_block = (u *) esbrk(SINT);
      if (!start_next_block)
	return 0;
      *(start_next_block) = PREV_BLOCK;
      fake("Allocated little fake block");
    }

    ptr = (u *) esbrk(chunk_size);
    if (ptr == 0)
      return 0;
    ptr -= 1;			/* overlap old memory block */
    block_size = chunk_size / SINT;

    /* configure header info on chunk */
      
    build_block(ptr,block_size);
    if (force_more)
      fake("Build little block");
    else
      fake("Built memory block description.");
    *l_next_block(ptr)=THIS_BLOCK;

    add_to_free_list(ptr, hashsize(block_size));
  } /* end of creating a new chunk */

  remove_from_free_list(ptr);
  real_size = *ptr & MASK;

  if (real_size > max_size) /* Mats 960307 */
  {
    /* split block pointed to by ptr into two blocks */
    build_block(ptr+size, real_size-size);
    fake("Built empty block");
    add_to_free_list(ptr+size);
    build_block(ptr, size);
  } else
    size = real_size;

  count(large_alloc_stat, size << 2);
  mark_block(ptr);
  fake("built allocated block");
  return (char *) (ptr + 1);
}

static void large_free(ptr)
char *ptr;
{
  u size, *p;
  p = (u *) ptr;
  p-=1;
  size = *p & MASK;
  count(large_alloc_stat, - (int) (size << 2));

  if (l_next_free(p))
  {
    remove_from_free_list(l_next_block(p));
    size += (*l_next_block(p) & MASK);
    *p = (*p & PREV_BLOCK) | size;
  }

  if (l_prev_free(p))
  {
    remove_from_free_list(l_prev_block(p));
    size += (*l_prev_block(p) & MASK);
    p = l_prev_block(p);
  }

  build_block(p, size);

  add_to_free_list(p);
}

char *srealloc(p, size)
char *p; unsigned size;
{
   unsigned *q, old_size;
   char *t;
   
   q = (unsigned *) p;
   --q;
   old_size = ((*q & MASK)-1)*sizeof(int);
   if (old_size >= size)
      return p;

   t = malloc(size);
   if (t == 0) return (char *) 0;

   memcpy(t, p, old_size);
   free(p);
   return t;
}



int resort_free_list() { return 0; }
#define dump_stat(str,stat) add_message(str,stat.counter,stat.size)
void dump_malloc_data()
{
  add_message("Type                   Count      Space (bytes)\n");
  dump_stat("sbrk requests:     %8d        %10d (a)\n",sbrk_stat);
  dump_stat("large blocks:      %8d        %10d (b)\n",large_alloc_stat);
  dump_stat("large free blocks: %8d        %10d (c)\n\n",large_free_stat);
  dump_stat("small chunks:      %8d        %10d (d)\n",small_chunk_stat);
  dump_stat("small blocks:      %8d        %10d (e)\n",small_alloc_stat);
  dump_stat("small free blocks: %8d        %10d (f)\n",small_free_stat);
  add_message(
"unused from current chunk          %10d (g)\n\n",unused_size<<2);
  add_message(
"    Small blocks are stored in small chunks, which are allocated as\n");
  add_message(
"large blocks.  Therefore, the total large blocks allocated (b) plus\n");
  add_message(
"the large free blocks (c) should equal total storage from sbrk (a).\n");
  add_message(
"Similarly, (e) + (f) + (g) equals (d).  The total amount of storage\n");
  add_message(
"wasted is (c) + (f) + (g); the amount allocated is (b) - (f) - (g).\n");
}

int cur_mem_use(void)
{
   return large_alloc_stat.size - small_free_stat.size - (unused_size << 2);
}

/*
 * calloc() is provided because some stdio packages uses it.
 */
char *calloc(nelem, sizel)
#ifndef MSDOS
    int nelem, sizel;
#else
    unsigned nelem, sizel;
#endif
{
    char *p;

    if (nelem == 0 || sizel == 0)
	return 0;
    p = malloc(nelem * sizel);
    if (p == 0)
	return 0;
    (void)memset(p, '\0', nelem * sizel);
    return p;
}

/*
 * Functions below can be used to debug malloc.
 */

#ifdef MALLOC_DEBUG

int debugmalloc;
/*
 * Verify that the free list is correct. The upper limit compared to
 * is very machine dependant.
 */
verify_sfltable() {
    u *p;
    int i, j;
    extern int end;

    if (!debugmalloc)
	return;

    if (unused_size > SMALL_CHUNK_SIZE / SINT)
	apa();
    for (i=0; i < SMALL_BLOCK_MAX; i++) {
	for (j=0, p = sfltable[i]; p; p = * (u **) (p + 1), j++) {
	    if (p < (u *)&end || p > (u *) current_break)
		apa();
	    if (*p - 2 != i)
		apa();
	}
	if (p >= next_unused && p < next_unused + unused_size)
	    apa();
    }
    p = free_list;
    while (p) {
	if (p >= next_unused && p < next_unused + unused_size)
	    apa();
	p = l_next_ptr(p);
    }
}

verify_free(ptr)
    u *ptr;
{
    u *p;
    int i, j;

    if (!debugmalloc)
	return;
    for (i=0; i < SMALL_BLOCK_MAX; i++) {
	for (j=0, p = sfltable[i]; p; p = * (u **) (p + 1), j++) {
	    if (*p - 2 != i)
		apa();
	    if (ptr >= p && ptr < p + *p)
		apa();
	    if (p >= ptr && p < ptr + *ptr)
		apa();
	    if (p >= next_unused && p < next_unused + unused_size)
		apa();
	}
    }

    p = free_list;
    while (p) {
	if (ptr >= p && ptr < p + (*p & MASK))
	    apa();
	if (p >= ptr && p < ptr + (*ptr & MASK))
	    apa();
	if (p >= next_unused && p < next_unused + unused_size)
	    apa();
	p = l_next_ptr(p);
    }
    if (ptr >= next_unused && ptr < next_unused + unused_size)
	apa();
}

apa() {
  abort();
}

static char *ref;
test_malloc(p)
    char *p;
{
    if (p == ref)
	printf("Found 0x%x\n", p);
}

#else
verify_sfltable() {}

#endif /* MALLOC_DEBUG */
