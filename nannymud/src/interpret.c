#include <varargs.h>
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>		/* sys/types.h and netinet/in.h are here to enable include of comm.h below */
#include <sys/stat.h>
/* #include <netinet/in.h> Included in comm.h below */
#ifdef MARK
#include <prof.h>
#endif
#include "memory.h"
#include "debug.h"
#ifdef MARK
#define CASE(x) case x-F_OFFSET: MARK(x);
#else
#define CASE(x) case x-F_OFFSET:
#endif

#include "lint.h"
#include "lang.h"
#include "exec.h"
#include "interpret.h"
#include "config.h"
#include "object.h"
#include "wiz_list.h"
#include "instrs.h"
#include "comm.h"
#include "switch.h"
#include "socket_efuns.h"
#include "socket_err.h"
#include "interpret2.h"
#ifdef RUSAGE			/* Defined in config.h */
#include <sys/resource.h>
extern int getrusage PROT((int, struct rusage *));
#ifdef sun
extern int getpagesize();
#endif
#ifndef RUSAGE_SELF
#define RUSAGE_SELF	0
#endif
#endif

#include "acl.h"
#include "acl_efuns.h"
extern int cfg_use_acls;

#if defined(__GNUC__) && !defined(lint)
#define INLINE /* inline */ /* Another time ! */
#else
#define INLINE
#endif

/* mapping prototypes */

struct vector *allocate_mapping PROT((struct vector*, struct vector*));
void free_mapping PROT((struct vector*));
struct svalue *get_map_lvalue PROT((struct vector*, struct svalue*, int));
void remove_mapping PROT((struct vector *, int));
struct vector *add_mapping PROT((struct vector*, struct vector*));
struct vector *add_array_destructively PROT((struct vector*, struct vector*));
struct vector *filter_mapping PROT((struct vector*, char*, struct object*,
				    struct svalue*));
struct vector *map_mapping PROT((struct vector*, char*, struct object*,
				 struct svalue*));

extern struct object *master_ob;

extern struct vector *get_action PROT((struct object *, char *));
extern struct vector *get_all_actions PROT((struct object *));
extern void print_svalue PROT((struct svalue *));
extern struct object *find_and_load_object();
struct svalue *sapply PROT((char *, struct object *, int));
static void do_trace PROT((char *, char *, char *));
static int apply_low PROT((char *, struct object *, int));
static int inter_sscanf PROT((int));
static int strpref PROT((char *, char *));
extern int do_rename PROT((char *, char *));     

extern int data_size PROT((struct object *));

extern struct object *previous_ob;
extern char *last_verb;
extern struct svalue const0, const1;
struct program *current_prog;
extern int current_time;
extern struct object *current_heart_beat, *current_interactive;

static int tracedepth;
#define TRACE_CALL 1
#define TRACE_CALL_OTHER 2
#define TRACE_RETURN 4
#define TRACE_ARGS 8
#define TRACE_EXEC 16
#define TRACE_HEART_BEAT 32
#define TRACE_APPLY 64
#define TRACE_OBJNAME 128
#define TRACETST(b) (command_giver->interactive->trace_level & (b))
#define TRACEP(b) \
    (command_giver && command_giver->interactive && TRACETST(b) && \
     (command_giver->interactive->trace_prefix == 0 || \
      (current_object && strpref(command_giver->interactive->trace_prefix, \
	      current_object->name))) )
#define TRACEHB (current_heart_beat == 0 || (command_giver->interactive->trace_level & TRACE_HEART_BEAT))

/*
 * Inheritance:
 * An object X can inherit from another object Y. This is done with
 * the statement 'inherit "file";'
 * The inherit statement will clone a copy of that file, call reset
 * in it, and set a pointer to Y from X.
 * Y has to be removed from the linked list of all objects.
 * All variables declared by Y will be copied to X, so that X has access
 * to them.
 *
 * If Y isn't loaded when it is needed, X will be discarded, and Y will be
 * loaded separetly. X will then be reloaded again.
 */
extern int d_flag;

extern int current_line, eval_cost;

INLINE void pop_n_elems(int);
void check_for_destr(struct vector *v);

/*
 * These are the registers used at runtime.
 * The control stack saves registers to be restored when a function
 * will return. That means that control_stack[0] will have almost no
 * interesting values, as it will terminate execution.
 */
static char *pc;		/* Program pointer. */
static struct svalue *fp;	/* Pointer to first argument. */
struct svalue *sp;	/* Points to value of last push. */
static int function_index_offset; /* Needed for inheritance */
static int variable_index_offset; /* Needed for inheritance */

struct svalue start_of_stack[EVALUATOR_STACK_SIZE];
struct svalue catch_value;	/* Used to throw an error to a catch */

static struct control_stack control_stack[MAX_TRACE];
static struct control_stack *csp;	/* Points to last element pushed */

#ifdef F__CACHE_STATS
unsigned int apply_low_call_others = 0;
unsigned int apply_low_cache_hits = 0;
unsigned int apply_low_slots_used = 0;
unsigned int apply_low_collisions = 0;
#endif


/*
 * Information about assignments of values:
 *
 * There are three types of l-values: Local variables, global variables
 * and vector elements.
 *
 * The local variables are allocated on the stack together with the arguments.
 * the register 'frame_pointer' points to the first argument.
 *
 * The global variables must keep their values between executions, and
 * have space allocated at the creation of the object.
 *
 * Elements in vectors are similar to global variables. There is a reference
 * count to the whole vector, that states when to deallocate the vector.
 * The elements consists of 'struct svalue's, and will thus have to be freed
 * immediately when over written.
 */
 
/*
 * Push an object pointer on the stack. Note that the reference count is
 * incremented.
 * A destructed object must never be pushed onto the stack.
 */
INLINE
void push_object(ob)
    struct object *ob;
{
  sp++;
  if (sp == &start_of_stack[EVALUATOR_STACK_SIZE])
    fatal("stack overflow\n");
  if(ob && !(ob->flags & O_DESTRUCTED))
  {
    sp->type = T_OBJECT;
    sp->u.ob = ob;
    add_ref(ob, "push_object");
  }else{
    sp->type=T_NUMBER;
    sp->u.number=0;
  }
}

/*
 * Push a number on the value stack.
 */
INLINE
void push_number(n)
    int n;
{
    sp++;
    if (sp == &start_of_stack[EVALUATOR_STACK_SIZE])
	fatal("stack overflow\n");
    sp->type = T_NUMBER;
    sp->u.number = n;
}

/*
 * Push a string on the value stack.
 */
INLINE
void push_string(p, type)
    char *p;
    int type;
{
    sp++;
    if (sp == &start_of_stack[EVALUATOR_STACK_SIZE])
	fatal("stack overflow\n");
    sp->type = T_STRING;
    sp->string_type = type;
    switch(type) {
    case STRING_MALLOC:
	sp->u.string = string_copy(p);
	break;
    case STRING_SHARED:
	sp->u.string = make_shared_string(p);
	break;
    case STRING_CONSTANT:
	sp->u.string = p;
	break;
    }
}

/*
 * Get address to a valid global variable.
 */
static INLINE struct svalue *find_value(num)
    int num;
{
#ifdef DEBUG
    if (num >= current_object->prog->num_variables) {
	fatal("Illegal variable access %d(%d). See trace above.\n",
	    num, current_object->prog->num_variables);
    }
#endif
    return &current_object->variables[num];
}

/*
 * Frees a index_type, as index_types are allocated with xalloc that's
 * not too hard.../Wing 920401
 */
static INLINE void free_index_type( struct index_type *ptr )
{
    free( ptr );
}

/*
 * Free the data that an svalue is pointing to. Not the svalue
 * itself.
 */
void free_svalue(v)
    struct svalue *v;
{
    switch(v->type) {
    case T_STRING:
	switch(v->string_type) {
	case STRING_MALLOC:
	    free(v->u.string);
	    break;
	case STRING_SHARED:
	    free_string(v->u.string);
	    break;
	}
	break;
    case T_OBJECT:
	free_object(v->u.ob, "free_svalue");
	break;
    case T_POINTER:
	free_vector(v->u.vec);
	break;
    case T_MAPPING:
	free_mapping(v->u.vec);
	break;
    case T_INDEX_TYPE:
	free_index_type(v->u.ind);
	break;
    }
    *v = const0; /* marion - clear this value all away */
}

/* #ifndef COMPAT_MODE */
/*
 * Prepend a slash in front of a string.
 */
static INLINE char *add_slash(str)
    char *str;
{
    char *tmp;

    tmp = xalloc(strlen(str)+2);
    strcpy(tmp,"/"); strcat(tmp,str);
    return tmp;
}
/* #endif */

/*
 * Assign to a svalue.
 * This is done either when element in vector, or when to an identifier
 * (as all identifiers are kept in a vector pointed to by the object).
 */

INLINE void assign_svalue_no_free(to, from)
    struct svalue *to;
    struct svalue *from;
{
#ifdef DEBUG
    if (from == 0)
	fatal("Null pointer to assign_svalue().\n");
#endif
    *to = *from;
    switch(from->type) {
    case T_STRING:
	switch(from->string_type) {
	case STRING_MALLOC:	/* No idea to make the string shared */
	    to->u.string = string_copy(from->u.string);
	    break;
	case STRING_CONSTANT:	/* Good idea to make it shared */
	    to->string_type = STRING_SHARED;
	    to->u.string = make_shared_string(from->u.string);
	    break;
	case STRING_SHARED:	/* It already is shared */
	    to->u.string = copy_shared_string(from->u.string);
	    break;
	default:
	    fatal("Bad string type %d\n", from->string_type);
	}
	break;
    case T_OBJECT:
	add_ref(to->u.ob, "ass to var");
	break;
    case T_POINTER:
    case T_MAPPING:
	to->u.vec->ref++;
	break;
    }
}

INLINE void assign_svalue(dest, v)
    struct svalue *dest;
    struct svalue *v;
{
    /* First deallocate the previous value. */
    free_svalue(dest);
    assign_svalue_no_free(dest, v);
}

INLINE void check_svalue_for_destr(struct svalue *s)
{
  if(s->type == T_OBJECT && (s->u.ob->flags & O_DESTRUCTED))
  {
    free_svalue(s);
    *s=const0;
  }
}

INLINE
void push_svalue(v)
    struct svalue *v;
{
  sp++;
  assign_svalue_no_free(sp, v);
  check_svalue_for_destr(sp);
}

/*
 * Pop the top-most value of the stack.
 * Don't do this if it is a value that will be used afterwards, as the
 * data may be sent to free(), and destroyed.
 */
INLINE void pop_stack() {
#ifdef DEBUG
    if (sp < start_of_stack)
	fatal("Stack underflow.\n");
#endif
    free_svalue(sp);
    sp--;
}

/*
 * Push a pointer to a vector on the stack. Note that the reference count
 * is incremented. Newly created vectors normally have a reference count
 * initialized to 1.
 */
INLINE void push_vector(v)
    struct vector *v;
{
    v->ref++;
    sp++;
    sp->type = T_POINTER;
    sp->u.vec = v;
}

INLINE void simple_push_vector(struct vector *v)
{
  if(!v)
  {
    push_number(0);
  }else{
    sp++;
    sp->type=T_POINTER;
    sp->u.vec=v;
  }
}


INLINE void push_mapping(v)
    struct vector *v;
{
    v->ref++;
    sp++;
    sp->type = T_MAPPING;
    sp->u.vec = v;
}

/*
 * Push a string on the stack that is already malloced.
 */
INLINE void push_malloced_string(char *p)
{
    sp++;
    sp->type = T_STRING;
    sp->u.string = p;
    sp->string_type = STRING_MALLOC;
}

/*
 * Push a string on the stack that is already constant.
 */
INLINE void push_constant_string(char *p)
{
    sp++;
    sp->type = T_STRING;
    sp->u.string = p;
    sp->string_type = STRING_CONSTANT;
}

/*
 * This function pushes a element from an indexed lvalue or svalue
 * on the stack /Wing 920324
 */
static INLINE void push_indexed_svalue()
{
    struct svalue *i, *vec, *item;
    int ind;

    i = sp;
    vec = sp - 1;
    if( vec->type == T_LVALUE )
	assign_svalue_no_free( vec, vec->u.lvalue );
    if (vec->type != T_MAPPING) {
      if (i->type != T_NUMBER/* || i->u.number < 0*/)
	    error("Illegal index\n");
	ind = i->u.number;
    }
    if (vec->type == T_INDEX_TYPE)
    {
	struct index_type *ptr = vec->u.ind;

	if( ptr->type == T_NUMBER )
	  error("Illegal argument to push_indexed_svalue.\n");
	if (ind < 0)
	  ind = ptr->to + ind;
	else
	  ind = ind + ptr->from;
	if( ind > ptr->to || ind < ptr->from )
	    error("Index out of bound in push_index_svalue.\n");
	vec = ptr->ptr;
    }
    switch (vec->type) {
    case T_STRING: {
	/* changed by Wing */
	int number;
	pop_stack();

        if(ind < 0)
           ind = strlen(vec->u.string) + ind;
	if(ind >= strlen(vec->u.string) || ind < 0)
	  number = 0;
	else
	  number = vec->u.string[ind];
	free_svalue(sp);
	sp->type = T_NUMBER;
	sp->u.number = number;
	break;}
    case T_POINTER:
	pop_stack();
        if(ind < 0)
           ind = vec->u.vec->size + ind;
	if (ind >= vec->u.vec->size || ind<0)
	  error ("Index out of bounds\n");
	item = &vec->u.vec->item[ind];
	if (vec->u.vec->ref == 1) {
	    static struct svalue quickfix = { T_NUMBER };
	    /* marion says: but this is crude too */
	    /* marion blushes. */
	    assign_svalue (&quickfix, item);
	    item = &quickfix;
	}
        assign_svalue( sp, item );/* wing */
	break;
    case T_MAPPING:
	item = get_map_lvalue(vec->u.vec, i, 0); /* doesn't need lvalue */
	pop_stack();
	if (vec->u.vec->ref == 1) {
	    static struct svalue quickfix = { T_NUMBER };
	    assign_svalue (&quickfix, item);
	    item = &quickfix;
	}
	free_svalue(sp);
        assign_svalue_no_free( sp, item );
	break;
    default:
	error("Indexing on illegal type.\n");
	break;
    }
}


/*
 * This function puts an element from an indexed lvalue or index_type
 * and pushes it on the stack in form of a lvalue or a index_type (if the
 * element's type is T_STRING) /Wing 920325
 */
static INLINE void push_indexed_lvalue()
{
    struct svalue *i, *vec, *item;
    int ind;

    i = sp;
    vec = sp - 1;
    if( vec->type == T_LVALUE )
      if (vec->u.lvalue->type == T_STRING)
	vec = vec->u.lvalue;
      else
	assign_svalue_no_free( vec, vec->u.lvalue );
    else if (vec->type == T_STRING)
      error( "In push_indexed_lvalue: Constant strings cannot be lvalues." );
    if (vec->type != T_MAPPING) {
      if (i->type != T_NUMBER/* || i->u.number < 0*/)
	    error("Illegal index\n");
	ind = i->u.number;
    }
    switch (vec->type) {
     case T_INDEX_TYPE: {
	struct index_type *ptr;
	ptr = vec->u.ind;
	if (i->type != T_NUMBER/* || i->u.number < 0*/)
	    error("Illegal index\n");
	ind = i->u.number;
	
	switch( ptr->type )
	{
	 case T_STRING :
	  {
	    if (ind < 0)
	      ind += ptr->to;
	    else
	      ind += ptr->from;
	    if( ind < ptr->from || ind > ptr->to)
	      error("Index out of bounds in push_indexed_lvalue.\n");
	    ptr->from = ind;
	    ptr->to = -1;
	    ptr->type = T_NUMBER;
	    pop_stack();
	    break;
	  }
	 case T_POINTER :
	  pop_stack();
	  if (ind < 0)
	    ind += ptr->to;
	  else
	    ind += ptr->from;
	  if( ind < ptr->from || ind > ptr->to)
	    error ("Index out of bounds\n");
	  item = &ptr->ptr->u.vec->item[ind];
#ifdef IDI
	  if (ptr->ptr->u.vec->ref == 1) {
	    static struct svalue quickfix = { T_NUMBER };
	    /* marion says: but this is crude too */
	    /* marion blushes. */
	    assign_svalue (&quickfix, item);
	    item = &quickfix;
	    }
#endif /* Undrar om detta funkar...*ponder* */
	  free_svalue(sp);
	  sp->type = T_LVALUE;
	  sp->u.lvalue = item;
	  break;
	 default:
	  error("Bad type to push_indexed_lvalue.\n");
	}
	return; }
    case T_STRING: {
	struct index_type *ptr;
	ptr = (struct index_type *)xalloc( sizeof( struct index_type ) );
	pop_stack();
	if (ind < 0)
	  ind += strlen(vec->u.string);
	if (ind < 0 || ind > strlen(vec->u.string))
	    error("Index out of bounds.\n");
	ptr->from = ind;
	ptr->to = -1;
	ptr->ptr = vec;
	ptr->type = T_NUMBER;
	free_svalue(sp);
	sp->type = T_INDEX_TYPE;
	sp->u.ind = ptr;
	break;}
    case T_POINTER:
	pop_stack();
        if(ind < 0)
           ind = vec->u.vec->size + ind;
	if(ind >= vec->u.vec->size || ind < 0)
           error ("Index out of bounds\n");
	item = &vec->u.vec->item[ind];
	 vec->u.vec->flags &=~ VECTOR_CLEAN;
	if (vec->u.vec->ref == 1) {
	    static struct svalue quickfix = { T_NUMBER };
	    /* marion says: but this is crude too */
	    /* marion blushes. */
	    assign_svalue (&quickfix, item);
	    item = &quickfix;
	}
	free_svalue(sp);
	sp->type = T_LVALUE;
	sp->u.lvalue = item;
	break;
    case T_MAPPING:
        item = get_map_lvalue(vec->u.vec, i, 1);
        pop_stack();
	if (vec->u.vec->ref == 1) {
	    static struct svalue quickfix = { T_NUMBER };
	    assign_svalue (&quickfix, item);
	    item = &quickfix;
	}
	free_svalue(sp);
	sp->type = T_LVALUE;
	sp->u.lvalue = item;
        break;
    default:
	error("Indexing on illegal type.\n");
	break;
    }
}

/*
 * This function fetches the value from a index_type and returns it as
 * a svalue. /Wing 920401
 */
static INLINE struct svalue *get_index_type_value( struct index_type *ptr )
{
  static struct svalue result = { T_NUMBER, 0, 0 };

    free_svalue( &result );
    switch( result.type = ptr->type )
    {
	case T_NUMBER :
	    result.u.number = ptr->ptr->u.string[ ptr->from ];
	    break;
	case T_STRING :
	    result.string_type = STRING_MALLOC;
	    if( ptr->to == -1 )
	    {
		result.u.string = xalloc( 1 );
		result.u.string[0] = '\0';
		break;
	    }
	    result.u.string = xalloc( ptr->to - ptr->from + 2 );
	    memcpy( result.u.string, ptr->ptr->u.string + ptr->from, ptr->to - ptr->from + 1 );
	    result.u.string[ ptr->to - ptr->from + 1 ] = 0;
	    break;
	case T_POINTER :
	{
	    struct vector *v;

	    v = slice_array(ptr->ptr->u.vec, ptr->from, ptr->to);
	    if (v) {
		result.type = T_POINTER;
		result.u.vec = v;
#if 0	
		/* snyggt hackat, inte skoj att tappa referenser {r det
		 * /Prof
		 */
		v->ref--;	/* Will make ref count == 1 */
#endif
	    } else {
		result.type = T_NUMBER;
		result.u.number = 0; }
	    break;
	}
	default :
	    error("Wrong type to get_index_type_value\n");
    }
    return &result;
}

