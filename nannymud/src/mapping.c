#include <string.h>
#ifdef sun
#include <alloca.h>
#endif
#include "config.h"
#include "lint.h"
#include "interpret.h"
#include "object.h"
#include "wiz_list.h"
#include "regexp.h"
#include "exec.h"

#if defined(__GNUC__) && !defined(lint)
#define INLINE inline
#else
#define INLINE
#endif

extern struct vector *order_alist PROT((struct vector *));
extern struct svalue *insert_alist PROT((struct svalue *key,
					 struct svalue *key_data,
					 struct vector *list));
extern int assoc PROT((struct svalue *key, struct vector *keys));

extern struct svalue const0;

struct vector *allocate_mapping(v, w)
struct vector *v, *w;
{
    struct vector *m;

    m = allocate_array(2);
    m->item[0].u.vec = v;
    m->item[0].type = T_POINTER;
    m->item[1].u.vec = w;
    m->item[1].type = T_POINTER;
    return m;
}

void free_mapping(m)
struct vector *m;
{
    free_vector(m);
}

struct svalue *get_map_lvalue(m, index, needlval)
struct vector *m;
struct svalue *index;
int needlval;
{
    int i;

    if(needlval)
    {
      if(index->type == T_OBJECT)
	m->item[0].u.vec->flags &=~ VECTOR_CLEAN;

      m->item[1].u.vec->flags &=~ VECTOR_CLEAN;
    }

    i = assoc(index, m->item[0].u.vec);
    if (i < 0) {
	if (needlval) {
	    struct vector *v;

	    /*
	     * NOTE: we change the mapping here!
	     */
	    v = insert_alist(index, (&const0)-1, m)->u.vec;
	    free_vector(m->item[0].u.vec);
	    m->item[0].u.vec = v->item[0].u.vec;
	    free_vector(m->item[1].u.vec);
	    m->item[1].u.vec = v->item[1].u.vec;
	    v->item[0].type = T_NUMBER;
	    v->item[1].type = T_NUMBER;
	    free_vector(v);

	    i = assoc(index, m->item[0].u.vec);
	    if(i==-1)
	      error("Failed to add entry to mapping (shouldn't happen)\n");
	} else {
	    static struct svalue zero;

	    /* should not be overwritten, but who knows... */
	    zero = const0;
	    return &zero;
	}
    }
    return & m->item[1].u.vec->item[i];
}

void remove_mapping(m, i)
struct vector *m;
int i;
{
    struct svalue *p, *q;
    int size;
    extern int total_array_size;

    /* use old space */
    size = m->item[0].u.vec->size;
    p = m->item[0].u.vec->item;
    q = m->item[1].u.vec->item;
    free_svalue(p+i);
    free_svalue(q+i);
    for (size = m->item[0].u.vec->size - 1; i < size; i++) {
	p[i] = p[i + 1];
	q[i] = q[i + 1];
    }
    total_array_size -= sizeof (struct svalue) * 2;
    if (m->user)
	m->user->size_array-=2;
    m->item[0].u.vec->size = size;
    m->item[1].u.vec->size = size;
}

struct vector *add_mapping(v, w)
  struct vector *v, *w;
{
#if 1
  int a,b;
  struct vector *ind,*val;
  int size;
  size=v->item[0].u.vec->size+w->item[0].u.vec->size;
  ind=allocate_array(size);
  val=allocate_array(size);
  size=a=b=0;
  while(a<v->item[0].u.vec->size && b<w->item[0].u.vec->size)
  {
    if(0>alist_cmp(v->item[0].u.vec->item+a,w->item[0].u.vec->item+b))
    {
      assign_svalue_no_free(ind->item+size,v->item[0].u.vec->item+a);
      assign_svalue_no_free(val->item+size,v->item[1].u.vec->item+a);
      a++;
    }else{
      assign_svalue_no_free(ind->item+size,w->item[0].u.vec->item+b);
      assign_svalue_no_free(val->item+size,w->item[1].u.vec->item+b);
      b++;
    }
    size++;
  }

