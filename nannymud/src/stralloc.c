#include <stdio.h>
#include <string.h>

#include "config.h"
#include "lint.h"

#define SAVE_HASHVAL

/*
 * stralloc.c - string management.
 *
 * All strings are stored in an extensible hash table, with reference counts.
 * free_string decreases the reference count; if it gets to zero, the string
 * will be deallocated.  add_string increases the ref count if it finds a
 * matching string, or allocates it if it cant.  There is no way to allocate
 * a string of a particular size to fill later (hash wont work!), so you'll
 * have to copy things freom a static (or malloced and later freed) buffer -
 * that is, if you want to avoid space leaks...
 *
 * Current overhead:
 *	8 bytes per string (next pointer, and 2 shorts for length and refs)
 *	Strings are nearly all fairly short, so this is a significant overhead-
 *	there is also the 4 byte malloc overhead and the fact that malloc
 *	generally allocates blocks which are a power of 2 (should write my
 *	own best-fit malloc specialised to strings); then again, GNU malloc
 *	is bug free...
 */

/*
 * there is also generic hash table management code, but strings can be shared
 * (that was the point of this code), will be unique in the table,
 * require a reference count, and are malloced, copied and freed at
 * will by the string manager.  Besides, I wrote this code first :-).
 * Look at htable.c for the other code.  It uses the Hash() function
 * defined here, and requires hashed objects to have a pointer to the
 * next element in the chain (which you specify when you call the functions).
 */

#define	MAXSHORT	(1 << (sizeof(short)*8 - 2))
#define	WORD_ALIGN_BIT	0x3	/* these are 0 for aligned ptrs */

char * xalloc();
#ifndef _AIX
char * strcpy();
#endif
extern unsigned int hashstr2(char *a,int mlen);

static int num_distinct_strings = 0;
int bytes_distinct_strings = 0;
static int allocd_strings = 0;
static int allocd_bytes = 0;
int overhead_bytes = 0;
static int search_len = 0;
static int num_str_searches = 0;

/*
 * strings are stored:
 *	(char * next) (short hval) (short numrefs) string
 *				                   ^
 *                                               pointer points here
 */

struct shared_string
{
  char *next;
  short refs;
#ifdef SAVE_HASHVAL
  unsigned short hval;
#endif
  char str[1];
};

#define BASEOF(ptr, str_type, field)  \
((struct str_type *)((char*)ptr - (char*)& (((struct str_type *)0)->field)))

#define BASE(X) BASEOF((X),shared_string,str)
#define NEXT(X) (BASE(X)->next)
#define REFS(X) (BASE(X)->refs)
#define HVAL(X) (BASE(X)->hval)


/*
 * hash table - list of pointers to heads of string chains.
 * Each string in chain has a pointer to the next string and a
 * reference count (char *, int) stored just before the start of the string.
 * HTABLE_SIZE is in config.h, and should be a prime, probably between
 * 1000 and 5000.
 */

static char *base_table[HTABLE_SIZE];
#define HASHLEN 20

#ifdef SAVE_HASHVAL
static unsigned short short_hashval;
static unsigned int hashval;
#define HASHSTR(X) ((hashval=hashstr2(X,HASHLEN))%HTABLE_SIZE)
#else
#define HASHSTR(X) hashstr(X,HASHLEN,HTABLE_SIZE)
#endif

static void init_strings()
{
  int x;
  overhead_bytes = 0;

  for (x=0; x<HTABLE_SIZE; x++)
    base_table[x] = 0;
}

void walk_shared_strings(func)
    void (*func) PROT((char *, char *));
{
  int x;
  char *p, *n;

  for (x=0; x<HTABLE_SIZE; x++)
    for (n = base_table[x]; p = n; ) {
      n = NEXT(p);		/* p may be freed by (*func)() . */
      (*func)(p-sizeof(short)-sizeof(char *), p);
    }
}


/*
 * Looks for a string in the table.  If it finds it, returns a pointer to
 * the start of the string part, and moves the entry for the string to
 * the head of the pointer chain.  One thing (blech!) - puts the previous
 * pointer on the hash chain into fs_prev.
 */

char * findstring2(char *s,int h)
{
  char * curr, *prev;

  curr = base_table[h];
  prev = 0;
  num_str_searches++;

#ifdef SAVE_HASHVAL
  short_hashval=hashval;
#endif

  while (curr)
  {
    search_len++;
    if (curr == s || (
#ifdef SAVE_HASHVAL
		      short_hashval == HVAL(curr)
#else
		      curr[0] == s[0]
#endif
		      && !strcmp(curr, s)))
    {
      /* found it */
      if (prev)
      {		/* not at head of list */
	NEXT(prev) = NEXT(curr);
	NEXT(curr) = base_table[h];
	base_table[h] = curr;
      }
      return curr;		/* pointer to string */
    }
    prev = curr;
    curr = NEXT(curr);
  }
	
  return 0;			/* not found */
}

char *findstring(char *s)
{
  return findstring2(s,HASHSTR(s));
}