/*
 * A little macro that counts and corrects limits for a range. If from is less
 * than zero from is corrected to zero. If to is less than from to is set to -1
 * which indicates that the range is an element between from and from -1 (in
 * front of the range if from = 0, behind the range if from == length). This
 * makes it possible to insert elements in a string/array using the range
 * operator (size is never an issue when you assign something to a range.)
 * /Wing 920401
 */
#define COUNT_TO_FROM( to, from, length ) \
    if (from < 0) from = 0; \
    if (from >= length) { from = length; to = -1; } \
    else { if (to < from) to = -1; if (to >= length) to = length - 1; }

/*
 * This function pushes the range value of a svalue, lvalue or an index_type
 * on the stack /Wing 920324
 */
static INLINE void push_range_svalue()
{
  struct svalue *ptr;
  int free_ptrp = 0;
    
  if (sp[-1].type != T_NUMBER)
    error("Bad type of start interval to [ .. ] range.\n");

  if (sp[0].type != T_NUMBER)
    error("Bad type of end interval to [ .. ] range.\n");

  if (sp[-2].type == T_INDEX_TYPE)
  {
    if(  sp[-2].u.ind->type == T_NUMBER )
      error("Wrong type to push_range_svalue.\n");
    ptr = get_index_type_value( sp[-2].u.ind );
  }
  else
  {
    ptr = (sp[-2].type == T_LVALUE) ? sp[-2].u.lvalue : &sp[-2];
  }

  if (ptr->type == T_POINTER) {
    struct vector *v;

    COUNT_TO_FROM( sp[0].u.number, sp[-1].u.number, ptr->u.vec->size )

      if (sp[0].u.number >= ptr->u.vec->size)
	error("Interval is larger than size or the array in [ .. ] range.\n");
    v = slice_array(ptr->u.vec, sp[-1].u.number, sp[0].u.number);
    pop_n_elems(3);
    simple_push_vector(v);
  } else if (ptr->type == T_STRING) {
    int len, from, to;
    char *res;
	
    len = strlen(ptr->u.string);
    from = sp[-1].u.number;
    to = sp[0].u.number;
    COUNT_TO_FROM( to, from, len )
      if (to == -1)
      {
	pop_n_elems(3);
	res = xalloc( 1 );
	*res = '\0';
	push_malloced_string(res);
      }
      else if (to == len-1) {
	res = string_copy(ptr->u.string + from);
	pop_n_elems(3);
	push_malloced_string(res);
      }
      else
      {
	res = xalloc(to - from + 2);
	strncpy(res, ptr->u.string + from, to - from + 1);
	res[to - from + 1] = '\0';
	pop_n_elems(3);
	push_malloced_string(res);
      }
  } else {
    error("Bad argument to [ .. ] range operand.\n");
  }
  if( free_ptrp )
    free_svalue( ptr );
}

/*
 * This function pushes an range in form of a index_type made from a lvalue
 * or another index_type on the stack. If the indexes are illegal it makes
 * them legal. /Wing 920401
 */
static INLINE void push_range_lvalue()
{
    struct svalue *vec;
    struct index_type *ptr;
    if (sp[-1].type != T_NUMBER)
	error("Bad type of start interval to [ .. ] range.\n");
    if (sp[0].type != T_NUMBER)
	error("Bad type of end interval to [ .. ] range.\n");

    if (sp[-2].type == T_INDEX_TYPE)
    {
	int tmp;

	ptr = sp[-2].u.ind;
	if (ptr->type == T_NUMBER)
	    error("Bad argument to push_range_lvalue\n");
	COUNT_TO_FROM( sp[0].u.number, sp[-1].u.number, ptr->to - ptr->from + 1 )
	tmp = (sp[0].u.number == -1) ? -1 : ptr->from + sp[0].u.number;
	ptr->from += sp[-1].u.number;
	ptr->to = tmp > ptr->to ? ptr->to : tmp;
	pop_n_elems(2);
	return;
    }
    if (sp[-2].type != T_LVALUE)
    {
	push_range_svalue();
	return;
    }
    ptr = (struct index_type *)xalloc( sizeof( struct index_type ) );
    vec = sp[-2].u.lvalue;
    switch( vec->type )
    {
	case T_POINTER :
	    ptr->type = T_POINTER;
	    ptr->from = sp[-1].u.number;
	    ptr->to = sp[0].u.number;
	    COUNT_TO_FROM( ptr->to, ptr->from, vec->u.vec->size )
	    ptr->ptr = vec;
	    pop_n_elems(2);
	    free_svalue(sp);
	    sp->type = T_INDEX_TYPE;
	    sp->u.ind = ptr;
	    break;
	case T_STRING :
	    ptr->from = sp[-1].u.number;
	    ptr->to = sp[0].u.number;
	    ptr->type = T_STRING;
	    COUNT_TO_FROM( ptr->to, ptr->from, strlen( vec->u.string ) )
	    ptr->ptr = vec;
	    pop_n_elems(2);
	    free_svalue(sp);
	    sp->type = T_INDEX_TYPE;
	    sp->u.ind = ptr;
	    break;
        default :
	    error("Bad argument to [ .. ] range operand.\n");
	}
}

/*
 * This functiomn assigns the value in value to an index_type at
 * sp - 1. Then it removes two topmost stack levels (maybe not too
 * fo a primitive, but every function that uses it would have to
 * do it otherwise). Finally it pushes value on the stack.
 * /Wing 920324
 * Bleach! There are more errors than characters in these functions...
 */
static INLINE void assign_index_type( struct svalue *value )
{
    struct index_type *ptr = sp[-1].u.ind;
    char *str;
    
    if( sp[-1].type != T_INDEX_TYPE )
	error("Bad argument to assign_index_type.\n");
    switch( ptr->type )
    {
	case T_NUMBER :
	    if( value->type != T_NUMBER )
		error("Wrong type to assign_index_type.\n");
	    switch( ptr->ptr->string_type )
	    {
		case STRING_CONSTANT :
		    str = string_copy( ptr->ptr->u.string );
		    ptr->ptr->string_type = STRING_MALLOC;
		    ptr->ptr->u.string = str;
		    /* FALL THROUGH */

		case STRING_MALLOC :
		    ptr->ptr->u.string[ ptr->from ] = value->u.number;
		    break;

		case STRING_SHARED :
		    str = string_copy( ptr->ptr->u.string );
		    str[ ptr->from ] = value->u.number;
		    free_string( ptr->ptr->u.string );
		    ptr->ptr->string_type = STRING_MALLOC;
		    ptr->ptr->u.string = str;
	    }
	    assign_svalue( sp-1, value );
	    pop_stack();
	    break;

	case T_STRING :
	    if(value->type != T_STRING)
	       error("Wrong type to assign_index_type.\n");
	    if(ptr->to == -1)
	    {  /* Insert value->u.string at position ptr->from */
	       str = xalloc(strlen(ptr->ptr->u.string) +
			    strlen(value->u.string) + 1);
	    }
	    else
	    {  /* Replace range ptr->from, ptr->to with value->u.string */
	       str = xalloc(strlen(ptr->ptr->u.string) +
			    strlen(value->u.string) + ptr->from - ptr->to);
	    }
	    if(ptr->from > 0)
	       strncpy(str, ptr->ptr->u.string, ptr->from);
	    strcpy(str + ptr->from, value->u.string);
	    if(ptr->to == -1)
	       strcat(str, ptr->ptr->u.string + ptr->from);
	    else
	       strcat(str, ptr->ptr->u.string + ptr->to + 1);
	    switch( ptr->ptr->string_type )
	    {
	       case STRING_CONSTANT:
		  ptr->ptr->string_type = STRING_MALLOC;
		  ptr->ptr->u.string = str;
		  break;
	       case STRING_MALLOC :
		  free(ptr->ptr->u.string);
		  ptr->ptr->u.string = str;
		  break;
	       case STRING_SHARED :
		  free_string(ptr->ptr->u.string);
		  ptr->ptr->u.string = make_shared_string(str);
		  free(str);
		  break;
	    }
	    assign_svalue(sp - 1, value);
	    pop_stack();
	    break;

	case T_POINTER :
	{
	    struct vector *v;

	    if(value->type != T_POINTER)
	       error("Wrong type to assign_index_type.\n");

	    check_for_destr(ptr->ptr->u.vec);
	    check_for_destr(value->u.vec);

	    /*  Make orig[0..from]+value  */
	    if( ptr->from )
	    {
	        struct vector *lv;
		lv = slice_array(ptr->ptr->u.vec, 0, ptr->from - 1);
		v = add_array(lv, value->u.vec);
		free_vector(lv);
	    }
	    else
	    {
		v = value->u.vec;
		v->ref++;  /* copy array (primitives?) */
	    }

	    /* Add rest */
	    if( ptr->to == -1 )
	    {
	      if( ptr->from < ptr->ptr->u.vec->size )
	      {
		struct vector *tmp, *lv;
		tmp = slice_array(ptr->ptr->u.vec, ptr->from, ptr->ptr->u.vec->size - 1);
		lv = add_array(v, tmp);
		free_vector(v);
		free_vector(tmp);
		v = lv;
	      }
	    }
	    else
	    {
	      if( ptr->to < ptr->ptr->u.vec->size - 1 )
	      {
		struct vector *tmp, *lv;
		tmp=slice_array(ptr->ptr->u.vec, ptr->to + 1, ptr->ptr->u.vec->size - 1);
		lv = add_array(v,tmp);
		free_vector(v);
		free_vector(tmp);
		v = lv;
	      }
	    }
	    free_vector(ptr->ptr->u.vec);
	    ptr->ptr->u.vec = v;
	    assign_svalue( sp-1, value );
	    pop_stack();
	    break;
	}
	default:
	    error("Wrong type to assign_index_type.\n");
    }
}

#ifdef OPCPROF
#define MAXOPC 512
static int opcount[MAXOPC];
#endif

/*
 * Deallocate 'n' values from the stack.
 */
INLINE void pop_n_elems(int n)
{
#ifdef DEBUG
    if (n < 0)
	fatal("pop_n_elems: %d elements.\n", n);
#endif
    for (; n>0; n--)
	pop_stack();
}

void bad_arg(arg, instr)
    int arg, instr;
{
    error("Bad argument %d to %s()\n", arg, get_f_name(instr));
}

INLINE
static void push_control_stack(funp)
    struct function *funp;
{
    if (csp == &control_stack[MAX_TRACE-1])
	error("Too deep recursion.\n");
    csp++;
    csp->funp = funp;	/* Only used for tracebacks */
    csp->ob = current_object;
    csp->prev_ob = previous_ob;
    csp->fp = fp;
    csp->prog = current_prog;
    /* csp->extern_call = 0; It is set by eval_instruction() */
    csp->pc = pc;
    csp->function_index_offset = function_index_offset;
    csp->variable_index_offset = variable_index_offset;
}

/*
 * Pop the control stack one element, and restore registers.
 * extern_call must not be modified here, as it is used imediately after pop.
 */
static void pop_control_stack() {
#ifdef DEBUG
    if (csp == control_stack - 1)
	fatal("Popped out of the control stack");
#endif
    current_object = csp->ob;
    current_prog = csp->prog;
    previous_ob = csp->prev_ob;
    pc = csp->pc;
    fp = csp->fp;
    function_index_offset = csp->function_index_offset;
    variable_index_offset = csp->variable_index_offset;
    csp--;
}


static void do_trace_call(funp)
    struct function *funp;
{
    do_trace("Call direct ", funp->name, " ");
    if (TRACEHB) {
        if (TRACETST(TRACE_ARGS)) {
            int i;
            add_message(" with %d arguments: ", funp->num_arg);
            for(i = funp->num_arg-1; i >= 0; i--) {
                print_svalue(&sp[-i]);
                add_message(" ");
            }
        }
        add_message("\n");
    }
}

/*
 * Argument is the function to execute. If it is defined by inheritance,
 * then search for the real definition, and return it.
 * There is a number of arguments on the stack. Normalize them and initialize
 * local variables, so that the called function is pleased.
 */
static struct function *setup_new_frame(funp)
    struct function *funp;
{
    function_index_offset = 0;
    variable_index_offset = 0;
    while(funp->flags & NAME_INHERITED)
    {
      function_index_offset +=
	current_prog->inherit[funp->inherit_offset].function_index_offset;
      variable_index_offset +=
	current_prog->inherit[funp->inherit_offset].variable_index_offset;
      current_prog =
	current_prog->inherit[funp->inherit_offset].prog;
      funp = &current_prog->functions[funp->offset];
    }
    /* Remove excessive arguments */
    while(csp->num_local_variables > funp->num_arg) {
	pop_stack();
	csp->num_local_variables--;
    }
    /* Correct number of arguments and local variables */
    while(csp->num_local_variables < funp->num_arg + funp->num_local) {
	push_number(0);
	csp->num_local_variables++;
    }
    tracedepth++;
    if (TRACEP(TRACE_CALL)) {
	do_trace_call(funp);
    }
    fp = sp - csp->num_local_variables + 1;
    return funp;
}

static void break_point()
{
    if (sp - fp - csp->num_local_variables + 1 != 0)
	fatal("Bad stack pointer.\n");
}

/* marion
 * maintain a small and inefficient stack of error recovery context
 * data structures.
 * This routine is called in three different ways:
 * push=-1	Pop the stack.
 * push=1	push the stack.
 * push=0	No error occured, so the pushed value does not have to be
 *		restored. The pushed value can simply be popped into the void.
 *
 * The stack is implemented as a linked list of stack-objects, allocated
 * from the heap, and deallocated when popped.
 */
static void push_pop_error_context (push)
    int push;
{
    extern jmp_buf error_recovery_context;
    extern int error_recovery_context_exists;
    extern int onerror_level;
    extern int n_onerrhandlers;


    static struct error_context_stack {
	jmp_buf old_error_context;
	int old_exists_flag;
	int old_onerror_level;
	struct control_stack *save_csp;
	struct object *save_command_giver;
	struct svalue *save_sp;
	struct error_context_stack *next;
    } *ecsp = 0, *p;

    if (push == 1) {
	/*
	 * Save some global variables that must be restored separately
	 * after a longjmp. The stack will have to be manually popped all
	 * the way.
	 */
	p = (struct error_context_stack *)xalloc (sizeof *p);
	p->save_sp = sp;
	p->save_csp = csp;	
	p->save_command_giver = command_giver;
	p->old_onerror_level=onerror_level;
	onerror_level=n_onerrhandlers;
	memcpy (
	    (char *)p->old_error_context,
	    (char *)error_recovery_context,
	    sizeof error_recovery_context);
	p->old_exists_flag = error_recovery_context_exists;
	p->next = ecsp;
	ecsp = p;
    } else {
	p = ecsp;
	if (p == 0)
	    fatal("Catch: error context stack underflow");
	if (push == 0) {
#ifdef DEBUG
	    if (csp != p->save_csp-1)
		fatal("Catch: Lost track of csp");
#if 0
	    /*
	     * This test is not valid! The statement catch(exec("...")) will
	     * change the value of command_giver.
	     */
	    if (command_giver != p->save_command_giver)
		fatal("Catch: Lost track of command_giver");
#endif
#endif
	} else {
	    /* push == -1 !
	     * They did a throw() or error. That means that the control
	     * stack must be restored manually here.
	     */
	    csp = p->save_csp;	
	    pop_n_elems (sp - p->save_sp);
	    command_giver = p->save_command_giver;
	}
	memcpy ((char *)error_recovery_context,
		(char *)p->old_error_context,
		sizeof error_recovery_context);
	error_recovery_context_exists = p->old_exists_flag;
	onerror_level=p->old_onerror_level;
	ecsp = p->next;
	free ((char *)p);
    }
}

/*
 * When a vector is given as argument to an efun, all items has to be
 * checked if there would be an destructed object.
 * A bad problem currently is that a vector can contain another vector, so this
 * should be tested too. But, there is currently no prevention against
 * recursive vectors, which means that this can not be tested. Thus, the game
 * may crash if a vector contains a vector that contains a destructed object
 * and this top-most vector is used as an argument to an efun.
 */
/* The game won't crash when doing simple operations like assign_svalue
 * on a destructed object. You have to watch out, of course, that you don't
 * apply a function to it.
 * to save space it is preferable that destructed objects are freed soon.
 *   amylaar
 */
void check_for_destr(v)
    struct vector *v;
{
  int i;
  int clean=VECTOR_CLEAN;

  if(v->flags & VECTOR_CLEAN) return;

  for (i=0; i < v->size; i++)
  {
    if (v->item[i].type != T_OBJECT)
      continue;
    if (!(v->item[i].u.ob->flags & O_DESTRUCTED))
    {
      clean=0;
      continue;
    }
    assign_svalue(&v->item[i], &const0);
  }

  v->flags= (v->flags & ~ VECTOR_CLEAN) | clean;
}

void check_map_for_destr(m)
    struct vector *m;
{
  int i, clean;
  struct vector *v;

  v = m->item[0].u.vec;
  if(!(v->flags & VECTOR_CLEAN))
  {
    clean = VECTOR_CLEAN;
    for (i=0; i < v->size; )
    {
      if (v->item[i].type == T_OBJECT)
      {
	if((v->item[i].u.ob->flags & O_DESTRUCTED))
	{
	  remove_mapping(m, i);
	  v = m->item[0].u.vec;
	} else {
	  i++;
	  clean=0;
	}
      }else{
	i++;
      }
    }
    v->flags= (v->flags & ~ VECTOR_CLEAN) | clean;
  }
  check_for_destr(m->item[1].u.vec);
}


static int is_eq(struct svalue *a, struct svalue *b)
{
  if(a->type != b->type) return 0;
  switch(a->type)
  {
  case T_NUMBER:
    return a->u.number == b->u.number;
  case T_POINTER:
    return a->u.vec == b->u.vec;
  case T_STRING:
    if(a->string_type == STRING_SHARED && b->string_type==STRING_SHARED)
      return a->u.string==b->u.string;
    else
      return strcmp(a->u.string, b->u.string) == 0;
  case T_MAPPING:
    return a->u.vec == b->u.vec ||
      (a->u.vec->item[0].u.vec->size == 0 &&
       b->u.vec->item[0].u.vec->size == 0);
  case T_OBJECT:
    return a->u.ob == b->u.ob;

  default:
    error("Comparing unknown types (shouldn't happen).\n");
  }
}

static int is_lt(struct svalue *a, struct svalue *b)
{
  if(a->type != b->type)
    error("Comparing different types.\n");

  switch(a->type)
  {
  case T_NUMBER:
    return a->u.number < b->u.number;
  case T_STRING:
    return strcmp(a->u.string, b->u.string) < 0;
  default:
    error("Bad argument 1 to comparison.\n");
  }
}

#define is_gt(X,Y)   is_lt((Y),(X))

static char *add_strings(char *a,char *b)
{
  char *ret;
  int alen,blen;
  alen=strlen(a);
  blen=strlen(b);
  ret=xalloc(alen+blen+1);
  memcpy(ret,a,alen);
  memcpy(ret+alen,b,blen);
  ret[alen+blen]=0;
  return ret;
}

static INLINE void do_jump()
{
  short offset;

  ((char *)&offset)[0] = pc[0];
  ((char *)&offset)[1] = pc[1];
  pc += offset;

#ifdef DEBUG
  if(pc < current_prog->program ||
     pc >= current_prog->program + current_prog->program_size)
  {
     fatal("Bad jump.\n");
  }
#endif
}

extern char driver_version[];

