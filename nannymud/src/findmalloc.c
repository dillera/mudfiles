#include "lint.h"
#include "interpret.h"
#include "exec.h"
#include "object.h"
#include "call_out.h"

#define MARKER_CHUNK_SIZE 1024
#define REHASH_LIMIT 16
#define REHASH_FORMULA(X) ((X)*4-1)

struct marker
{
  struct marker *next;
  void *marked;
};

struct marker_chunk
{
  struct marker_chunk *next;
  struct marker markers[MARKER_CHUNK_SIZE];
};

static struct marker_chunk *chunk=0;
static int markers_left_in_chunk=0;

struct marker *new_marker()
{
  if(!markers_left_in_chunk)
  {
    struct marker_chunk *m;
    m=(struct marker_chunk *)xalloc(sizeof(struct marker_chunk));
    m->next=chunk;
    chunk=m;
    markers_left_in_chunk=MARKER_CHUNK_SIZE;
  }
  markers_left_in_chunk--;

  return chunk->markers + markers_left_in_chunk;
}

static struct marker **hash=0;
static int hashsize=0;
static int hashed=0;

static int checked(void *a)
{
  int hashval;
  struct marker *m;
  hashval=((int)a)%hashsize;

  for(m=hash[hashval];m;m=m->next)
    if(m->marked == a)
      return 1;

  m=new_marker();
  m->marked=a;
  m->next=hash[hashval];
  hash[hashval]=m;

  hashed++;
  if(hashed / REHASH_LIMIT > hashsize)
  {
    struct marker **new_hash,*next;
    int new_hashsize;
    int e;

    new_hashsize=REHASH_FORMULA(hashsize);
    new_hash=(struct marker **)xalloc(sizeof(struct marker **)*new_hashsize);
    memset((char *)new_hash,0,sizeof(struct marker **)*new_hashsize);

    for(e=0;e<hashsize;e++)
    {
      for(m=hash[e];m;m=next)
      {
	next=m->next;
	m->next=new_hash[((int)m->marked)%new_hashsize];
	new_hash[((int)m->marked)%new_hashsize]=m;
      }
    }

    free((char *)hash);
    hash=new_hash;
    hashsize=new_hashsize;
  }

  return 0;
}

static int malloced_strings;
static int malloced_string_size;
static int malloced_strings_shared;
static int malloced_strings_shared_size;
static int level;

static void count_malloc_strings(char *s)
{
  malloced_strings++;
  malloced_string_size+=strlen(s)+1;
  if(level>0)
  {
    if(findstring(s))
    {
      malloced_strings_shared++;
      malloced_strings_shared_size+=strlen(s)+1;
    }
  }
}

static void check_svalues(struct svalue *s, int num);

static void check_object(struct object *o)
{
  if(!checked(o))
  {
    check_svalues(o->variables, o->prog->num_variables);
    count_malloc_strings(o->name);
    
    if(!checked(o->prog))
    {
      count_malloc_strings(o->prog->name);
    }
  }
}

static void check_svalues(struct svalue *s, int num)
{
  int e;
  for(e=0;e<num;e++)
  {
    switch(s[e].type)
    {
    case T_STRING:
      if(s[e].string_type == STRING_MALLOC)
      {
	count_malloc_strings(s[e].u.string);
	if(level>1)
	{
	  char *str;
	  str=make_shared_string(s[e].u.string);
	  free(s[e].u.string);
	  s[e].u.string=str;
	  s[e].string_type=STRING_SHARED;
	}
      }
      break;

    case T_POINTER:
    case T_MAPPING:
      if(!checked(s[e].u.vec))
	check_svalues(s[e].u.vec->item, s[e].u.vec->size);
      break;

    case T_OBJECT:
      check_object(s[e].u.ob);
    }
  }
}

void count_malloced_strings(int force)
{
  extern struct svalue *sp;
  extern struct svalue start_of_stack[];
  extern struct object *obj_list;
  extern struct call *call_list;

  struct marker_chunk *m,*next;
  struct object *o;
  struct call *cop;

  /* init */
  level=force;
  hashsize=4711;
  hashed=0;
  malloced_strings=0;
  malloced_string_size=0;
  malloced_strings_shared=0;
  malloced_strings_shared_size=0;
  hash=(struct marker **)xalloc(sizeof(struct marker **)*hashsize);
  memset((char *)hash,0,sizeof(struct marker **)*hashsize);
  markers_left_in_chunk=0;

  check_svalues(start_of_stack, sp-start_of_stack+1);

  for(o=obj_list;o;o=o->next_all)
    check_object(o);

  for(cop=call_list;cop;cop=cop->next)
    check_svalues(&cop->v, 1);

  add_message("Malloced strings: %d\n", malloced_strings);
  add_message("Total size      : %d\n", malloced_string_size);
  if(level>0)
  {
    add_message("Of those, shared: %d\n", malloced_strings_shared);
    add_message("Size of those   : %d\n", malloced_strings_shared_size);
  }

  /* cleanup */
  free((char *)hash);
  while(m=chunk)
  {
    chunk=m->next;
    free((char *)m);
  }
}