  while(a<v->item[0].u.vec->size)
  {
    assign_svalue_no_free(ind->item+size,v->item[0].u.vec->item+a);
    assign_svalue_no_free(val->item+size,v->item[1].u.vec->item+a);
    a++;
    size++;
  }

  while( b<w->item[0].u.vec->size)
  {
    assign_svalue_no_free(ind->item+size,w->item[0].u.vec->item+b);
    assign_svalue_no_free(val->item+size,w->item[1].u.vec->item+b);
    b++;
    size++;
  }
  return allocate_mapping(ind,val);
#else
  struct vector *tmp,*tmp2;
  tmp=allocate_mapping(add_array(v->item[0].u.vec,
				 w->item[0].u.vec),
		       add_array(v->item[1].u.vec,
				 w->item[1].u.vec));
  tmp2=order_alist(tmp);
  free_vector(tmp); /* gotta free this /Profezzorn */
  return tmp2;
#endif
}

struct vector *filter_mapping (p, func, ob, extra)
    struct vector *p;
    char *func;
    struct object *ob;
    struct svalue *extra;
{
  extern void my_free();
  extern int onerror();
  extern int remove_onerror();
  int onerror_num;
    struct vector *q, *r;
    struct svalue *v;
    char *flags;
    int cnt,res;
    
    res=0;
    r=0;
    if ( !func || !ob || (ob->flags & O_DESTRUCTED)) {
	return 0;
    }
    if (p->item[0].u.vec->size<1) {
	p = allocate_array(0);
	return allocate_mapping(p, p);
    }

    q = p->item[1].u.vec;
    flags=xalloc(q->size+1); 
    onerror_num=onerror(my_free,flags);

    for (cnt=0;cnt<q->size;cnt++) {
	flags[cnt]=0;
	push_svalue(&q->item[cnt]);
	if (extra) {
	    push_svalue(extra);
	    v = apply (func, ob, 2);
	} else {
	    v = apply (func, ob, 1);
	}
	if ((v) && (v->type==T_NUMBER)) {
	    if (v->u.number) { flags[cnt]=1; res++; }
	}
    }
  remove_onerror(onerror_num);
    q = allocate_array(res);
    r = allocate_array(res);
    if (res) {
	for (cnt = res = 0; res < q->size; cnt++) {
	    if (flags[cnt]) {
		assign_svalue_no_free (&q->item[res],
				       &p->item[0].u.vec->item[cnt]);
		assign_svalue_no_free (&r->item[res++],
				       &p->item[1].u.vec->item[cnt]);
	    }
	}
    }
    free(flags);
    return allocate_mapping(q, r);
}

struct vector *map_mapping (arr, func, ob, extra)
    struct vector *arr;
    char *func;
    struct object *ob;
    struct svalue *extra;
{
  extern int onerror();
  extern int remove_onerror();
  int onerror_num;
    struct vector *q, *r;
    struct svalue *v;
    int cnt;
    
    if (arr->size < 1) {
	arr = allocate_array(0);
	return allocate_mapping(arr, arr);
    }

    q = arr->item[0].u.vec;
    arr = arr->item[1].u.vec;
    r = allocate_array(arr->size);

    onerror_num=onerror(free_vector, r);
    
    for (cnt = 0; cnt < arr->size; cnt++)
    {
	if (ob->flags & O_DESTRUCTED)
	    error("object used by map_mapping destructed"); /* amylaar */
	push_svalue(&arr->item[cnt]);

	if (extra)
	{
	    push_svalue (extra);
	    v = apply(func, ob, 2);
	}
	else 
	{
	    v = apply(func,ob,1);
	}
	if (v)
	    assign_svalue_no_free (&r->item[cnt], v);
    }
    q = slice_array(q, 0, q->size - 1);
    remove_onerror(onerror_num);
    return allocate_mapping(q, r);
}