/*
 * Evaluate instructions at address 'p'. All program offsets are
 * to current_prog->program. 'current_prog' must be setup before
 * call of this function.
 *
 * There must not be destructed objects on the stack. The destruct_object()
 * function will automatically remove all occurences. The effect is that
 * all called efuns knows that they won't have destructed objects as
 * arguments.
 */
#ifdef TRACE_CODE
#define TRACE_LENGTH 64

int previous_instruction[TRACE_LENGTH];
int stack_size[TRACE_LENGTH];
char *previous_pc[TRACE_LENGTH];
static unsigned int last;
#endif

static void eval_instruction(char *p)
{
  struct object *ob;
  int i, num_arg;
  int instruction;
  struct svalue *expected_stack, *argp;
  
  /* Next F_RETURN at this level will return out of eval_instruction() */
  csp->extern_call = 1;
  pc = p;
  while(1)
  {
#ifdef MALLOC_DEBUG
    verify_sfltable();
#endif
    instruction = EXTRACT_UCHAR(pc);
#ifdef TRACE_CODE
    previous_instruction[last] = instruction + F_OFFSET;
    previous_pc[last] = pc;
    stack_size[last] = sp - fp - csp->num_local_variables;
    last = (last + 1) % TRACE_LENGTH;
#endif
    pc++;
    if (++eval_cost > MAX_COST)
    {
      printf("eval_cost too big %d\n", eval_cost);
      /* eval_cost = 0; /gahhhhhhhh /Profezzorn */
      error("Too long evaluation. Execution aborted.\n");
    }
    /*
     * Execute current instruction. Note that all functions callable
     * from LPC must return a value. This does not apply to control
     * instructions, like F_JUMP.
     */
    if (instrs[instruction].min_arg != instrs[instruction].max_arg)
    {
      num_arg = EXTRACT_UCHAR(pc);
      pc++;
      if (num_arg > 0)
      {
	if ((instrs[instruction].type[0] & (sp-num_arg+1)->type) == 0)
	  bad_arg(1, instruction + F_OFFSET);

	if (num_arg > 1)
	{
	  if ((instrs[instruction].type[1] & (sp-num_arg+2)->type) == 0)
	    bad_arg(2, instruction + F_OFFSET);
	}
      }
#ifdef DEBUG
      expected_stack = sp - num_arg +1;
#endif
    } else {
      num_arg = instrs[instruction].min_arg;
      if (num_arg > 0)
      {
	if ((instrs[instruction].type[0] & (sp-num_arg+1)->type) == 0)
	  bad_arg(1, instruction + F_OFFSET);

	if (num_arg > 1)
	{
	  if ((instrs[instruction].type[1] & (sp-num_arg+2)->type) == 0)
	    bad_arg(2, instruction + F_OFFSET);
	}
      }
      /*
       * Safety measure. It is supposed that the evaluator knows
       * the number of arguments.
       */
      num_arg = -1;
#ifdef DEBUG
      expected_stack=0;
#endif
    }

#ifdef OPCPROF
    if (instruction+F_OFFSET >= 0 && instruction+F_OFFSET < MAXOPC)
      opcount[instruction+F_OFFSET]++;
#endif
#if 0 /* Optimized, Profezzorn */
    if (TRACEP(TRACE_EXEC)) {
      do_trace("Exec ", get_f_name(instruction+F_OFFSET), "\n");
    }
#endif
    /*
     * Execute the instructions. The number of arguments are correct,
     * and the type of the two first arguments are also correct.
     */
    switch(instruction)
    {
    default:
      fatal("Undefined instruction %s (%d)\n", get_f_name(instruction+F_OFFSET),
	    instruction+F_OFFSET);
      /*NOTREACHED*/
      
#ifdef F__FUZZYMATCH
      CASE(F__FUZZYMATCH)
      {
        extern int fuzzymatch(char *,char *);
	if(!strcmp(sp[0].u.string,sp[-1].u.string))
	{
	  i=100;
	}else{
	  i =fuzzymatch(sp[0].u.string,sp[-1].u.string);
	  i+=fuzzymatch(sp[-1].u.string,sp[0].u.string);
	  i=i*100/(strlen(sp[0].u.string)+strlen(sp[-1].u.string));
	}
        pop_stack();
        pop_stack();
        push_number(i);
	break; /* Milamber, a.k.a Per */
      }
#endif
#ifdef F__GET_SPEC_OBJ
      CASE(F__GET_SPEC_OBJ);
      {
	 struct vector *vec = get_spec_obj(sp[-1].u.number, sp[0].u.number);
	 pop_n_elems(2);
	 simple_push_vector(vec);
	 break;
      }
#endif
#ifdef F__GET_CLONE_BY_NUMBER
      CASE(F__GET_CLONE_BY_NUMBER);
         ob = f__get_clone_by_number(sp->u.number);
	 pop_stack();
	 push_object(ob);
	 break;
#endif
#ifdef F__CHMOD
      CASE(F__CHMOD);
      {
         extern int f__chmod(char *, int);
	 i = f__chmod((sp-1)->u.string, sp->u.number);
	 pop_n_elems(2);
	 push_number(i);
         break;
      }
#endif
#ifdef F__RUSAGE
      CASE(F__RUSAGE)
      {
	extern struct vector *get_rusage();
	simple_push_vector(get_rusage());
	break;
      }
#endif /* F__RUSAGE */
      
      
#ifdef F__NEXT_CLONE
      CASE(F__NEXT_CLONE); f__next_clone(); break;
#endif
#ifdef F__CACHE_STATS
      CASE(F__CACHE_STATS)
      {
	add_message ("Function cache information\n");
	add_message ("-------------------------------\n");
	add_message("%% cache hits:    %10.2f\n",
		    100 * ((double)apply_low_cache_hits / apply_low_call_others));
	add_message("call_others:     %10lu\n", apply_low_call_others);
	add_message("cache hits:      %10lu\n", apply_low_cache_hits);
	add_message("cache size:      %10lu\n", APPLY_CACHE_SIZE);
	add_message("slots used:      %10lu\n", apply_low_slots_used);
	add_message("%% slots used:    %10.2f\n",
		    100 * ((double)apply_low_slots_used / APPLY_CACHE_SIZE));
	add_message("collisions:      %10lu\n", apply_low_collisions);
	add_message("%% collisions:    %10.2f\n",
		    100 * ((double)apply_low_collisions / apply_low_call_others));
	push_number(0);
	break;
      }
#endif /* F__CACHE_STATS */
      
#ifdef F__TGETENT
      CASE(F__TGETENT)
      {
	char buff[1024];
	int  ret;
	
	ret = tgetent(buff, sp->u.string);
	pop_stack();
	if(ret)
	  push_malloced_string(string_copy(buff));
	else
	  push_number(0);
	break;
      }
#endif /* F__TGETENT */
      
#ifdef F__SET_PROMPT
      CASE(F__SET_PROMPT)
      {
	set_prompt(sp->u.string, 1);
	pop_stack();
	push_number(0);
	break;
      }
#endif /* F__SET_PROMPT */
      
#ifdef F__QUERY_ED
      CASE(F__QUERY_ED)
      {
	int i;
	
	if(sp->u.ob->interactive && sp->u.ob->interactive->ed_buffer)
	  i = 17;
	else 
	  i = 0;
	pop_stack();
	push_number(i);
	break;
      }
#endif
      
#ifdef F__DUMP_FILE_DESCRIPTORS
      
      CASE(F__DUMP_FILE_DESCRIPTORS)
      {
	dump_file_descriptors();
	push_number(0);
	break;
      }
#endif
      
#ifdef SOCKET
      CASE(F_SOCKET_CREATE); f_socket_create(num_arg); break;
      CASE(F_SOCKET_BIND); f_socket_bind(num_arg); break;
      CASE(F_SOCKET_LISTEN); f_socket_listen(num_arg); break;
      CASE(F_SOCKET_ACCEPT); f_socket_accept(num_arg); break;
      CASE(F_SOCKET_CONNECT); f_socket_connect(num_arg); break;
      CASE(F_SOCKET_WRITE); f_socket_write(num_arg); break;
      CASE(F_SOCKET_CLOSE); f_socket_close(num_arg); break;
      CASE(F_SOCKET_RELEASE); f_socket_release(num_arg); break;
      CASE(F_SOCKET_ACQUIRE); f_socket_acquire(num_arg); break;
      CASE(F_SOCKET_ERROR); f_socket_error(num_arg); break;
      CASE(F_SOCKET_ADDRESS); f_socket_address(num_arg); break;
      CASE(F_DUMP_SOCKET_STATUS); f_dump_socket_status(num_arg); break;
#endif /* SOCKET */

#ifdef F__FIND_SHORTEST_PATH
      CASE(F__FIND_SHORTEST_PATH); f__find_shortest_path(num_arg); break;
#endif
      
#ifdef F__DUMP_OBJECT
      CASE(F__DUMP_OBJECT)
      {
	dump_object(sp->u.ob);
	break;
      }
#endif
      CASE(F__OBJECT_CPU)
      {
	i=sp->u.ob->cpu;
	pop_stack();
	push_number(i);
	break;
      }

      CASE(F__OBJECT_MEMORY)
      {
	float size=0.0;
	if(!(sp->u.ob->flags & O_DESTRUCTED))
	{
	  size=(float)(sizeof(struct object)-sizeof(struct svalue));
	  size+=check_memory_usage(sp->u.ob->variables,sp->u.ob->prog->num_variables);
	  size+=((float)(sp->u.ob->prog->total_size))/
	    (float)(sp->u.ob->prog->ref);
	}
	pop_stack();
	push_number((int)size);
	break;
      }

      CASE(F_REGEXP)
      {
	struct vector *v;
	v = match_regexp((sp-1)->u.vec, sp->u.string);
	pop_n_elems(2);
	simple_push_vector(v);
	break;
      }
      CASE(F_SHADOW); f_shadow(); break;
      CASE(F_POP_VALUE2); pop_stack();
      /* FALL THROUGH */
      CASE(F_POP_VALUE); pop_stack(); break;
      CASE(F_DUP);
      sp++;
      assign_svalue_no_free(sp, sp-1);
      break;
      
      CASE(F_JUMP_WHEN_EQ);
      if(is_eq(sp-1,sp)) do_jump(); else pc+=2; pop_n_elems(2); break;
      CASE(F_JUMP_WHEN_NE);
      if(!is_eq(sp-1, sp)) do_jump(); else pc += 2; pop_n_elems(2); break;
      CASE(F_JUMP_WHEN_LT);
      if(is_lt(sp-1, sp)) do_jump(); else pc += 2; pop_n_elems(2); break;
      CASE(F_JUMP_WHEN_GT);
      if(is_gt(sp-1, sp)) do_jump(); else pc += 2; pop_n_elems(2); break;
      CASE(F_JUMP_WHEN_LE);
      if(!is_gt(sp-1, sp)) do_jump(); else pc += 2; pop_n_elems(2); break;
      CASE(F_JUMP_WHEN_GE);
      if(!is_lt(sp-1, sp)) do_jump(); else pc += 2; pop_n_elems(2); break;
      
      CASE(F_JUMP_WHEN_NON_ZERO);
      if (sp->type != T_NUMBER || sp->u.number != 0) do_jump(); else pc += 2;
      pop_stack();
      break;
      
      CASE(F_JUMP_WHEN_ZERO);
      if (sp->type == T_NUMBER && sp->u.number == 0) do_jump(); else pc += 2;
      pop_stack();
      break;
      
      CASE(F_JUMP); do_jump(); break;
      
      CASE(F_INDIRECT);
#ifdef DEBUG
      if (sp->type != T_LVALUE)
	fatal("Bad type to F_INDIRECT\n");
#endif
      /*
       * Fetch value of a variable. It is possible that it is a variable
       * that points to a destructed object. In that case, it has to
       * be replaced by 0.
       */
      check_svalue_for_destr(sp->u.lvalue);
      assign_svalue(sp, sp->u.lvalue);
      
      break;
      CASE(F_IDENTIFIER);
      argp=find_value((int)(EXTRACT_UCHAR(pc) + variable_index_offset));
      check_svalue_for_destr(argp);
      assign_svalue_no_free(++sp, argp);
      pc++;
      break;
      CASE(F_PUSH_IDENTIFIER_LVALUE);
      sp++;
      sp->type = T_LVALUE;
      sp->u.lvalue = find_value((int)(EXTRACT_UCHAR(pc) +
				      variable_index_offset));
      pc++;
      break;
      CASE(F_PUSH_INDEXED_LVALUE);
      push_indexed_lvalue();
      break;
      CASE(F_INDEX);
      push_indexed_svalue();
      check_svalue_for_destr(sp);
      break;
      CASE(F_LOCAL_NAME);
      assign_svalue_no_free(++sp, fp + EXTRACT_UCHAR(pc));
      pc++;
      break;
      CASE(F_PUSH_LOCAL_VARIABLE_LVALUE);
      sp++;
      sp->type = T_LVALUE;
      sp->u.lvalue = fp + EXTRACT_UCHAR(pc);
      pc++;
      break;
      
      CASE(F_RETURN_ONE);
      sp++;
      sp->type=T_NUMBER;
      sp->u.number=1;
      goto do_return;
      
      CASE(F_RETURN_ZERO);
      sp++;
      sp->type=T_NUMBER;
      sp->u.number=0;
      /* FALL THROUGH */
      
    do_return:
      
      CASE(F_RETURN)
      {
	struct svalue sv;
      
	sv = *sp--;
	/*
	 * Deallocate frame and return.
	 */
	for (i=0; i < csp->num_local_variables; i++)
	  pop_stack();
	sp++;
#ifdef DEBUG
	if (sp != fp)
	  fatal("Bad stack at F_RETURN  [%x vs %x]\n", sp, fp); /* marion */
#endif
	*sp = sv;	/* This way, the same ref counts are maintained */
	pop_control_stack();
	tracedepth--;
	if (TRACEP(TRACE_RETURN)) {
	  do_trace("Return", "", "");
	  if (TRACEHB) {
	    if (TRACETST(TRACE_ARGS)) {
	      add_message(" with value: ");
	      print_svalue(sp);
	    }
	    add_message("\n");
	  }
	}
	if (csp[1].extern_call)	/* The control stack was popped just before */
	  return;
	break;
      }
      CASE(F_BREAK_POINT);
      break_point();	/* generated by lang.y when -d. Will check stack. */
      push_number(0);
      break;
      
      CASE(F_CLONE_OBJECT)
      {
	 if(num_arg == 2)
	    ob = clone_object(sp[-1].u.string, sp->u.number != 0);
	 else
	    ob = clone_object(sp->u.string, 0);
	 pop_n_elems(num_arg);
	 if(ob)
	 {
	    sp++;
	    sp->type = T_OBJECT;
	    sp->u.ob = ob;
	    add_ref(ob, "F_CLONE_OBJECT");
	 }
	 else
	    push_number(0);
	 break;
      }
      
      CASE(F_AGGREGATE)
      {
	extern struct vector *allocate_array_no_init(int num);
	struct vector *v;
	unsigned short num;
      
	((char *)&num)[0] = pc[0];
	((char *)&num)[1] = pc[1];
	pc += 2;
	v = allocate_array_no_init((int)num);
#if 0				/* Optimization by Profezzorn */
	for (i=0; i < num; i++)
	  assign_svalue_no_free(&v->item[i], sp + i - num + 1);
	pop_n_elems((int)num);
#else
	sp-=(num-1);
	memcpy((char *)(v->item),sp,num*sizeof(struct svalue));
	/* clear to type 'illegal' */
	memset(sp,0,num*sizeof(struct svalue));
#endif
	sp->type = T_POINTER;
	sp->u.vec = v;		/* Ref count already initialized */
	break;
      }

      CASE(F_M_AGGREGATE)
      {
	struct vector *v, *w;
	unsigned short num;
	struct svalue *arg;
	extern struct vector *order_alist PROT((struct vector *));
      
	((char *)&num)[0] = pc[0];
	((char *)&num)[1] = pc[1];
	pc += 2;
	eval_cost+=num;
	v = allocate_array(num / 2);
	w = allocate_array(num / 2);
	for (i=0; i < num; i += 2)
	  {
	    arg = sp + i - num;
	    assign_svalue_no_free(&v->item[i / 2], arg + 1);
	    assign_svalue_no_free(&w->item[i / 2], arg + 2);
	  }
	pop_n_elems((int)num);
	sp++;
	v = allocate_mapping(v, w);
	sp->type = T_MAPPING;
	sp->u.vec = order_alist(v);
	free_mapping(v);
	break;
      }

      CASE(F_TAIL);
      if (tail(sp->u.string))
	assign_svalue(sp, &const1);
      else
	assign_svalue(sp, &const0);
      break;
      CASE(F_CALL_FUNCTION_BY_ADDRESS)
      {
	unsigned short func_index;
	struct function *funp;
      
	((char *)&func_index)[0] = pc[0];
	((char *)&func_index)[1] = pc[1];
	pc += 2;
	func_index += function_index_offset;
	/*
	 * Find the function in the function table. As the function may have
	 * been redefined by inheritance, we must look in the last table,
	 * which is pointed to by current_object.
	 */
#ifdef DEBUG
	if(func_index >= current_object->prog->num_functions)
	  fatal("Illegal function index\n");
#endif
      
	/* NOT current_prog, which can be an inherited object. */
	funp = &current_object->prog->functions[func_index];
      
	if (funp->flags & NAME_UNDEFINED)
	  error("Undefined function: %s\n", funp->name);
	/* Save all important global stack machine registers */
	push_control_stack(funp); /* return pc is adjusted later */
      
	/* This assigment must be done after push_control_stack() */
	current_prog = current_object->prog;
	/*
	 * If it is an inherited function, search for the real
	 * definition.
	 */
	csp->num_local_variables = EXTRACT_UCHAR(pc);
	pc++;
	funp = setup_new_frame(funp);
	csp->pc = pc;		/* The corrected return address */
	pc = current_prog->program + funp->offset;
	csp->extern_call = 0;

#ifdef DEBUG
	if(funp->offset >= current_prog->program_size)
	{
	   fatal("Bad function offset for F_CALL_FUNCTION_BY_ADDRESS.\n");
	}
#endif

	break;
      }

      CASE(F_SAVE_OBJECT);
      if(num_arg == 1)
      {
	save_object(current_object, sp->u.string);
      }
      else
      {
	save_object(sp->u.ob, (sp-1)->u.string);
	pop_stack();
      }
      break;

      CASE(F_FIND_OBJECT);
      if(num_arg == 2)
	if(sp->type == T_NUMBER && sp->u.number == 1)
	  ob = find_and_load_object((sp-1)->u.string);
	else
	  ob = find_object((sp-1)->u.string);
      else
	ob = find_object2(sp->u.string);
      pop_n_elems(num_arg);
      push_object(ob);
      break;

      CASE(F_FIND_PLAYER);
      ob = find_living_object(sp->u.string, 1);
      pop_stack();
      push_object(ob);
      break;

      CASE(F_WRITE_FILE);
      i = write_file((sp-1)->u.string, sp->u.string);
      pop_n_elems(2);
      push_number(i);
      break;

      CASE(F_READ_FILE)
      {
	char *str;
	struct svalue *arg = sp- num_arg + 1;
	int start = 0, len = 0;
      
	if (num_arg > 1)
	  start = arg[1].u.number;
	if (num_arg == 3)
	{
	  if (arg[2].type != T_NUMBER)
	    bad_arg(2, instruction+F_OFFSET);
	  len = arg[2].u.number;
	}
      
	str = read_file(arg[0].u.string, start, len);
	pop_n_elems(num_arg);
	if (str == 0)
	  push_number(0);
	else
	{
	  push_string(str, STRING_MALLOC);
	  free(str);
	}
	break;
      }
      CASE(F_READ_BYTES)
      {
	char *str;
	struct svalue *arg = sp- num_arg + 1;
	int start = 0, len = 0;
      
	if (num_arg > 1)
	  start = arg[1].u.number;
	if (num_arg == 3)
	{
	  if (arg[2].type != T_NUMBER)
	    bad_arg(2, instruction+F_OFFSET);
	  len = arg[2].u.number;
	}
      
	str = read_bytes(arg[0].u.string, start, len);
	pop_n_elems(num_arg);
	if (str == 0)
	  push_number(0);
	else
	{
	  push_string(str, STRING_MALLOC);
	  free(str);
	}
	break;
      }

      CASE(F_WRITE_BYTES);
      if(sp->type!=T_STRING) bad_arg(3,F_WRITE_BYTES);
      i = write_bytes((sp-2)->u.string, (sp-1)->u.number, sp->u.string);
      pop_n_elems(3);
      push_number(i);
      break;

      CASE(F_FILE_SIZE);
      i = file_size(sp->u.string);
      pop_stack();
      push_number(i);
      break;

#ifdef F__FILE_STAT
      CASE(F__FILE_STAT)
      {
	extern struct vector *file_stat();
	struct vector *v;
      
	v = file_stat(sp->u.string);
	pop_stack();
	simple_push_vector(v);
	break;
      }
#endif /* F__FILE_STAT */
#ifdef F__OBJECT_STAT
      CASE(F__OBJECT_STAT)
      {
	struct vector *v;
      
	v = allocate_array(1);
	v->item[0].u.number = sp->u.ob->time_of_load;
	pop_stack();
	simple_push_vector(v);
	break;
      }
#endif /* F__OBJECT_STAT */
#ifdef F__DRIVER_STAT
      CASE(F__DRIVER_STAT); f__driver_stat(); break;
#endif
      CASE(F_FIND_LIVING);
      ob = find_living_object(sp->u.string, 0);
      pop_stack();
      push_object(ob);
      break;

      CASE(F_TELL_OBJECT);
      tell_object((sp-1)->u.ob, sp->u.string);
      pop_stack();	/* Return first argument */
      break;

      CASE(F_RESTORE_OBJECT);
      i = restore_object(current_object, sp->u.string);
      pop_stack();
      push_number(i);
      break;

      CASE(F_THIS_PLAYER);
      pop_n_elems(num_arg);
      if(num_arg)
      {
	 if(current_interactive && current_interactive->interactive &&
	    !(current_interactive->flags & O_DESTRUCTED))
	 {
	    push_object(current_interactive);
	 }
	 else
	    push_number(0);
      }
      else if(command_giver && !(command_giver->flags & O_DESTRUCTED))
	push_object(command_giver);
      else
	push_number(0);
      break;
      
#ifdef F_FIRST_INVENTORY
      CASE(F_FIRST_INVENTORY);
      ob = first_inventory(sp);
      pop_stack();
      push_object(ob);
      break;
#endif /* F_FIRST_INVENTORY */

      CASE(F_LIVING);
      if (sp->u.ob->flags & O_ENABLE_COMMANDS)
	assign_svalue(sp, &const1);
      else
	assign_svalue(sp, &const0);
      break;

#ifdef F_GETUID
      CASE(F_GETUID);
      /*
       * Are there any reasons to support this one in -o mode ?
       */
      ob = sp->u.ob;
#ifdef DEBUG
      if (ob->user == 0)
	fatal("User is null pointer\n");
#endif
    {   char *tmp;
	tmp = ob->user->name;
	pop_stack();
	push_string(tmp, STRING_CONSTANT);
      }
      break;
#endif /* F_GETUID */

#ifdef F_GETEUID
      CASE(F_GETEUID);
      /*
       * Are there any reasons to support this one in -o mode ?
       */
      ob = sp->u.ob;
      
      if (ob->eff_user) {
	char *tmp;
	tmp = ob->eff_user->name;
	pop_stack();
	push_string(tmp, STRING_CONSTANT);
      }
      else {
	pop_stack();
	push_number(0);
      }
      break;
#endif /* F_GETEUID */
#ifdef F_EXPORT_UID
      CASE(F_EXPORT_UID);
      if (current_object->eff_user == 0)
	error("Illegal to export uid 0\n");
      ob = sp->u.ob;
      if (ob->eff_user)	/* Only allowed to export when null */
	break;
      ob->user = current_object->eff_user;
      break;
#endif /* F_EXPORT_UID */
#ifdef F_SETEUID
      CASE(F_SETEUID)
      {
	struct svalue *ret;
      
	if (sp->type == T_NUMBER)
	{
	  if (sp->u.number != 0)
	    bad_arg(1, F_SETEUID);
	  current_object->eff_user = 0;
	  pop_stack();
	  push_number(1);
	  break;
	}
	argp = sp;
	if (argp->type != T_STRING)
	  bad_arg(1, F_SETEUID);
	push_object(current_object);
	push_string(argp->u.string, STRING_CONSTANT);
	ret = apply_master_ob("valid_seteuid", 2);
	if (ret == 0 || ret->type != T_NUMBER || ret->u.number != 1)
	{
	  pop_stack();
	  push_number(0);
	  break;
	}
	current_object->eff_user = add_name(argp->u.string);
	pop_stack();
	push_number(1);
	break;
      }
#endif /* F_SETEUID */

#ifdef F_SETUID
      CASE(F_SETUID)
	setuid();
      push_number(0);
      break;
#endif /* F_SETUID */

#ifdef F_CREATOR
      CASE(F_CREATOR);
      ob = sp->u.ob;
      if (ob->user == 0) {
	assign_svalue(sp, &const0);
      } else {
	pop_stack();
	push_string(ob->user->name, STRING_CONSTANT);
      }
      break;
#endif
      CASE(F_SHUTDOWN);
#if !defined(_AIX) && !defined(__linux__) && !defined(__svr4__)
      startshutdowngame();
#else
      startshutdowngame(-1);
#endif
      push_number(0);
      break;

#ifdef F_SPRINTF
      CASE(F_SPRINTF)
      {
	extern char *string_print_formatted PROT((char *, int, struct svalue *));
	char *s;
      
	/*
	 * string_print_formatted() returns a pointer to it's internal
	 * buffer, or to an internal constant...  Either way, it must
	 * be copied before it's returned as a string.
	 */
      
	s = string_print_formatted((sp-num_arg+1)->u.string,
				   num_arg-1, sp-num_arg+2);
	pop_n_elems(num_arg);
	if (!s) push_number(0); else push_malloced_string(string_copy(s));
	break;
      }
#endif
#ifdef F__QUERY_IDENT
      CASE(F__QUERY_IDENT)
      {
	extern char *query_ident PROT((struct object *));
	char *tmp;
	struct svalue *arg1;
      
	if (!command_giver || !command_giver->interactive)
	  error("_query_ident(): illegal caller\n");
      
	if (num_arg == 1 && sp->type != T_OBJECT)
	  error("Bad optional argument to _query_ident()\n");
      
	push_constant_string("query_ident");
	arg1 = sapply("query_level_sec", command_giver, 1);
	if (num_arg && (!arg1 || arg1->type != T_NUMBER || arg1->u.number < 22))
	  tmp = 0;
	else
	  tmp = query_ident(num_arg ? sp->u.ob : 0);
      
	if (num_arg)
	  pop_stack();
	if (tmp == 0)
	  push_number(0);
	else
	  push_string(tmp, STRING_MALLOC);
	break;
      }
#endif
#ifdef F__QUERY_ACTION	
      CASE(F__QUERY_ACTION)
      {
	struct vector *v;
	struct svalue *arg;
	struct svalue *level;
      
	arg = sp - num_arg + 1;
	if (arg[0].type == T_OBJECT)
	  ob = arg[0].u.ob;
	else
	{
	  ob = find_object(arg[0].u.string);
	  if (ob == 0)
	    error("_query_action() failed\n");
	}
      
	if(ob->interactive)
	{
	  if(!current_interactive || !current_interactive->interactive ||
	     (current_interactive->flags & O_DESTRUCTED))
	    error("_query_action(): illegal usage\n");
	  if(current_interactive!=ob)
	  {
	    level = sapply("query_level", current_interactive, 0);
	    if (!level || level->type != T_NUMBER)
	      error("_query_action(): query_level returned illegal value\n");
	    if (level->u.number < 22)
	      error("_query_action(): illegal usage\n");
	  }
	}
      
	if (num_arg > 1)
	  v = get_action(ob, arg[1].u.string);
	else
	  v = get_all_actions(ob);
      
	pop_n_elems(num_arg);
	simple_push_vector(v);
      }
      break;
#endif
      
      CASE(F_EXPLODE)
      {
	struct vector *v;
	v = explode_string((sp-1)->u.string, sp->u.string);
	pop_n_elems(2);
	simple_push_vector(v);
	break;
      }
      
      CASE(F_FILTER_ARRAY); f_filter_array(num_arg); break;
      CASE(F_FILTER_MAPPING); f_filter_mapping(num_arg); break;
      CASE(F_SET_BIT); f_set_bit(); break;
      CASE(F_CLEAR_BIT); f_clear_bit(); break;
      CASE(F_TEST_BIT); f_test_bit(); break;
      
      CASE(F_QUERY_LOAD_AVERAGE);
      push_string(query_load_av(), STRING_MALLOC);
      break;
      CASE(F_CATCH)
      /*
       * Catch/Throw - catch errors in system or other peoples routines.
       */
      {
	extern jmp_buf error_recovery_context;
	extern int error_recovery_context_exists;
	extern struct svalue catch_value;
	unsigned short new_pc_offset;
	char *new_pc;
      
	/*
	 * Compute address of next instruction after the CATCH statement.
	 */
	((char *)&new_pc_offset)[0] = pc[0];
	((char *)&new_pc_offset)[1] = pc[1];
      
	new_pc = pc + new_pc_offset; /* relative */
	pc += 2;

#ifdef DEBUG
	if(new_pc < current_prog->program ||
	   new_pc >= current_prog->program + current_prog->program_size)
	{
	   fatal("Bad new program counter in catch().\n");
	}
#endif
      
	push_control_stack(0);
	csp->num_local_variables = 0; /* No extra variables */
	csp->pc = new_pc;
	csp->num_local_variables = (csp-1)->num_local_variables; /* marion */
	/*
	 * Save some global variables that must be restored separately
	 * after a longjmp. The stack will have to be manually popped all
	 * the way.
	 */
	push_pop_error_context (1);
      
	/* signal catch OK - print no err msg */
	error_recovery_context_exists = 2;
	if (setjmp(error_recovery_context))
	{
	  /*
	   * They did a throw() or error. That means that the control
	   * stack must be restored manually here.
	   * Restore the value of expected_stack also. It is always 0
	   * for catch().
	   */
	  expected_stack = 0;
	  push_pop_error_context (-1);
	  pop_control_stack();
	  assign_svalue_no_free(++sp, &catch_value);
	}
      
	/* next error will return 1 by default */
	assign_svalue(&catch_value, &const1);
	break;
      }
      CASE(F_THROW)
      {
	/* marion
	 * the return from catch is now done by a 0 throw
	 */
	extern int error_recovery_context_exists;
	assign_svalue(&catch_value, sp--);
#if 0
	if(catch_value.type == T_NUMBER && catch_value.u.number == 0 &&
	   error_recovery_context_exists > 1)
	{
	  /* We come here when no longjmp() was executed. */
	  pop_control_stack();
	  push_pop_error_context (0);
	  push_number(0);
	}
	else
#endif
	  throw_error();	/* do the longjump, with extra checks... */
	break;
      }

      CASE(F_NOTIFY_FAIL);
      set_notify_fail_message(sp->u.string);
      /* Return the argument */
      break;

      CASE(F_QUERY_IDLE);
      i = query_idle(sp->u.ob);
      pop_stack();
      push_number(i);
      break;

      CASE(F_INTERACTIVE);
      i = (int)sp->u.ob->interactive;
      pop_stack();
      push_number(i);
      break;

      CASE(F_IMPLODE)
      {
	char *str;
	check_for_destr((sp-1)->u.vec);
	str = implode_string((sp-1)->u.vec, sp->u.string);
	pop_n_elems(2);
	if (str)
	{
	  sp++;
	  sp->type = T_STRING;
	  sp->string_type = STRING_MALLOC;
	  sp->u.string = str;
	} else {
	  push_number(0);
	}
	break;
      }

      CASE(F_QUERY_SNOOP); f_query_snoop(); break;

      CASE(F_QUERY_IP_NUMBER);
      CASE(F_QUERY_IP_PORT)
      {
	extern char *query_ip_number PROT((struct object *));
	extern char *query_ip_port PROT((struct object *));
	char *tmp;
      
	if (num_arg == 1 && sp->type != T_OBJECT)
	  error("Bad optional argument to query_ip_number()\n");
	if (instruction+F_OFFSET == F_QUERY_IP_PORT)
	  tmp = query_ip_port(num_arg ? sp->u.ob : 0);
	else
	  tmp = query_ip_number(num_arg ? sp->u.ob : 0);
	if (num_arg)
	  pop_stack();
	if (tmp == 0)
	  push_number(0);
	else
	  push_string(tmp, STRING_MALLOC);
	break;
      }
#ifdef F_QUERY_HOST_NAME
      CASE(F_QUERY_HOST_NAME)
      {
	extern char *query_host_name();
	char *tmp;
      
	tmp = query_host_name();
	if (tmp)
	  push_string(tmp, STRING_CONSTANT);
	else
	  push_number(0);
	break;
      }
#endif
#ifdef F_NEXT_INVENTORY
      CASE(F_NEXT_INVENTORY);
      ob = sp->u.ob;
      pop_stack();
      push_object(ob->next_inv);
      break;
#endif /* F_NEXT_INVENTORY */
      CASE(F_ALL_INVENTORY)
      {
	struct vector *vec;
	vec = all_inventory(sp->u.ob);
	pop_stack();
	simple_push_vector(vec);
	break;
      }

      CASE(F_DEEP_INVENTORY)
      {
	struct vector *vec;
      
	vec = deep_inventory(sp->u.ob);
	free_svalue(sp);
	sp->type = T_POINTER;
	sp->u.vec = vec;
	break;
      }

      CASE(F_ENVIRONMENT);
      if (num_arg) {
	ob = environment(sp);
	pop_stack();
      } else if (!(current_object->flags & O_DESTRUCTED)) {
	ob = current_object->super;
      } else
	ob = 0;
      push_object(ob);
      break;

      CASE(F_THIS_OBJECT); push_object(current_object); break;
      CASE(F_PREVIOUS_OBJECT); push_object(previous_ob); break;
#ifdef F_LOCALCMD
      CASE(F_LOCALCMD);
      print_local_commands();
      push_number(0);
      break;
#endif /* F_LOCALCMD */
      CASE(F_SWAP);
#if TIME_TO_SWAP > 0
      swap(sp->u.ob);
#endif
      break;
      CASE(F_TRACE); f_trace(); break;
      CASE(F_TRACEPREFIX); f_traceprefix(); break;
      CASE(F_TIME); push_number(current_time); break;

      CASE(F_WIZLIST);
      if (num_arg) {
	wizlist(sp->u.string);
      } else {
	wizlist(0);
	push_number(0);
      }
      break;

#ifdef F__WIZ_LIST_INFO
      CASE(F__WIZ_LIST_INFO)
      {
	extern struct vector *get_wiz_list();
	simple_push_vector(get_wiz_list());
	break;
      }
#endif

#ifdef F_TRANSFER
      CASE(F_TRANSFER)
      {
	struct object *dest;
      
	if (sp->type == T_STRING) {
	  dest = find_object(sp->u.string);
	  if (dest == 0)
	    error("Object not found.\n");
	} else {
	  dest = sp->u.ob;
	}
	i = transfer_object((sp-1)->u.ob, dest);
	pop_n_elems(2);
	push_number(i);
	break;
      }
#endif

#ifdef F_ADD_WORTH
      CASE(F_ADD_WORTH);
      if (strncmp(current_object->name, "obj/", 4) != 0 &&
	  strncmp(current_object->name, "std/", 4) != 0 &&
	  strncmp(current_object->name, "room/", 5) != 0)
	error("Illegal call of add_worth.\n");
      if (num_arg == 2) {
	if (sp->u.ob->user)
	  sp->u.ob->user->total_worth += (sp-1)->u.number;
	pop_stack();
      } else {
	if (previous_ob == 0)
	  break;
	if (previous_ob->user)
	  previous_ob->user->total_worth += sp->u.number;
      }
      break;
#endif /* F_ADD_WORTH */
      CASE(F_ADD);
      /*if (inadd==0) checkplus(p);*/
      if(sp[-1].type==sp->type)
      {
	switch(sp->type)
	{
	case T_STRING:
	{
	  char *res;
	  res=add_strings(sp[-1].u.string,sp->u.string);
	  pop_n_elems(2);
	  push_malloced_string(res);
	  break;
	}	    
	case T_POINTER:
	{
	  struct vector *v;
	  check_for_destr((sp-1)->u.vec);
	  check_for_destr(sp->u.vec);
	  v = add_array_destructively((sp-1)->u.vec,sp->u.vec);
	  pop_n_elems(2);
	  simple_push_vector(v);
	  break;
	}
	case T_MAPPING:
	{
	  struct vector *v;
	  check_map_for_destr((sp-1)->u.vec);
	  check_map_for_destr(sp->u.vec);
	  v = add_mapping((sp-1)->u.vec,sp->u.vec);
	  pop_n_elems(2);
	  push_mapping(v);	/* This will make ref count == 2 */
	  v->ref--;
	  break;
	}
	case T_NUMBER:
	  sp[-1].u.number += sp->u.number;
	  sp--;
	  break;
	  
	default:
	  error("Bad arguments to +.\n");
	}
      } else if ((sp-1)->type == T_NUMBER && sp->type == T_STRING) {
	char buff[20], *res;
	sprintf(buff, "%d", (sp-1)->u.number);
	res=add_strings(buff,sp->u.string);
	pop_n_elems(2);
	push_malloced_string(res);
      } else if (sp[-1].type == T_STRING && sp->type == T_NUMBER) {
	char buff[20];
	char *res;
	sprintf(buff, "%d", sp->u.number);
	res=add_strings(sp[-1].u.string,buff);
	pop_n_elems(2);
	push_malloced_string(res);
      } else {
	error("Bad type of arg to '+'\n");
      }
      break;
      
      CASE(F_SUBTRACT);
      if(sp[-1].type != sp->type)
	bad_arg(2,instruction+F_OFFSET);
      
      if((sp-1)->type == T_POINTER)
      {
	extern struct vector *subtract_array
	  PROT((struct vector *,struct vector*));
	struct vector *v;
	
	v = sp->u.vec;
	
	if (v->ref > 1)
	{
	  free_vector(v);
	  sp->u.vec=v = slice_array(v, 0, v->size-1 );
	}
	/* subtract_array already takes care of destructed objects */
	*sp=const0; sp--;
	v = subtract_array(sp->u.vec, v);
	pop_stack();
	simple_push_vector(v);
	break;
      }
      sp[-1].u.number -= sp->u.number;
      sp--;
      break;

    CASE(F_AND);
      if(sp[-1].type != sp->type)
	bad_arg(2,instruction+F_OFFSET);

      if (sp->type == T_POINTER) {
	extern struct vector *intersect_array
	  PROT((struct vector *, struct vector *));
	(sp-1)->u.vec = intersect_array(sp->u.vec, (sp-1)->u.vec);
	sp--;
      }else{
	sp[-1].u.number &= sp->u.number;
	sp--;
      }
      break;

    CASE(F_OR);  sp[-1].u.number |= sp->u.number; sp--; break;
    CASE(F_XOR); sp[-1].u.number ^= sp->u.number; sp--;  break;
    CASE(F_LSH); sp[-1].u.number <<= sp->u.number; sp--; break;
    CASE(F_RSH); sp[-1].u.number >>= sp->u.number; sp--; break;
    CASE(F_MULTIPLY); sp[-1].u.number *= sp->u.number; sp--; break;

    CASE(F_DIVIDE);
      if (sp->u.number == 0)
	error("Division by zero\n");
      sp[-1].u.number /= sp->u.number;
      sp--;
      break;
    CASE(F_MOD);
      if (sp->u.number == 0)
	error("Modulus by zero.\n");
      sp[-1].u.number %= sp->u.number;
      sp--;
      break;
    CASE(F_GT); i=is_gt(sp-1,sp); pop_n_elems(2); push_number(i); break;
    CASE(F_GE); i=!is_lt(sp-1,sp);pop_n_elems(2); push_number(i); break;
    CASE(F_LT);	i=is_lt(sp-1,sp); pop_n_elems(2); push_number(i); break;
    CASE(F_LE); i=!is_gt(sp-1,sp);pop_n_elems(2); push_number(i); break;
    CASE(F_EQ); i=is_eq(sp-1,sp); pop_n_elems(2); push_number(i); break;
    CASE(F_NE); i=!is_eq(sp-1,sp);pop_n_elems(2); push_number(i); break;

#ifdef F_LOG_FILE
    CASE(F_LOG_FILE);
      log_file((sp-1)->u.string, sp->u.string);
      pop_stack();
      break;	/* Return first argument */
#endif /* F_LOG_FILE */

    CASE(F_NOT);
      if (sp->type == T_NUMBER)
	sp->u.number = !sp->u.number;
      else
	assign_svalue(sp, &const0);
      break;
    CASE(F_COMPL); sp->u.number = ~ sp->u.number; break;
    CASE(F_NEGATE); sp->u.number = - sp->u.number; break;

    CASE(F_INC_AND_POP);
    CASE(F_INC);
	if (sp->type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    if( sp->u.ind->type != T_NUMBER )
		error("Bad left type to ++.\n");
	    push_svalue( get_index_type_value( sp->u.ind ) );
	    sp->u.number++;
	    assign_index_type( sp );
	    if(instruction+F_OFFSET == F_INC_AND_POP)
	      pop_stack();
	    break;
	}
	if (sp->type != T_LVALUE)
	    error("Bad argument to ++\n");
	if (sp->u.lvalue->type != T_NUMBER)
	    error("++ of non-numeric argument\n");
	sp->u.lvalue->u.number++;
	if(instruction+F_OFFSET == F_INC_AND_POP)
	{
	  pop_stack();
	}else{
	  assign_svalue(sp, sp->u.lvalue);
	}
	break;

    CASE(F_DEC_AND_POP);
    CASE(F_DEC);
	if (sp->type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    if( sp->u.ind->type != T_NUMBER )
		error("Bad left type to --.\n");
	    push_svalue( get_index_type_value( sp->u.ind ) );
	    sp->u.number--;
	    assign_index_type( sp );
	    if(instruction+F_OFFSET== F_DEC_AND_POP)
	      pop_stack();
	    break;
	}
	if (sp->type != T_LVALUE)
	    error("Bad argument to --\n");
	if (sp->u.lvalue->type != T_NUMBER)
	    error("-- of non-numeric argument\n");
	sp->u.lvalue->u.number--;
	if(instruction+F_OFFSET == F_DEC_AND_POP)
	{
	  pop_stack();
	}else{
	  assign_svalue(sp, sp->u.lvalue);
	}
	break;
    CASE(F_POST_INC);
	if (sp->type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    if( sp->u.ind->type != T_NUMBER )
		error("Bad right type to ++.\n");
	    push_svalue( get_index_type_value( sp->u.ind ) );
	    sp->u.number++;
	    assign_index_type( sp );
	    sp->u.number--;
	    break;
	}
	if (sp->type != T_LVALUE)
	    error("Bad argument to ++\n");
	if (sp->u.lvalue->type != T_NUMBER)
	    error("++ of non-numeric argument\n");
	sp->u.lvalue->u.number++;
	assign_svalue(sp, sp->u.lvalue);
	sp->u.number--;
	break;
    CASE(F_POST_DEC);
	if (sp->type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    if( sp->u.ind->type != T_NUMBER )
		error("Bad right type to --.\n");
	    push_svalue( get_index_type_value( sp->u.ind ) );
	    sp->u.number--;
	    assign_index_type( sp );
	    sp->u.number++;
	    break;
	}
	if (sp->type != T_LVALUE)
	    error("Bad argument to --\n");
	if (sp->u.lvalue->type != T_NUMBER)
	    error("-- of non-numeric argument\n");
	sp->u.lvalue->u.number--;
	assign_svalue(sp, sp->u.lvalue);
	sp->u.number++;
	break;
    CASE(F_CALL_OTHER);
    {
	struct svalue *arg, tmp;
	
	arg = sp - num_arg + 1;
	if (current_object->flags & O_DESTRUCTED) {
	    /*
	     * No external calls may be done when this object is
	     * destructed.
	     */
	    pop_n_elems(num_arg);
	    push_number(0);
	    break;
	}
	if (arg[1].u.string[0] == ':')
	    error("Illegal function name in call_other: %s\n",
		  arg[1].u.string);

	if (arg[0].type == T_OBJECT)
	    ob = arg[0].u.ob;
	else  if (arg[0].type == T_POINTER)
	{
	  struct vector *ret;
	  struct vector *call_all_other();

	  ret =  call_all_other(arg[0].u.vec, arg[1].u.string, num_arg-2);
	  pop_n_elems(2);
	  simple_push_vector(ret);
	  break;
	} else {
	  ob = find_object(arg[0].u.string);
	  if (ob == 0)
	    error("call_other() failed\n");
	}
	/*
	 * Send the remaining arguments to the function.
	 */
	if (TRACEP(TRACE_CALL_OTHER)) {
	    do_trace("Call other ", arg[1].u.string, "\n");
	}
	if (apply_low(arg[1].u.string, ob, num_arg-2) == 0) {
	    /* Function not found */
	    pop_n_elems(2);
	    push_number(0);
	    break;
	}
	/*
	 * The result of the function call is on the stack. But, so
	 * is the function name and object that was called.
	 * These have to be removed.
	 */
	tmp = *sp--;	/* Copy the function call result */
	pop_n_elems(2);	/* Remove old arguments to call_other */
	*++sp = tmp;	/* Re-insert function result */
	break;
    }
    CASE(F_INTP);
	if (sp->type == T_NUMBER)
	  sp->u.number=1;
	else
	  assign_svalue(sp, &const0);
	break;
    CASE(F_STRINGP);
	if (sp->type == T_STRING)
	    assign_svalue(sp, &const1);
	else
	    assign_svalue(sp, &const0);
	break;
    CASE(F_OBJECTP);
	if (sp->type == T_OBJECT)
	    assign_svalue(sp, &const1);
	else
	    assign_svalue(sp, &const0);
	break;
    CASE(F_POINTERP);
	if (sp->type == T_POINTER)
	    assign_svalue(sp, &const1);
	else
	    assign_svalue(sp, &const0);
	break;
    CASE(F_MAPPINGP);
	if (sp->type == T_MAPPING)
	    assign_svalue(sp, &const1);
	else
	    assign_svalue(sp, &const0);
	break;
    CASE(F_EXTRACT); f_extract(num_arg); break;
    CASE(F_PUSH_RANGE_LVALUE) push_range_lvalue(); break;
    CASE(F_RANGE); push_range_svalue(); break;
    
    CASE(F_QUERY_VERB);
	if (last_verb == 0) {
	    push_number(0);
	    break;
	}
	push_string(last_verb, STRING_MALLOC);
	break;

    CASE(F_EXEC);
	i = replace_interactive((sp-1)->u.ob, sp->u.ob, current_object->name);
	pop_stack();
	pop_stack();
	push_number(i);
	break;

    CASE(F_FILE_NAME);
    {
      char *name,*res;

      /* This function now returns a leading '/', except when -o flag */
      name = sp->u.ob->name;
#ifdef COMPAT_MODE
      res = string_copy(name);
#else
      res = add_slash(name);
#endif	
      pop_stack();
      push_malloced_string(res);
      break;
    }

    CASE(F_USERS); simple_push_vector(users()); break;

    CASE(F_CALL_OUT);
    {
      struct svalue *arg = sp - num_arg + 1;

      if (!(current_object->flags & O_DESTRUCTED))
	new_call_out(current_object, arg[0].u.string, arg[1].u.number,
		     num_arg == 3 ? sp : 0);
      pop_n_elems(num_arg);
      push_number(0);
    }
      break;
    CASE(F_CALL_OUT_INFO);
	simple_push_vector(get_all_call_outs());
	break;
    CASE(F_REMOVE_CALL_OUT);
	i = remove_call_out(current_object, sp->u.string);
	pop_stack();
	push_number(i);
	break;
    CASE(F_FIND_CALL_OUT);
        i = find_call_out(current_object, sp->u.string);
  	pop_stack();
	push_number(i);
        break;
#ifdef F_INHERIT_LIST
    CASE(F_INHERIT_LIST)
    {
	struct vector *vec;
	extern struct vector *inherit_list PROT((struct object *));

	vec = inherit_list(sp->u.ob);
	pop_stack();
	simple_push_vector(vec);
	break;
    }
#endif /* F_INHERIT_LIST */
    CASE(F_WRITE);
	do_write(sp);
	break;
#ifdef F__SYSLOG	
    CASE(F__SYSLOG);
	if (current_object)
	  printf("syslog: %s: %s\n",
		 current_object->name,
		 sp->u.string);
	else
	  printf("syslog: %s\n", sp->u.string);
	fflush(stdout);
	pop_stack();
	push_number(0);
	break;
#endif
    CASE (F_MEMBER_ARRAY);
    {
      struct vector *v;

      v = sp->u.vec;
      check_for_destr(v);
      for (i=0; i < v->size; i++)
      {
	eval_cost ++;
	if(is_eq(v->item+i, sp-1))
	  break;
      }
      if (i == v->size)
	i = -1;			/* Return -1 for failure */
      pop_n_elems(2);
      push_number(i);
      break;
    }
    CASE(F_MOVE_OBJECT);
    {
      struct object *o1, *o2;

      if ((sp-1)->type == T_OBJECT)
	o1 = (sp-1)->u.ob;
      else {
	o1 = find_object((sp-1)->u.string);
	if (o1 == 0)
	  error("move_object failed\n");
      }
      if (sp->type == T_OBJECT)
	o2 = sp->u.ob;
      else {
	o2 = find_object(sp->u.string);
	if (o2 == 0)
	  error("move_object failed\n");
      }
      move_object(o1, o2);
      pop_stack();
      break;
    }

    CASE(F_FUNCTION_EXISTS);
    {
      char *str, *res;

      str = function_exists((sp-1)->u.string, sp->u.ob);
      pop_n_elems(2);
      if (str) {
#ifdef COMPAT_MODE
	res = string_copy (str); /* Marion sighs deeply. */
#else
	res = add_slash(str);
#endif
	if (str = strrchr (res, '.'))
	  *str = 0;
	push_malloced_string(res);
      } else {
	push_number(0);
      }
      break;
    }
    CASE(F_SNOOP); f_snoop(num_arg); break;
    CASE(F_ADD_ACTION);
    {
	struct svalue *arg = sp - num_arg + 1;
	if (num_arg == 3) {
	    if (arg[2].type != T_NUMBER)
		bad_arg(3, instruction+F_OFFSET);
	}
	add_action(arg[0].u.string,
		   num_arg > 1 ? arg[1].u.string : 0,
		   num_arg > 2 ? arg[2].u.number : 0);
	pop_n_elems(num_arg-1);
	break;
    }
#ifdef F_ADD_VERB
    CASE(F_ADD_VERB);
	add_verb(sp->u.string,0);
	break;
#endif /* F_ADD_VERB */
#ifdef F_ADD_XVERB
    CASE(F_ADD_XVERB);
   	add_verb(sp->u.string,1);
	break;
#endif /* F_ADD_XVERB */
    CASE(F_ALLOCATE);
    {
	struct vector *v;

	v = allocate_array(sp->u.number);	/* Will have ref count == 1 */
	pop_stack();
	simple_push_vector(v);
	break;
    }
    CASE(F_ED); f_ed(num_arg); break;
    CASE(F_CRYPT);
    {
	char salt[2];
	char *res;
	char *choise =
	    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

	if (sp->type == T_STRING && strlen(sp->u.string) >= 2) {
	    salt[0] = sp->u.string[0];
	    salt[1] = sp->u.string[1];
	} else {
	    salt[0] = choise[random_number(strlen(choise))];
	    salt[1] = choise[random_number(strlen(choise))];
	}
#ifdef sun
	res = string_copy((char *) _crypt((sp-1)->u.string, salt));
#else
	res = string_copy((char *) crypt((sp-1)->u.string, salt));
#endif
	pop_n_elems(2);
	push_malloced_string(res);
	break;
    }
#ifdef F_CREATE_WIZARD
    CASE(F_CREATE_WIZARD);
    {
	char *str;
	struct svalue *arg = sp - num_arg + 1;
	str = create_wizard(arg[0].u.string,
			    num_arg == 2 ? arg[1].u.string : 0);
	pop_n_elems(num_arg);
	if (str)
	    push_string(str, STRING_MALLOC);
	else
	    push_number(0);
	break;
    }
#endif
    CASE(F_DESTRUCT);
	destruct_object(sp);
	break;

#ifdef F__DISCONNECT
    CASE(F__DISCONNECT);
      {
	struct svalue *ret;
	ret=apply_master_ob("valid_disconnect",0);
	if(ret && !(ret->type==T_NUMBER && ret->u.number==0))
	{
	  remove_interactive(sp->u.ob);
	}
	break;
      }
#endif
#ifdef F__DESTRUCT	
    CASE(F__DESTRUCT);
	{
	struct svalue *arg1;

	/* Don't wan't no NPC to call this function */
	if (!command_giver || !command_giver->interactive)
	  error("_destruct(): illegal caller\n");

	/* We don't want to call apply on destructed objects... */
	if(command_giver->flags & O_DESTRUCTED)
	   error("Bad call to _destruct()\n");

	/* Only wizards > level 21 may use this function */
	arg1 = sapply("query_level_sec", command_giver, 0);
	if (!arg1 || arg1->type != T_NUMBER || arg1->u.number < 22)
	  error("_destruct(): illegal caller\n");
	
	sdestruct_object(sp);
	break;
      }
#endif
#ifdef F__SUPER_INVIS
      CASE(F__SUPER_INVIS); f__super_invis(num_arg); break;
#endif

#ifdef F__LOCK
      CASE(F__LOCK)
      {
	if (command_giver)
	{
	  if (sp->u.number)
	    command_giver->flags |= O_OBJECT_LOCKED;
	  else
	    command_giver->flags &= ~O_OBJECT_LOCKED;
	}
	break;
      }
#endif

#ifdef F__ACL_ACCESS
      CASE(F__ACL_ACCESS); efun_acl_access(sp, num_arg); break;
#endif
	
#ifdef F__ACL_MODIFY
      CASE(F__ACL_MODIFY); efun_acl_modify(sp); break;
#endif
#ifdef F__ACL_GET
      CASE(F__ACL_GET); efun_acl_get(sp); break;
#endif
#ifdef F__ACL_STR2NUM
      CASE(F__ACL_STR2NUM); efun_acl_str2num(sp); break;
#endif
#ifdef F__ACL_PUT
      CASE(F__ACL_PUT); efun_acl_put(sp); break;
#endif
#ifdef F__ACL_NUM2STR
      CASE(F__ACL_NUM2STR); efun_acl_num2str(sp); break;
#endif
#ifdef F__ACL_DEBUG
      CASE(F__ACL_DEBUG); efun_acl_debug(sp); break;
#endif
#ifdef F__ISCLONE
    CASE(F__ISCLONE);
    {
      int i;
	  
      i = is_cloned_object(sp);
      pop_stack();
      push_number(i);
      break;
    }
#endif
    CASE(F_RANDOM);
      if (sp->u.number <= 0) {
	sp->u.number = 0;
	break;
      }
      sp->u.number = random_number(sp->u.number);
      break;

#ifdef F_SAY
    CASE(F_SAY); f_say(num_arg); break;
#endif

#ifdef F_TELL_ROOM
    CASE(F_TELL_ROOM); f_tell_room(num_arg); break;
#endif
#ifdef F_SHOUT
    CASE(F_SHOUT); shout_string(sp->u.string); break;
#endif /* F_SHOUT */
    CASE(F_WHILE);
	fatal("F_WHILE should not appear.\n");
    CASE(F_DO);
	fatal("F_DO should not appear.\n");
    CASE(F_FOR);
	fatal("F_FOR should not appear.\n");
    CASE(F_SWITCH);
    {
	extern char* findstring PROT((char*));
	unsigned short offset,end_adr;
	int d,s,r;
	char *l,*end_tab;
	static short off_tab[] = { 0*6,1*6,3*6,7*6,15*6,31*6,63*6,127*6,255*6,
					511*6,1023*6,2047*6,4095*6,8191*6 };

	((char *)&offset)[0] = pc[1];
	((char *)&offset)[1] = pc[2];
	((char *)&end_adr)[0] = pc[3];
	((char *)&end_adr)[1] = pc[4];

	if ( ( i = EXTRACT_UCHAR(pc) >> 4 ) != 0xf ) {
	    if ( sp->type == T_NUMBER && !sp->u.number ) {
		/* special case: uninitalized string */
		s = (int)ZERO_AS_STR_CASE_LABEL;
	    } else if ( sp->type == T_STRING ) {
	        switch(sp->string_type) {
	        case STRING_SHARED:
	            s = (int)sp->u.string;
	            break;
		default:
		    s = (int)findstring(sp->u.string);
	            break;
	        }
	    } else {
		bad_arg(1, F_SWITCH);
	    }
	} else {
	    if (sp->type != T_NUMBER) bad_arg(1, F_SWITCH);
	    s = sp->u.number;
	    i = (int)pc[0] &0xf ;
	}
	pop_stack();
	if ( i >= 14 )
	  fatal("unsupported switch table format.\n");

#ifdef DEBUG
	for(l = current_prog->program + offset;
	    l < current_prog->program + end_adr; l += 6)
	{
	   int key;
	   unsigned short addr;
	   ((char *)&key)[0] = l[0];
	   ((char *)&key)[1] = l[1];
	   ((char *)&key)[2] = l[2];
	   ((char *)&key)[3] = l[3];
	   ((char *)&addr)[0] = l[4];
	   ((char *)&addr)[1] = l[5];
	   /* The special case is for range */
	   if(addr != 1 &&
	      (current_prog->program + addr < pc + 7 || addr >= offset))
	   {
	      printf("   Codestart: %d   Codeend: %d\n",
		     pc - current_prog->program + 7, offset);
	      printf("   Key: %d         Addr: %u\n", key, addr);
	      fatal("Bad switch table at execution.\n");
	   }
	}
#endif

	end_tab = current_prog->program + end_adr;
	l = current_prog->program + offset + off_tab[i];
	d = (off_tab[i]+6) >> 1;
	if (d == 3) d=0;
	for(;;)
	{
	  ((char *)&r)[0] = l[0];
	  ((char *)&r)[1] = l[1];
	  ((char *)&r)[2] = l[2];
	  ((char *)&r)[3] = l[3];
	  if (s < r)
	  {
	    if (d < 6)
	    {
	      if (!d)		/* test for range */
	      {
		((char *)&offset)[0] = l[-2];
		((char *)&offset)[1] = l[-1];

		/* F_BREAK is required to be > 1 */
		if (offset <= 1) {
		  ((char *)&r)[0] = l[-6];
		  ((char *)&r)[1] = l[-5];
		  ((char *)&r)[2] = l[-4];
		  ((char *)&r)[3] = l[-3];
		  if (s >= r) {
		    /* s is in the range */
		    if (!offset) {
		      /* range with lookup table */
		      ((char *)&offset)[0] = l[4];
		      ((char *)&offset)[1] = l[5];
		      l = current_prog->program + offset +
			(s-r) * sizeof(short);
		      ((char *)&offset)[0] = l[0];
		      ((char *)&offset)[1] = l[1];
		      break;
		    }
		    ((char *)&offset)[0] = l[4];
		    ((char *)&offset)[1] = l[5];
		    break;
		  }
		}
		/* use default address */
		((char *)&offset)[0] = pc[5];
		((char *)&offset)[1] = pc[6];
		break;
	      } /* !d */
	      d = 0;
	    } else {
	      /* d >= 6 */
	      l -= d;
	      d >>= 1;
	    }
	  }
	  else if (s > r)
	  {
	    if (d < 6)
	    {
	      if (!d)		/* test for range */
	      {
		((char *)&offset)[0] = l[4];
		((char *)&offset)[1] = l[5];
		if (offset <= 1)
		{
		  ((char *)&r)[0] = l[6];
		  ((char *)&r)[1] = l[7];
		  ((char *)&r)[2] = l[8];
		  ((char *)&r)[3] = l[9];
		  if (s <= r)
		  {
		    /* s is in the range */
		    if (!offset)
		    {
		      /* range with lookup table */
		      ((char *)&offset)[0] = l[10];
		      ((char *)&offset)[1] = l[11];
		      l = current_prog->program + offset +
			(s-r) * sizeof(short);
		      ((char *)&offset)[0] = l[0];
		      ((char *)&offset)[1] = l[1];
		      break;
		    }
		    ((char *)&offset)[0] = l[10];
		    ((char *)&offset)[1] = l[11];
		    break;
		  }
		}
		/* use default address */
		((char *)&offset)[0] = pc[5];
		((char *)&offset)[1] = pc[6];
		break;
	      } /* !d */
	      d = 0;
	    } else {
	      /* d >= 6 */
	      l += d;
	      while (l >= end_tab)
	      {
		d >>= 1;
		if (d <= 3) {
		  if (!d) break;
		  d = 0;
		}
		l -= d;
	      }
	      d >>= 1;
	    }
	  } else {
	    /* s == r */
	    ((char *)&offset)[0] = l[4];
	    ((char *)&offset)[1] = l[5];
	    if ( !l[-2] && !l[-1] ) 
	    {
	      /* end of range with lookup table */
	      ((char *)&r)[0] = l[-6];
	      ((char *)&r)[1] = l[-5];
	      ((char *)&r)[2] = l[-4];
	      ((char *)&r)[3] = l[-3];
	      l = current_prog->program + offset + (s-r)*sizeof(short);
	      ((char *)&offset)[0] = l[0];
	      ((char *)&offset)[1] = l[1];
	    }
	    if (offset <= 1)
	    {
	      if (!offset) {
		/* start of range with lookup table */
		((char *)&offset)[0] = l[10];
		((char *)&offset)[1] = l[11];
		l = current_prog->program + offset;
		((char *)&offset)[0] = l[0];
		((char *)&offset)[1] = l[1];
	      } else {
		((char *)&offset)[0] = l[10];
		((char *)&offset)[1] = l[11];
	      }
	    }
	    break;
	  }
	}
	pc = current_prog->program + offset;

#ifdef DEBUG
	if(pc < current_prog->program ||
	   pc >= current_prog->program + current_prog->program_size)
	{
	   fatal("Bad jump in switch().\n");
	}
#endif

	break;
    }
    CASE(F_BREAK);
	fatal("F_BREAK should not appaear.\n");
    CASE(F_SUBSCRIPT);
	fatal("F_SUBSCRIPT should not appear.\n");
    CASE(F_STRLEN);
	i = strlen(sp->u.string);
	pop_stack();
	push_number(i);
	break;
    CASE(F_MKMAPPING);
    {
      extern struct vector *order_alist PROT((struct vector *));
      struct vector *v;

      i = (sp-1)->u.vec->size;
      if (i > sp->u.vec->size)
      {
	i = sp->u.vec->size;
      }
      v = allocate_mapping(slice_array((sp-1)->u.vec, 0, i - 1),
			   slice_array(sp->u.vec, 0, i - 1));
      pop_n_elems(2);
      push_mapping(order_alist(v));
      free_mapping(v);
      sp->u.vec->ref--;		/* This will make ref count == 1 */
      break;
    }
    CASE(F_M_SIZEOF);
      i = sp->u.vec->item[0].u.vec->size;
      pop_stack();
      push_number(i);
      break;

    CASE(F_M_INDICES);
    {
      struct vector *v;

      v = sp->u.vec;
      check_map_for_destr(v);
      v = v->item[0].u.vec;
      v = slice_array(v, 0, v->size - 1);
      pop_stack();
      simple_push_vector(v);
      break;
    }

    CASE(F_M_VALUES);
    {
      struct vector *v;

      v = sp->u.vec;
      check_map_for_destr(v);
      v = v->item[1].u.vec;
      v = slice_array(v, 0, v->size - 1);
      pop_stack();
      simple_push_vector(v);
      break;
    }

    CASE(F_M_DELETE);
      check_map_for_destr((sp-1)->u.vec);
      i = assoc(sp, (sp-1)->u.vec->item[0].u.vec);
      pop_stack();
      if (i >= 0)
      {
	struct vector *v, *w;

	v = sp->u.vec->item[0].u.vec;
	w = sp->u.vec->item[1].u.vec;
	v = allocate_mapping(slice_array(v, 0, v->size-1),
			     slice_array(w, 0, w->size-1));
	pop_stack();
	remove_mapping(v, i);
	push_mapping(v);
	v->ref--;		/* Will make ref count == 1 */
      }
      break;

    CASE(F__M_DELETE);
      check_map_for_destr((sp-1)->u.vec);
      i = assoc(sp, (sp-1)->u.vec->item[0].u.vec);
      pop_stack();
      if (i >= 0)
	remove_mapping(sp->u.vec, i);
      break;

    CASE(F_SIZEOF);
      i = sp->u.vec->size;
      pop_stack();
      push_number(i);
      break;

    CASE(F_LOWER_CASE);
    {
      char *str = string_copy(sp->u.string);
      for (i = strlen(str)-1; i>=0; i--)
	if (isalpha(str[i]))
	  str[i] |= 'a' - 'A';
      pop_stack();
      push_malloced_string(str);
      break;
    }

    CASE(F_ATOI);
      i=atoi(sp->u.string);
      pop_stack();
      push_number(i);
      break;

    CASE(F_UPPER_CASE);
    {
      char *str = string_copy(sp->u.string);
      for (i = strlen(str)-1; i>=0; i--)
	if (islower(str[i]))
	  str[i] -= 'a' - 'A';
      pop_stack();
      push_malloced_string(str);
      break;
    }

    CASE(F_SET_HEART_BEAT);
      i = set_heart_beat(current_object, sp->u.number);
      sp->u.number = i;
      break;

    CASE(F_CAPITALIZE);
      if (islower(sp->u.string[0]))
      {
	char *str;

	str = string_copy(sp->u.string);
	str[0] += 'A' - 'a';
	pop_stack();
	push_malloced_string(str);
      }
      break;

    CASE(F_PROCESS_STRING);
    {
	extern char
	    *process_string PROT((char *));

	char *str;

	str = process_string(sp->u.string);
	if (str != sp->u.string) {
	    pop_stack();
	    push_malloced_string(str);
	}
	break;
    }

    CASE(F_COMMAND);
    {
	struct svalue *arg = sp - num_arg + 1;

	if (num_arg == 1)
	    i = command_for_object(arg[0].u.string, 0);
	else
#ifdef COMPAT_MODE
	    i = command_for_object(arg[0].u.string, arg[1].u.ob);
#else
	    error("Too many arguments to command()\n");
#endif
	pop_n_elems(num_arg);
	push_number(i);
	break;
    }
    CASE(F_GET_DIR);
    {
	struct vector *v = get_dir(sp->u.string);
	pop_stack();
	simple_push_vector(v);
	break;
    }
    CASE(F_RM);
	i = remove_file(sp->u.string);
	pop_stack();
	push_number(i);
	break;
    CASE(F_CAT);
    {
	struct svalue *arg = sp- num_arg + 1;
	int start = 0, len = 0;

	if (num_arg > 1)
	    start = arg[1].u.number;
	if (num_arg == 3) {
	    if (arg[2].type != T_NUMBER)
		bad_arg(2, instruction+F_OFFSET);
	    len = arg[2].u.number;
	}
	i = print_file(arg[0].u.string, start, len);
	pop_n_elems(num_arg);
	push_number(i);
	break;
    }
    CASE(F_MKDIR);
    {
	char *path;

	if (cfg_use_acls)
	{
	    path = acl_checkrights(sp->u.string, ACL_A, 1, 0);
	}
	else
	{
#ifdef COMPAT_MODE
	    path = check_file_name(sp->u.string, 1);
#else
	    path = check_valid_path(sp->u.string, current_object->eff_user, "mkdir", 1);
#endif
	}
	/* pop_stack(); see comment above... */
	if (path == 0 || mkdir(path, 0770) == -1)
	    assign_svalue(sp, &const0);
	else
	    assign_svalue(sp, &const1);
	break;
    }
    CASE(F_RMDIR);
    {
	char *path;

	if (cfg_use_acls)
	{
	    path = acl_checkrights(sp->u.string, ACL_D, 1, 0);
	}
	else
	{
#ifdef COMPAT_MODE	
	    path = check_file_name(sp->u.string, 1);
#else	
	    path = check_valid_path(sp->u.string, current_object->eff_user, "rmdir", 1);
#endif
	}
	/* pop_stack(); rw - what the heck ??? */
	if (path == 0 || rmdir(path) == -1)
	    assign_svalue(sp, &const0);
	else
	    assign_svalue(sp, &const1);
	break;
    }
    CASE(F_INPUT_TO);
    {
	struct svalue *arg = sp - num_arg + 1;
	struct object *obj;
	int flag = 1;
	obj=(struct object *)0;

	if (num_arg == 1 || arg[1].type == T_NUMBER && arg[1].u.number == 0)
	    flag = 0;

#if 1
	if(num_arg == 3)
	{
	  if(arg[2].type == T_OBJECT && arg[2].u.ob)
	    obj=arg[2].u.ob;
	}
	i = input_to(arg[0].u.string, flag, obj ? obj : current_object);
#else
	i = input_to(arg[0].u.string, flag);
#endif
	pop_n_elems(num_arg);
	push_number(i);
	break;
    }
    CASE(F_SET_LIVING_NAME);
	set_living_name(current_object, sp->u.string);
	break;
    CASE(F_PARSE_COMMAND);
    {
	struct svalue *arg;

	num_arg = EXTRACT_UCHAR(pc);
	pc++;
	arg = sp - num_arg + 1;
	if (arg[0].type != T_STRING)
	    bad_arg(1, F_PARSE_COMMAND);
	if (arg[1].type != T_OBJECT && arg[1].type != T_POINTER)
	    bad_arg(2, F_PARSE_COMMAND);
	if (arg[2].type != T_STRING)
	    bad_arg(3, F_PARSE_COMMAND);
	if (arg[1].type == T_POINTER)
	    check_for_destr(arg[1].u.vec);

	i = parse(arg[0].u.string, &arg[1], arg[2].u.string, &arg[3],
		  num_arg-3); 
	pop_n_elems(num_arg);	/* Get rid of all arguments */
	push_number(i);		/* Push the result value */
	break;
    }
#ifdef F_REPLACE
    CASE(F_REPLACE);
      {
	extern char *string_replace(struct svalue *,struct svalue *,
				    struct svalue *);
	struct svalue *argp = sp - 3 + 1;

	if(argp[2].type == T_STRING)
	{
	  char *e;
	  e=string_replace(argp,argp+1,argp+2);
	  pop_n_elems(3);
	  push_malloced_string(e);
	  break;
	} else {
	  bad_arg(3,F_REPLACE);
	}
	break;
      }
#endif

    CASE(F_SSCANF);
	num_arg = EXTRACT_UCHAR(pc);
	pc++;
	i = f_sscanf(num_arg);
	pop_n_elems(num_arg);
	push_number(i);
	break;
    CASE(F_ENABLE_COMMANDS);
	enable_commands(1);
	push_number(1);
	break;
    CASE(F_DISABLE_COMMANDS);
	enable_commands(0);
	push_number(0);
	break;
    CASE(F_PRESENT);
	{
	  struct svalue *arg = sp - num_arg + 1;
	  ob = object_present(arg, num_arg == 1 ? 0 : arg[1].u.ob);
	  pop_n_elems(num_arg);
	  push_object(ob);
	}
	break;
#ifdef F_SET_LIGHT
    CASE(F_SET_LIGHT);
    {
	struct object *o1;
	o1=current_object;
	if(num_arg > 1) o1=sp->u.ob;
	add_light(o1, sp[1-num_arg].u.number);
	while(o1->super) o1 = o1->super;
	sp[1-num_arg].u.number = o1->total_light;
	if(num_arg>1) pop_stack();
	break;
    }
#endif /* F_SET_LIGHT */
    CASE(F_CONST0);
	push_number(0);
	break;
    CASE(F_CONST1);
	push_number(1);
	break;
    CASE(F_CONST_1);
        push_number(-1);
	break;
    CASE(F_BYTE);
	push_number(EXTRACT_UCHAR(pc++)+2);
	break;
    CASE(F_NEG_BYTE);
        push_number(-EXTRACT_UCHAR(pc++)-2);
        break;
    CASE(F_SHORT);
    {
      short foo;
      ((char *)&foo)[0] = pc[0];
      ((char *)&foo)[1] = pc[1];
      pc += 2;
      push_number(foo);
      break;
    }

    CASE(F_NUMBER);
	((char *)&i)[0] = pc[0];
	((char *)&i)[1] = pc[1];
	((char *)&i)[2] = pc[2];
	((char *)&i)[3] = pc[3];
	pc += 4;
	push_number(i);
	break;
    CASE(F_ASSIGN);
	if( sp[-1].type == T_INDEX_TYPE ) /* Wing 920401 */
	{
	    if( sp->type != sp[-1].u.ind->type )
		error("Bad type to =.\n");
	    assign_index_type( sp );
	    break;
	}
	assign_svalue((sp-1)->u.lvalue, sp);
	assign_svalue(sp-1, sp);
	pop_stack();
	break;
    CASE(F_ASSIGN_AND_POP);
	if( sp[-1].type == T_INDEX_TYPE ) /* Wing 920401 */
	{
	  if( sp->type != sp[-1].u.ind->type )
	    error("Bad type to =.\n");
	  assign_index_type( sp );
	  pop_stack();
	  break;
	}
	assign_svalue((sp-1)->u.lvalue, sp);
	pop_stack();
	pop_stack();
	break;

    CASE(F_CTIME);
    {
	char *cp;
	cp = string_copy(time_string(sp->u.number));
	pop_stack();
	push_malloced_string(cp);
	/* Now strip the newline. */
	cp = strchr(cp, '\n');
	if (cp)
	    *cp = '\0';
	break;
    }
    CASE(F_ADD_EQ);
	if (sp[-1].type == T_INDEX_TYPE) /* Wing 920401 */
        {
	    push_svalue( get_index_type_value( sp[-1].u.ind ) );
	    switch(sp->type) {
		case T_STRING:
		{
		    char *new_str;
		    if (sp[-1].type == T_STRING) {
		      new_str=add_strings(sp->u.string,sp[-1].u.string);
		    } else if (sp[-1].type == T_NUMBER) {
		      char buff[20];
		      sprintf(buff, "%d", sp[-1].u.number);
		      new_str=add_strings(sp->u.string,buff);
		    } else {
			bad_arg(2, F_ADD_EQ);
		    }
		    pop_n_elems(2);
		    push_malloced_string(new_str);
		    break;
		}
	     case T_NUMBER:
		if (sp[-1].type == T_NUMBER) {
		    i = sp->u.number + sp[-1].u.number;
		    pop_n_elems(2);
		    push_number(i);
		} else {
		    error("Bad type number to rhs +=.\n");
		}
		break;
	    case T_POINTER:
		if (sp[-1].type != T_POINTER) {
		    error("Bad type to rhs +=.\n");
		} else {
		    struct vector *v;
		    check_for_destr(sp->u.vec);
		    check_for_destr(sp[-1].u.vec);
		    v = add_array_destructively(sp->u.vec,sp[-1].u.vec);
		    pop_stack();
		    simple_push_vector(v); /* This will make ref count == 2 */
		}
		break;	      
	    default:
		error("Bad type to lhs +=");
	    }
	    assign_index_type( sp );
	    break;
        }
        else if (sp[-1].type != T_LVALUE)
	    bad_arg(1, F_ADD_EQ);
	argp = sp[-1].u.lvalue;
	switch(argp->type) {
	case T_STRING:
	{
	    char *new_str;
	    if (sp->type == T_STRING) {
	      new_str=add_strings(argp->u.string,sp->u.string);
	    } else if (sp->type == T_NUMBER) {
		char buff[20];
		sprintf(buff, "%d", sp->u.number);
		new_str=add_strings(argp->u.string,buff);
	    } else {
		bad_arg(2, F_ADD_EQ);
	    }
	    pop_n_elems(2);
	    push_malloced_string(new_str);
	    break;
	}
	case T_NUMBER:
	    if (sp->type == T_NUMBER) {
	        i = argp->u.number + sp->u.number;
		pop_n_elems(2);
		push_number(i);
	    } else {
	        error("Bad type number to rhs +=.\n");
	    }
	    break;
	case T_MAPPING:
	    if (sp->type != T_MAPPING) {
		error("Bad type to rhs +=.\n");
	    } else {
		struct vector *v;
		check_map_for_destr(argp->u.vec);
		check_map_for_destr(sp->u.vec);
		v = add_mapping(argp->u.vec, sp->u.vec);
		pop_n_elems(2);
	        push_mapping(v); /* This will make ref count == 2 */
	        v->ref--;
	    }
	    break;
        case T_POINTER:
	    if (sp->type != T_POINTER) {
		error("Bad type to rhs +=.\n");
	    } else {
	      struct vector *v;
	      check_for_destr(argp->u.vec);
	      check_for_destr(sp->u.vec);
	      v = add_array_destructively(argp->u.vec,sp->u.vec);
	      pop_n_elems(2);
	      simple_push_vector(v); /* This will make ref count == 2 */
	    }
	    break;	      
	default:
	    error("Bad type to lhs +=");
	}
	assign_svalue(argp, sp);
	break;
    CASE(F_SUB_EQ);
	if (sp[-1].type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    struct svalue *value = get_index_type_value( sp[-1].u.ind );

	    switch (sp[-1].u.ind->type) {
		case T_NUMBER:
		   if (sp->type != T_NUMBER)
		      error("Bad right type to -=");
		   sp->u.number = value->u.number - sp->u.number;
		   assign_index_type( sp );
		   break;
		case T_POINTER:
		{
		   struct vector *subtract_array PROT((struct vector*,struct vector*));
		   struct vector *v;
		   static struct svalue tmp;
		    
		   if (sp->type != T_POINTER)
		      error("Bad right type to -=");
		   v = sp->u.vec;
		   if (v->ref > 1)
		   {
		      v = slice_array(v, 0, v->size-1 );
		      v->ref--;
		   }
		   sp--;
		   v = subtract_array(value->u.vec, v);
		   tmp.type = T_POINTER;
		   tmp.u.vec = v;
		   push_number(0);  /* Dummy for assign_index_type to pop... */
		   assign_index_type( &tmp );
		   free_vector( v );
		   break;
		}
		default:
		    error("Bad left type to -=.\n");
	    }
	    break;
	}
	if (sp[-1].type != T_LVALUE)
	    bad_arg(1, F_SUB_EQ);
	argp = sp[-1].u.lvalue;
	switch (argp->type) {
	case T_NUMBER:
	if (sp->type != T_NUMBER)
	        error("Bad right type to -=");
	    argp->u.number -= sp->u.number;
	    sp--;
            break;
	case T_POINTER:
	  {
	    struct vector *subtract_array PROT((struct vector*,struct vector*));
	    struct vector *v;

	    if (sp->type != T_POINTER)
	        error("Bad right type to -=");
	    v = sp->u.vec;
	    if (v->ref > 1) {
		v = slice_array(v, 0, v->size-1 );
		v->ref--;
            }
	    sp--;
	    v = subtract_array(argp->u.vec, v);
	    free_vector(argp->u.vec);
	    argp->u.vec = v;
	    break;
	  }
	default:
	    error("Bad left type to -=.\n");
	}
	assign_svalue_no_free(sp, argp);
	break;
    CASE(F_MULT_EQ);
	if (sp[-1].type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    struct svalue *value = get_index_type_value( sp[-1].u.ind );

	    if( sp[-1].u.ind->type != T_NUMBER )
		error("Bad right type to *=.\n");
	    if( sp->type != T_NUMBER )
		error("Bad right type to *=.\n");
	    sp->u.number *= value->u.number;
	    assign_index_type( sp );
	    break;
	}
	argp = sp[-1].u.lvalue;
	if (argp->type != T_NUMBER)
	    error("Bad left type to *=.\n");
	i = argp->u.number * sp->u.number;
	pop_n_elems(2);
	push_number(i);
	assign_svalue(argp, sp);
	break;
    CASE(F_AND_EQ);
	if (sp[-1].type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    struct svalue *value = get_index_type_value( sp[-1].u.ind );

	    if( sp[-1].u.ind->type != T_NUMBER )
		error("Bad right type to &=.\n");
	    if( sp->type != T_NUMBER )
		error("Bad right type to &=.\n");
	    sp->u.number &= value->u.number;
	    assign_index_type( sp );
	    break;
	}
	if(sp[-1].type != T_LVALUE)
	   error("Bad left value to &=.\n");
	argp = sp[-1].u.lvalue;
	if(argp->type != T_NUMBER && argp->type != T_POINTER)
	   error("Bad left type to &=.\n");
	if(argp->type != sp->type)
	   error("Bad right type to &=.\n");
	if(sp->type == T_POINTER)
	{
	   struct vector *v;
	   extern struct vector *intersect_array
	      PROT((struct vector *, struct vector *));
	   v = intersect_array(sp->u.vec, argp->u.vec);
	   sp -= 2;  /* Intersect_array just freed the arguments */
	   simple_push_vector(v);
	}
	else
	{
	   i = argp->u.number & sp->u.number;
	   pop_n_elems(2);
	   push_number(i);
	}
	assign_svalue_no_free(argp, sp);
	break;
    CASE(F_OR_EQ);
	if (sp[-1].type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    struct svalue *value = get_index_type_value( sp[-1].u.ind );

	    if( sp[-1].u.ind->type != T_NUMBER )
		error("Bad right type to |=.\n");
	    sp->u.number |= value->u.number;
	    assign_index_type( sp );
	    break;
	}
	argp = sp[-1].u.lvalue;
	if (argp->type != T_NUMBER)
	    error("Bad left type to |=.\n");
	i = argp->u.number | sp->u.number;
	pop_n_elems(2);
	push_number(i);
	assign_svalue(argp, sp);
	break;
    CASE(F_XOR_EQ);
	if (sp[-1].type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    struct svalue *value = get_index_type_value( sp[-1].u.ind );

	    if( sp[-1].u.ind->type != T_NUMBER )
		error("Bad right type to ^=.\n");
	    sp->u.number ^= value->u.number;
	    assign_index_type( sp );
	    break;
	}
	argp = sp[-1].u.lvalue;
	if (argp->type != T_NUMBER)
	    error("Bad left type to ^=.\n");
	i = argp->u.number ^ sp->u.number;
	pop_n_elems(2);
	push_number(i);
	assign_svalue(argp, sp);
	break;
    CASE(F_LSH_EQ);
	if (sp[-1].type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    struct svalue *value = get_index_type_value( sp[-1].u.ind );

	    if( sp[-1].u.ind->type != T_NUMBER )
		error("Bad right type to <<=.\n");
	    sp->u.number = value->u.number << sp->u.number;;
	    assign_index_type( sp );
	    break;
	}
	argp = sp[-1].u.lvalue;
	if (argp->type != T_NUMBER)
	    error("Bad left type to <<=.\n");
	i = argp->u.number << sp->u.number;
	pop_n_elems(2);
	push_number(i);
	assign_svalue(argp, sp);
	break;
    CASE(F_RSH_EQ);
	if (sp[-1].type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    struct svalue *value = get_index_type_value( sp[-1].u.ind );

	    if( sp[-1].u.ind->type != T_NUMBER )
		error("Bad right type to >>=.\n");
	    sp->u.number = value->u.number >> sp->u.number;;
	    assign_index_type( sp );
	    break;
	}
	argp = sp[-1].u.lvalue;
	if (argp->type != T_NUMBER)
	    error("Bad left type to >>=.\n");
	i = argp->u.number >> sp->u.number;
	pop_n_elems(2);
	push_number(i);
	assign_svalue(argp, sp);
	break;
#ifdef F_COMBINE_FREE_LIST
    CASE(F_COMBINE_FREE_LIST);
#ifdef MALLOC_malloc
	push_number(resort_free_list());
#else
	push_number(0);
#endif
	break;
#endif
    CASE(F_DIV_EQ);
	if (sp[-1].type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    struct svalue *value = get_index_type_value( sp[-1].u.ind );

	    if( sp[-1].u.ind->type != T_NUMBER )
		error("Bad right type to /=.\n");
	    if (sp->u.number == 0)
		error("Division by 0\n");
	    sp->u.number = value->u.number / sp->u.number;;
	    assign_index_type( sp );
	    break;
	}
	argp = sp[-1].u.lvalue;
	if (argp->type != T_NUMBER)
	    error("Bad left type to /=.\n");
	if (sp->u.number == 0)
	    error("Division by 0\n");
	i = argp->u.number / sp->u.number;
	pop_n_elems(2);
	push_number(i);
	assign_svalue(argp, sp);
	break;
    CASE(F_MOD_EQ);
	if (sp[-1].type == T_INDEX_TYPE) /* Wing 920401 */
	{
	    struct svalue *value = get_index_type_value( sp[-1].u.ind );

	    if( sp[-1].u.ind->type != T_NUMBER )
		error("Bad right type to %%=.\n");
	    if(sp->u.number == 0)
	       error("Division by 0\n");
	    sp->u.number = value->u.number % sp->u.number;;
	    assign_index_type( sp );
	    break;
	}
	argp = sp[-1].u.lvalue;
	if (argp->type != T_NUMBER)
	    error("Bad left type to %%=.\n");
	if (sp->u.number == 0)
	    error("Division by 0\n");
	i = argp->u.number % sp->u.number;
	pop_n_elems(2);
	push_number(i);
	assign_svalue(argp, sp);
	break;
    CASE(F_STRING);
    {
	unsigned short string_number;
	((char *)&string_number)[0] = pc[0];
	((char *)&string_number)[1] = pc[1];
	pc += 2;
	sp++;
	sp->type=T_STRING;
	sp->string_type=STRING_SHARED;
	sp->u.string=copy_shared_string(current_prog->strings[string_number]);
	break;
    }

    CASE(F_LAND);
      if (sp->type == T_NUMBER && sp->u.number == 0)
      {
	do_jump();
      }else{
        pc += 2;
        pop_stack();
      }
      break;
  
    CASE(F_LOR);
      if (sp->type != T_NUMBER || sp->u.number != 0)
      {
	do_jump();
      }else{
        pc += 2;
        pop_stack();
      }
      break;

    CASE(F_SHORT_STRING);
	sp++;
	sp->type=T_STRING;
	sp->string_type=STRING_SHARED;
	sp->u.string=copy_shared_string(
		current_prog->strings[EXTRACT_UCHAR(pc++)]);
	break;

#ifdef F_RUSAGE
    CASE(F_RUSAGE);
    {
        char buff[500];

	struct rusage rus;
	long utime, stime;
	int maxrss;

	if (getrusage(RUSAGE_SELF, &rus) < 0)
            buff[0] = 0;
	else {
	    utime = rus.ru_utime.tv_sec * 1000 + rus.ru_utime.tv_usec / 1000;
	    stime = rus.ru_stime.tv_sec * 1000 + rus.ru_stime.tv_usec / 1000;
	    maxrss = rus.ru_maxrss;
#ifdef sun
	    maxrss *= getpagesize() / 1024;
#endif
	    sprintf(buff, "%ld %ld %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
		    utime, stime, maxrss, rus.ru_ixrss, rus.ru_idrss,
		    rus.ru_isrss, rus.ru_minflt, rus.ru_majflt, rus.ru_nswap,
		    rus.ru_inblock, rus.ru_oublock, rus.ru_msgsnd, 
		    rus.ru_msgrcv, rus.ru_nsignals, rus.ru_nvcsw, 
		    rus.ru_nivcsw);
	  }
	push_string(buff, STRING_MALLOC);
	break;
    }
#endif
    CASE(F_CINDENT);
    {
	char *path;

	if (cfg_use_acls)
	{
	    path = acl_checkrights(sp->u.string, ACL_R+ACL_W, 0, 0);
	}
	else
	{
	    
#ifdef COMPAT_MODE
	    path = check_file_name(sp->u.string, 1);
#else
	    path = check_valid_path(sp->u.string, current_object->eff_user, "cindent", 1);
#endif
	}
	if (path) {
	    if (indent_program(path)) {
		assign_svalue(sp, &const1);
		break;
	    }
	} else {
	    add_message("Illegal attempt to indent\n");
	}
	assign_svalue(sp, &const0);
	break;
    }
    CASE(F_UNIQUE_ARRAY); {
	extern struct vector
	    *make_unique PROT((struct vector *arr,char *func,
	    struct svalue *skipnum));
	struct vector *res;

	if (num_arg < 3) {
	    check_for_destr((sp-1)->u.vec);
	    res = make_unique((sp-1)->u.vec, sp->u.string, &const0);
	} else {
	    check_for_destr((sp-2)->u.vec);
	    res = make_unique((sp-2)->u.vec, (sp-1)->u.string, sp);
	    pop_stack ();
	}
	pop_n_elems(2);
	simple_push_vector(res);
	break;
    }

    CASE(F_VERSION);
        push_string(driver_version, STRING_MALLOC);
        break;

#ifdef F_RENAME
    CASE(F_RENAME); {
	i = do_rename((sp-1)->u.string, sp->u.string);
	pop_n_elems(2);
	push_number(i);
	break;
    }
#endif /* F_RENAME */
    CASE(F_MAP_ARRAY); f_map_array(num_arg); break;
    CASE(F_MAP_MAPPING); f_map_mapping(num_arg); break;
    CASE(F_SORT_ARRAY); f_sort_array(num_arg); break;
#ifdef F_ORDER_ALIST
    CASE(F_ORDER_ALIST); f_order_alist(num_arg); break;
#endif

#ifdef F_INSERT_ALIST
    CASE(F_INSERT_ALIST) f_insert_alist(num_arg); break;
#endif /* F_INSERT_ALIST */

#ifdef F_ASSOC
    CASE(F_ASSOC); f_assoc(num_arg); break;
#endif /* F_ASSOC */

#ifdef F_INTERSECT_ALIST
    CASE(F_INTERSECT_ALIST);
    {
	extern struct vector *intersect_alist
	  PROT((struct vector *, struct vector *));
	struct vector *tmp = intersect_alist( (sp-1)->u.vec, sp->u.vec );
	pop_stack();
	free_vector(sp->u.vec);
	sp->u.vec = tmp;
	break;
    }
#endif /* F_INTERSECT_ALIST */
#ifdef F_DEBUG_INFO
    CASE(F_DEBUG_INFO); f_debug_info(num_arg); break;
#endif /* F_DEBUG_INFO */
    }
#ifdef DEBUG
    if (expected_stack && expected_stack != sp ||
	sp < fp + csp->num_local_variables - 1)
    {
	fatal("Bad stack after evaluation. Instruction %d, num arg %d\n",
	      instruction+F_OFFSET, num_arg);
    }
#endif /* DEBUG */
  }
}