int print_string_refs(str)
     char *str;
{
  char *s;

  s = findstring(str);
  if(!s)
    return 0;
  return REFS(s);
}

char * make_shared_string(char *str)
{
  char * s;
  int h, len;

  h=HASHSTR(str);
  len=strlen(str);
  s = findstring2(str,h);
  if (!s)
  {
    s= xalloc(sizeof(struct shared_string) + len);

    s=((struct shared_string *)s)->str;
    memcpy(s, str, len);
    s[len]=0;
    REFS(s) = 0;
#ifdef SAVE_HASHVAL
    HVAL(s) = short_hashval;
#endif
    NEXT(s) = base_table[h];
    base_table[h] = s;
    num_distinct_strings++;
    bytes_distinct_strings += 4 + (len +3) & ~3;
    overhead_bytes += sizeof(struct shared_string)-1;
  }

  if(REFS(s) <= MAXSHORT)
    REFS(s)++;

  allocd_strings++;
  allocd_bytes += 4 + (len + 3) & ~3;
  return s;
}

char *copy_shared_string(str)
char *str;
{
  if(REFS(str) <= MAXSHORT)
    REFS(str)++;

  allocd_strings++;
  allocd_bytes += 4 + (strlen(str) + 3) & ~3;
  return str;
}

/*
 * free_string - reduce the ref count on a string.  Various sanity
 * checks applied, the best of them being that a add_string allocated
 * string will point to a word boundary + sizeof(int)+sizeof(short),
 * since malloc always allocates on a word boundary.
 * On systems where a short is 1/2 a word, this gives an easy check
 * to see whather we did in fact allocate it.
 *
 * Don't worry about the overhead for all those checks!
 */

/*
 * function called on free_string detected errors; things return checked(s).
 */

static void checked(s, str) char * s, *str;
{
	fprintf(stderr, "%s (\"%s\")\n", s, str);
	fatal(s); /* brutal - debugging */
}

void free_string(str)
char * str;
{
	char * s;
	int h;

	allocd_strings--;
	allocd_bytes -= 4 + (strlen(str) + 3) & ~3;

#ifdef BUG_FREE
	if (REFS(s) > MAXSHORT) return;
	REFS(s)--;
	if (REFS(s) > 0) return;

	h=HASHSTR(str);
	s = findstring2(str,h); /* moves it to head of table if found */
#else
	{
	  int align;
	  align = ((long)BASE(str)) & 3;
	  if (align)
	    checked("Free string: improperly aligned string!", str);
	}

	h=HASHSTR(str);
	s = findstring2(str,h); /* moves it to head of table if found */

	if (!s) {
	    checked("Free string: not found in string table!", str);
	    return;
	}
	if (s != str) {
	    checked("Free string: string didnt hash to the same spot!", str);
	    return;
	}

	if (REFS(s) <= 0) {
	    checked("Free String: String refs zero or -ve!", str);
	    return;
	}

	if (REFS(s) > MAXSHORT) return;
	REFS(s)--;
	if (REFS(s) > 0) return;

#endif	/* BUG_FREE */


	/* It will be at the head of the hash chain */
	base_table[h] = NEXT(s);
	num_distinct_strings--;
	/* We know how much overhead malloc has */
	bytes_distinct_strings-= 4 + (strlen(s) + 3) & ~3;
	overhead_bytes -= sizeof(struct shared_string)-1;
	free(BASE(s));

	return;
}

/*
 * you think this looks bad!  and we didn't even tell them about the
 * GNU malloc overhead!  tee hee!
 */

int add_string_status(verbose)
    int verbose;
{
    if (verbose) {
	add_message("\nShared string hash table:\n");
	add_message("-------------------------\t Strings    Bytes\n");
    }
    add_message("Strings malloced\t\t%8d %8d + %d overhead\n",
		num_distinct_strings, bytes_distinct_strings, overhead_bytes);
    if (verbose) {
	add_message("Total asked for\t\t\t%8d %8d\n",
		    allocd_strings, allocd_bytes);
	add_message("Space actually required/total string bytes %d%%\n",
		    (bytes_distinct_strings + overhead_bytes)*100 / allocd_bytes);
	add_message("Searches: %d    Average search length: %6.3f\n",
		    num_str_searches, (double)search_len / num_str_searches);
    }
    return bytes_distinct_strings + overhead_bytes;
}

#ifdef DEALLOCATE_MEMORY_AT_SHUTDOWN
void free_all_strings(void)
{
#ifdef DEBUG
   int i;
   for(i = 0; i < HTABLE_SIZE; i++)
   {
      char *ptr, *n;
      for(ptr = base_table[i]; ptr; ptr = n)
      {
	 n = NEXT(ptr);
	 printf("Freeing string \"%s\" [%lx] (%d)\n", ptr, ptr, REFS(ptr));
	 while(REFS(ptr) > 1)
	    free_string(ptr);
	 free_string(ptr);
      }
   }
#endif
#if 0
   free(base_table);
   base_table = NULL;
   overhead_bytes -= (sizeof(char *) * HTABLE_SIZE);
#endif
}
#endif