/*
 * Apply a fun 'fun' to the program in object 'ob', with
 * 'num_arg' arguments (already pushed on the stack).
 * If the function is not found, search in the object pointed to by the
 * inherit pointer.
 * If the function name starts with '::', search in the object pointed out
 * through the inherit pointer by the current object. The 'current_object'
 * stores the base object, not the object that has the current function being
 * evaluated. Thus, the variable current_prog will normally be the same as
 * current_object->prog, but not when executing inherited code. Then,
 * it will point to the code of the inherited object. As more than one
 * object can be inherited, the call of function by index number has to
 * be adjusted. The function number 0 in a superclass object must not remain
 * number 0 when it is inherited from a subclass object. The same problem
 * exists for variables. The global variables function_index_offset and
 * variable_index_offset keep track of how much to adjust the index when
 * executing code in the superclass objects.
 *
 * There is a special case when called from the heart beat, as
 * current_prog will be 0. When it is 0, set current_prog
 * to the 'ob->prog' sent as argument.
 *
 * Arguments are always removed from the stack.
 * If the function is not found, return 0 and nothing on the stack.
 * Otherwise, return 1, and a pushed return value on the stack.
 *
 * Note that the object 'ob' can be destructed. This must be handled by
 * the caller of apply().
 *
 * If the function failed to be called, then arguments must be deallocated
 * manually !
 */

char debug_apply_fun[30]; /* For debugging */
int ccsave;  /* for object_cpu() */


static int apply_low(fun, ob, num_arg)
    char *fun;
    struct object *ob;
    int num_arg;
{
  static int cache_id[APPLY_CACHE_SIZE];
  static int cache_flags[APPLY_CACHE_SIZE];
  static char *cache_name[APPLY_CACHE_SIZE];
  static struct function *cache_pr[APPLY_CACHE_SIZE];
  static struct function *cache_pr_inherited[APPLY_CACHE_SIZE];
  static struct program *cache_progp[APPLY_CACHE_SIZE];
  static int cache_function_index_offset[APPLY_CACHE_SIZE];
  static int cache_variable_index_offset[APPLY_CACHE_SIZE];

  struct function *pr;
  struct program *progp;
  extern int num_error;
  struct control_stack *save_csp;
  int ix;
  int csave=eval_cost;
  int cccsave=ccsave;
  char *funname;
  extern char *findstring PROT((char *));

  /* This condition may or may not be a good idea.
   * It aims at giving clean_up a better chance
   * Waiting for the explosion...
   */
  if(current_interactive || (command_giver && command_giver->interactive))
     ob->time_of_ref = current_time; /* Used by the swapper */

  /*
   * This object will now be used, and is thus a target for
   * reset later on (when time due).
   */
  ob->flags &= ~O_RESET_STATE;
#ifdef DEBUG
  strncpy(debug_apply_fun, fun, sizeof debug_apply_fun);
  debug_apply_fun[sizeof debug_apply_fun - 1] = '\0';
#endif
  if (num_error > 0)
    goto failure;
  if (fun[0] == ':')
    error("Illegal function call\n");
  /*
   * If there is a chain of objects shadowing, start with the first
   * of these.
   */
  while (ob->shadowed && ob->shadowed != current_object)
    ob = ob->shadowed;
 retry_for_shadow:
#if TIME_TO_SWAP > 0
  if (ob->flags & O_SWAPPED)
    load_ob_from_swap(ob);
#endif
  progp = ob->prog;
#ifdef DEBUG
  if (ob->flags & O_DESTRUCTED)
    fatal("apply() on destructed object\n");
#endif
#ifdef F__CACHE_STATS
  apply_low_call_others++;
#endif
  ix = ( progp->id_number ^ (int)fun ^ ( (int)fun >> APPLY_CACHE_BITS )) & (APPLY_CACHE_SIZE - 1);
  if (cache_id[ix] == progp->id_number
      && !strcmp(cache_name[ix], fun)
      /* && (!cache_progp[ix] || cache_progp[ix] == ob->prog) */)
  {
    /* We have found a matching entry in the cache. The pointer to
       the function name has to match, not only the contents.
       This is because hashing the string in order to get a cache index
       would be much more costly than hashing it's pointer.
       If cache access would be costly, the cache would be useless.
       */
#ifdef F__CACHE_STATS
    apply_low_cache_hits++;
#endif
    if (cache_progp[ix])
    {
      /* the cache will tell us in wich program the function is, and
       * where
       */
      pr = cache_pr_inherited[ix];

      if (!(pr->type & (TYPE_MOD_STATIC|TYPE_MOD_PRIVATE)) ||
	  current_object == ob)
      {
	push_control_stack(cache_pr[ix]);
	csp->num_local_variables = num_arg;
	current_prog = cache_progp[ix];

	function_index_offset = cache_function_index_offset[ix];
	variable_index_offset = cache_variable_index_offset[ix];
	/* Remove excessive arguments */
	while(csp->num_local_variables > pr->num_arg) {
	  pop_stack();
	  csp->num_local_variables--;
	}
	/* Correct number of arguments and local variables */
	while(csp->num_local_variables < pr->num_arg + pr->num_local) {
	  push_number(0);
	  csp->num_local_variables++;
	}
	tracedepth++;
	if (TRACEP(TRACE_CALL)) {
	  do_trace_call(pr);
	}
	fp = sp - csp->num_local_variables + 1;
#ifdef OLD_PREVIOUS_OBJECT_BEHAVIOUR
	/* Now, previous_object() is always set, even by
	 * call_other(this_object()). It should not break any
	 * compatibility.
	 */
	if (current_object != ob)
#endif
	  previous_ob = current_object;
	current_object = ob;
	save_csp = csp;
	eval_instruction(current_prog->program + pr->offset);
#ifdef DEBUG
	if (save_csp-1 != csp)
	  fatal("Bad csp after execution in apply_low\n");
#endif
	/*
	 * Arguments and local variables are now removed. One
	 * resulting value is always returned on the stack.
	 */
	csave-=eval_cost+cccsave-ccsave;
	ob->cpu-=csave;
	if(ob->user) ob->user->cost-=csave;
	ccsave-=csave;
	return 1;
      }
    } /* when we come here, the cache has told us that the function isn't
       * defined in the object
       */
  } else {
    int fun_was_static;		/* profezzorn */
    /* we have to search the function */

    fun_was_static=0;
    if(funname = findstring(fun))
    {				/* If the name is amongst the shared strings */
      for(pr=progp->functions; pr < progp->functions + progp->num_functions;
	  pr++)
      {
	eval_cost++;
	if (pr->name == 0 ||
	    pr->name != funname ||
	    (pr->type & TYPE_MOD_PRIVATE))
	{
	  continue;
	}
	if (pr->flags & NAME_UNDEFINED)
	  continue;
	/* Static functions may not be called from outside. */
	if ((pr->type & (TYPE_MOD_STATIC|TYPE_MOD_PRIVATE)) &&
	    current_object != ob)
	{
	  fun_was_static=1;
	  continue;
	}
	/* The searched function is found */
	if (!cache_progp[ix] && cache_id[ix])
	{
	  /* The old cache entry was for an undefined function, so the
	     name had to be malloced */
	  free(cache_name[ix]);
	}
#ifdef F__CACHE_STATS
	if (!cache_id[ix]) {
	  apply_low_slots_used++;
	} else {
	  apply_low_collisions++;
	}
#endif
	cache_id[ix] = progp->id_number;
	cache_pr[ix] = pr;
	cache_name[ix] = pr->name;
	push_control_stack(pr);
	csp->num_local_variables = num_arg;
	current_prog = progp;
	pr = setup_new_frame(pr);
	cache_pr_inherited[ix] = pr;
	cache_progp[ix] = current_prog;
	cache_variable_index_offset[ix] = variable_index_offset;
	cache_function_index_offset[ix] = function_index_offset;
#ifdef OLD_PREVIOUS_OBJECT_BEHAVIOUR
	if (current_object != ob)
#endif
	  previous_ob = current_object;
	current_object = ob;
	save_csp = csp;
	eval_instruction(current_prog->program + pr->offset);
#ifdef DEBUG
	if (save_csp-1 != csp)
	  fatal("Bad csp after execution in apply_low\n");
#endif
	/*
	 * Arguments and local variables are now removed. One
	 * resulting value is always returned on the stack.
	 */
	csave-=eval_cost+cccsave-ccsave;
	ob->cpu-=csave;
	if(ob->user) ob->user->cost-=csave;
	ccsave-=csave;
	return 1;
      }
    }
    /* We don't put static functions in the cache, 
     * because the next caller may be allowed to call it.
     *  /Profezzorn
     */
    if(!fun_was_static)
    {
      /* We have to mark a function not to be in the object */
      if (!cache_progp[ix] && cache_id[ix])
      {
	/* The old cache entry was for an undefined function, so the
	   name had to be malloced */
	free(cache_name[ix]);
      }
#ifdef F__CACHE_STATS
      if (!cache_id[ix]) {
	apply_low_slots_used++;
      } else {
	apply_low_collisions++;
      }
#endif
      cache_id[ix] = progp->id_number;
      cache_name[ix] = string_copy(fun);
      cache_progp[ix] = (struct program *)0;
    }
  }
  if (ob->shadowing) {
    /*
     * This is an object shadowing another. The function was not found,
     * but can maybe be found in the object we are shadowing.
     */
    ob = ob->shadowing;
    goto retry_for_shadow;
  }
 failure:
  /* Failure. Deallocate stack. */
  pop_n_elems(num_arg);
  return 0;
}

/*
 * Arguments are supposed to be
 * pushed (using push_string() etc) before the call. A pointer to a
 * 'struct svalue' will be returned. It will be a null pointer if the called
 * function was not found. Otherwise, it will be a pointer to a static
 * area in apply(), which will be overwritten by the next call to apply.
 * Reference counts will be updated for this value, to ensure that no pointers
 * are deallocated.
 */

struct svalue *sapply(fun, ob, num_arg)
    char *fun;
    struct object *ob;
    int num_arg;
{
#ifdef DEBUG
    struct svalue *expected_sp;
#endif
    static struct svalue ret_value = { T_NUMBER };

    if (TRACEP(TRACE_APPLY)) {
	do_trace("Apply", "", "\n");
    }
#ifdef DEBUG
    expected_sp = sp - num_arg;
#endif
    if (apply_low(fun, ob, num_arg) == 0)
	return 0;
    assign_svalue(&ret_value, sp);
    pop_stack();
#ifdef DEBUG
    if (expected_sp != sp)
	fatal("Corrupt stack pointer.\n");
#endif
    return &ret_value;
}

struct svalue *apply(fun, ob, num_arg)
    char *fun;
    struct object *ob;
    int num_arg;
{
    tracedepth = 0;
    return sapply(fun, ob, num_arg);
}

/*
 * this is a "safe" version of apply
 * this allows you to have dangerous driver mudlib dependencies
 * and not have to worry about causing serious bugs when errors occur in the
 * applied function and the driver depends on being able to do something
 * after the apply. (such as the ed exit function, and the net_dead function)
 */

struct svalue *safe_apply (char *fun,struct object * ob,int num_arg)
{
  jmp_buf save_error_recovery_context;
  extern jmp_buf error_recovery_context;
  struct svalue *ret;
  struct object *save_command_giver = command_giver;
  
  debug(32768, ("safe_apply: before sp = %d\n", sp));
  ret = NULL;
  memcpy((char *) save_error_recovery_context,
	 (char *) error_recovery_context, sizeof(error_recovery_context));
  if (!setjmp(error_recovery_context)) {
    if(!(ob->flags & O_DESTRUCTED)) {
      ret = apply(fun,ob,num_arg);
    }
  } else {
    clear_state();
    ret = NULL;
    fprintf(stderr,"Warning: Error in the '%s' function in '%s'\n",
	    fun,ob->name);
    fprintf(stderr,
	    "The driver may function improperly if this problem is not fixed.\n");
  }
  debug(32768, ("safe_apply: after sp = %d\n", sp));
  memcpy((char *) error_recovery_context,
	 (char *) save_error_recovery_context, sizeof(error_recovery_context));
  command_giver = save_command_giver;
  return ret;
}

/*
 * Call a function in all objects in a vector.
 */
struct vector *call_all_other(v, func, numargs)
     struct vector *v;
     char *func;
     int numargs;
{
  extern int onerror();
  extern int remove_onerror();
  int onerror_num;
  int idx;
  struct svalue *tmp;
  struct vector *ret;

  ret = allocate_array(v->size);
  onerror_num=onerror(free_vector, ret);
  tmp = sp;
  for (idx = 0; idx < v->size; idx++)
  {
    struct object *ob;
    int i;
    switch(v->item[idx].type)
    {
    default:
      continue;
    case T_STRING:
      ob = find_object(v->item[idx].u.string);
      break;

    case T_OBJECT:
      ob = v->item[idx].u.ob;
      break;
    }

    if(!ob || (ob->flags & O_DESTRUCTED))
      continue;

    for (i = numargs; i--; )  push_svalue(tmp - i);

    if (apply_low(func, ob, numargs))
    {
      assign_svalue_no_free(&ret->item[idx], sp);
      pop_stack();
    }
  }
  remove_onerror(onerror_num);
  pop_n_elems(numargs);
  return ret;
}

/*
 * This function is similar to apply(), except that it will not
 * call the function, only return object name if the function exists,
 * or 0 otherwise.
 */
char *function_exists(fun, ob)
    char *fun;
    struct object *ob;
{
  struct function *pr;

#ifdef DEBUG
  if (ob->flags & O_DESTRUCTED)
    fatal("function_exists() on destructed object\n");
#endif
#if TIME_TO_SWAP > 0
  if (ob->flags & O_SWAPPED)
    load_ob_from_swap(ob);
#endif
  pr = ob->prog->functions;
  for(; pr < ob->prog->functions + ob->prog->num_functions; pr++) {
    struct program *progp;

    if (pr->name[0] != fun[0] || strcmp(pr->name, fun) != 0)
      continue;

    /* Static functions may not be called from outside. */
    if ((pr->type & TYPE_MOD_STATIC) && current_object != ob)
      continue;
    if (pr->flags & NAME_UNDEFINED)
      return 0;
    
    for (progp = ob->prog; pr->flags & NAME_INHERITED;) {
      progp = progp->inherit[pr->inherit_offset].prog;
      pr = &progp->functions[pr->offset];
    }
    return progp->name;
  }
  return 0;
}

/*
 * Call a specific function address in an object. This is done with no
 * frame set up. It is expected that there are no arguments. Returned
 * values are removed.
 */

void call_function(progp, pr)
    struct program *progp;
    struct function *pr;
{
    int csave=eval_cost;
    int cccsave=ccsave;

    if (pr->flags & NAME_UNDEFINED)
	return;
    push_control_stack(pr);
#ifdef DEBUG
    if (csp != control_stack)
	fatal("call_function with bad csp\n");
#endif
    csp->num_local_variables = 0;
    current_prog = progp;
    pr = setup_new_frame(pr);
    previous_ob = current_object;
    tracedepth = 0;
    eval_instruction(current_prog->program + pr->offset);
    pop_stack();	/* Throw away the returned result */
    csave-=eval_cost+cccsave-ccsave;
    current_object->cpu-=csave;
    if(current_object->user) current_object->user->cost-=csave;
    ccsave-=csave;
}

/*
 * This can be done much more efficiently, but the fix has
 * low priority.
 */
static int get_line_number(char *p,struct program *progp)
{
  unsigned char *l;
  int offset,tmp;
  int i;
  if (progp == 0)
    return 0;
  offset = p - progp->program;
#ifdef DEBUG
  if (offset > progp->program_size)
    fatal("Illegal offset %d in object %s\n", offset, progp->name);
#endif
#ifdef SHORT_LINE_NUMBERS
  for(tmp=i=0, l=progp->line_numbers;offset > tmp; i++)
  {
    int tmp2;
    tmp2=EXTRACT_UCHAR(l);
    l++;
    if(tmp2==255)
    {
      tmp+=EXTRACT_UCHAR(l)<<8;
      l++;
      tmp+=EXTRACT_UCHAR(l);
      l++;
    }else{
      tmp+=tmp2;
    }
  }
#else  
  for (i=0; offset > progp->line_numbers[i]; i++)
    ;
#endif
  return i + 1;
}

/*
 * Write out a trace. If there is an heart_beat(), then return the
 * object that had that heart beat.
 */
char *dump_trace(how)
    int how;
{
    struct control_stack *p;
    char *ret = 0;
#ifdef DEBUG
    int last_instructions PROT((void));
#endif

    if (current_prog == 0)
	return 0;
    if (csp < &control_stack[0]) {
	(void)printf("No trace.\n");
	debug_message("No trace.\n");
	return 0;
    }
#ifdef DEBUG
#ifdef TRACE_CODE
    if (how)
	(void)last_instructions();
#endif
#endif
    for (p = &control_stack[0]; p < csp; p++) {
	(void)printf("'%15s' in '%20s' ('%20s')line %d\n",
		     p[0].funp ? p[0].funp->name : "CATCH",
		     p[1].prog->name, p[1].ob->name,
		     get_line_number(p[1].pc, p[1].prog));
	debug_message("'%15s' in '%20s' ('%20s')line %d\n",
		     p[0].funp ? p[0].funp->name : "CATCH",
		     p[1].prog->name, p[1].ob->name,
		     get_line_number(p[1].pc, p[1].prog));
	if (p->funp && strcmp(p->funp->name, "heart_beat") == 0)
	    ret = p->ob?p->ob->name:0; /*crash unliked gc*/
    }
    (void)printf("'%15s' in '%20s' ('%20s')line %d\n",
		 p[0].funp ? p[0].funp->name : "CATCH",
		 current_prog->name, current_object->name,
		 get_line_number(pc, current_prog));
    debug_message("'%15s' in '%20s' ('%20s')line %d\n",
		 p[0].funp ? p[0].funp->name : "CATCH",
		 current_prog->name, current_object->name,
		 get_line_number(pc, current_prog));
    return ret;
}

int get_line_number_if_any() {
    if (current_prog)
	return get_line_number(pc, current_prog);
    return 0;
}


/* test stuff ... -- LA */
#ifdef OPCPROF
void opcdump()
{
    int i;

    for(i = 0; i < MAXOPC; i++)
	if (opcount[i]) printf("%d: %d\n", i, opcount[i]);
}
#endif

/*
 * Reset the virtual stack machine.
 */
void reset_machine(first)
    int first;
{
  csp = control_stack - 1;
  if (first)
    sp = start_of_stack - 1;
  else
    pop_n_elems(sp - start_of_stack + 1);
}

#ifdef TRACE_CODE

static char *get_arg(a, b)
    int a, b;
{
    static char buff[10];
    char *from, *to;

    from = previous_pc[a]; to = previous_pc[b];
    if (to - from < 2)
	return "";
    if (to - from == 2) {
	sprintf(buff, "%d", from[1]);
	return buff;
    }
    if (to - from == 3) {
	short arg;
	((char *)&arg)[0] = from[1];
	((char *)&arg)[1] = from[2];
	sprintf(buff, "%d", arg);
	return buff;
    }
    if (to - from == 5) {
	int arg;
	((char *)&arg)[0] = from[1];
	((char *)&arg)[1] = from[2];
	((char *)&arg)[2] = from[3];
	((char *)&arg)[3] = from[4];
	sprintf(buff, "%d", arg);
	return buff;
    }
    return "";
}

int last_instructions() {
    int i;
    i = last;
    do {
	if (previous_instruction[i] != 0)
	    printf("%6x: %3d %8s %-25s (%d)\n", previous_pc[i],
		   previous_instruction[i],
		   get_arg(i, (i+1) %
			   (sizeof previous_instruction / sizeof (int))),
		   get_f_name(previous_instruction[i]),
		   stack_size[i] + 1);
	i = (i + 1) % (sizeof previous_instruction / sizeof (int));
    } while (i != last);
    return last;
}

#endif /* TRACE_CODE */


#ifdef DEBUG

static void count_inherits(progp, search_prog)
    struct program *progp, *search_prog;
{
    int i;

    /* Clones will not add to the ref count of inherited progs */
    if (progp->extra_ref != 1) return; /* marion */
    for (i=0; i< progp->num_inherited; i++) {
	progp->inherit[i].prog->extra_ref++;
	if (progp->inherit[i].prog == search_prog)
	    printf("Found prog, inherited by %s\n", progp->name);
	count_inherits(progp->inherit[i].prog, search_prog);
    }
}

static void count_ref_in_vector(svp, num)
    struct svalue *svp;
    int num;
{
   struct svalue *p;
   for(p = svp; p < svp + num; p++)
   {
      switch(p->type)
      {
         case T_OBJECT:
	    p->u.ob->extra_ref++;
	    continue;
	 case T_POINTER:
	    count_ref_in_vector(&p->u.vec->item[0], p->u.vec->size);
	    p->u.vec->extra_ref++;
	    continue;
	 case T_MAPPING:
	    count_ref_in_vector(&p->u.vec->item[0], p->u.vec->size);
	    p->u.vec->extra_ref++;
	    continue;
      }
   }
}

/*
 * Clear the extra debug ref count for vectors
 */
void clear_vector_refs(svp, num)
    struct svalue *svp;
    int num;
{
   struct svalue *p;
   for(p = svp; p < svp + num; p++)
   {
      switch(p->type)
      {
         case T_POINTER:
	    clear_vector_refs(&p->u.vec->item[0], p->u.vec->size);
	    p->u.vec->extra_ref = 0;
	    continue;
	 case T_MAPPING:
	    clear_vector_refs(&p->u.vec->item[0], p->u.vec->size);
	    p->u.vec->extra_ref = 0;
	    continue;
      }
   }
}

/*
 * Loop through every object and variable in the game and check
 * all reference counts. This will surely take some time, and should
 * only be used for debugging.
 */
void check_a_lot_ref_counts(search_prog)
    struct program *search_prog;
{
    extern struct object *master_ob;
    struct object *ob;

    /*
     * Pass 1: clear the ref counts.
     */
    for (ob=obj_list; ob; ob = ob->next_all) {
	ob->extra_ref = 0;
	ob->prog->extra_ref = 0;
	clear_vector_refs(ob->variables, ob->prog->num_variables);
    }
    clear_vector_refs(start_of_stack, sp - start_of_stack + 1);

    /*
     * Pass 2: Compute the ref counts.
     */

    /*
     * List of all objects.
     */
    for (ob=obj_list; ob; ob = ob->next_all) {
	ob->extra_ref++;
	count_ref_in_vector(ob->variables, ob->prog->num_variables);
	ob->prog->extra_ref++;
	if (ob->prog == search_prog)
	    printf("Found program for object %s\n", ob->name);
	/* Clones will not add to the ref count of inherited progs */
	if (ob->prog->extra_ref == 1)
	    count_inherits(ob->prog, search_prog);
    }

    /*
     * The current stack.
     */
    count_ref_in_vector(start_of_stack, sp - start_of_stack + 1);
    update_ref_counts_for_players();
    count_ref_from_call_outs();
    if (master_ob) master_ob->extra_ref++; /* marion */

    if (search_prog)
	return;

    /*
     * Pass 3: Check the ref counts.
     */
    for (ob=obj_list; ob; ob = ob->next_all) {
	if (ob->ref != ob->extra_ref)
 	    fatal("Bad ref count in object %s, %d - %d\n", ob->name,
		  ob->ref, ob->extra_ref);
	if (ob->prog->ref != ob->prog->extra_ref) {
	    check_a_lot_ref_counts(ob->prog);
	    fatal("Bad ref count in prog %s, %d - %d\n", ob->prog->name,
		  ob->prog->ref, ob->prog->extra_ref);
	}
    }
}

#endif /* DEBUG */

/* Generate a debug message to the player */
static void
do_trace(msg, fname, post)
char *msg, *fname, *post;
{
    char buf[10000];
    char *objname;

    if (!TRACEHB)
	return;
    objname = TRACETST(TRACE_OBJNAME) ? (current_object && current_object->name ? current_object->name : "??")  : "";
    sprintf(buf, "*** %d %*s %s %s %s%s", tracedepth, tracedepth, "", msg, objname, fname, post);
    add_message(buf);
}

struct svalue *apply_master_ob(fun, num_arg)
    char *fun;
    int num_arg;
{
    extern struct object *master_ob;

    assert_master_ob_loaded();
    /*
     * Maybe apply() should be called instead ?
     */
    return sapply(fun, master_ob, num_arg);
}

void assert_master_ob_loaded()
{
    extern struct object *master_ob;
    static int inside = 0;
#ifndef COMPAT_MODE
    struct svalue *ret;
#endif

    if (master_ob == 0 || master_ob->flags & O_DESTRUCTED) {
	/*
	 * The master object has been destructed. Free our reference,
	 * and load a new one.
	 *
	 * This test is needed because the master object is called from
	 * yyparse() at an error to find the wizard name. However, and error
	 * when loading the master object will cause a recursive call to this
	 * point.
	 *
	 * The best solution would be if the yyparse() did not have to call
	 * the master object to find the name of the wizard.
	 */
	if (inside) {
	    fprintf(stderr, "Failed to load master object.\n");
	    add_message("Failed to load master file !\n");
	    exit(1);
	}
	fprintf(stderr, "assert_master_ob_loaded: Reloading master.c\n");
	if (master_ob)
	    free_object(master_ob, "assert_master_ob_loaded");
	/*
	 * Clear the pointer, in case the load failed.
	 */
	master_ob = 0;
	inside = 1;
#ifdef COMPAT_MODE
	    master_ob = load_object("obj/master",0,0);
#else
	    master_ob = load_object("secure/master",0,0);

	    ret = apply_master_ob("get_root_uid", 0);
	    if (ret == 0 || ret->type != T_STRING) {
		fatal ("get_root_uid() in secure/master.c does not work\n");
	    }
	    master_ob->user = add_name(ret->u.string);
	    master_ob->eff_user = master_ob->user;
#endif
	inside = 0;
	add_ref(master_ob, "assert_master_ob_loaded");
	fprintf(stderr, "Reloading done.\n");
    }
}

/*
 * When an object is destructed, all references to it must be removed
 * from the stack.
 */
void remove_object_from_stack(ob)
    struct object *ob;
{
    struct svalue *svp;

    for (svp = start_of_stack; svp <= sp; svp++) {
	if (svp->type != T_OBJECT)
	    continue;
	if (svp->u.ob != ob)
	    continue;
	free_object(svp->u.ob, "remove_object_from_stack");
	svp->type = T_NUMBER;
	svp->u.number = 0;
    }
}

static int
strpref(p, s)
char *p, *s;
{
    while (*p)
	if (*p++ != *s++)
	    return 0;
    return 1;
}
