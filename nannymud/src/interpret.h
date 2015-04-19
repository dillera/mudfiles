union u {
    char *string;
    int number;
    struct object *ob;
    struct vector *vec;
    struct svalue *lvalue;
    struct index_type *ind;
};

/*
 * This struct stores a index_type, which is an lvalue storing a string or
 * an array together with starting and ending interval. Index_types are used
 * when a range is assigned a new value (e.g. foo[n..m]=bar) or when a char
 * in a string is assigned a new value (because of the use of shared strings
 * it's not possible to make charpointer). The index_string can have the types
 * T_NUMBER (char in string), T_STRING (range of a string) and T_POINTER (range
 * of an array) /Wing 920325
 */
struct index_type {
    short from;
    short to;
    short type;
    struct svalue *ptr;
};

/*
 * The value stack element.
 * If it is a string, then the way that the string has been allocated differ,
 * wich will affect how it should be freed.
 */
struct svalue {
    short type;
    short string_type;
    union u u;
};

#define T_INVALID	0x0
#define T_LVALUE	0x1
#define T_NUMBER	0x2
#define T_STRING	0x4
#define T_POINTER	0x8
#define T_OBJECT	0x10
#define T_MAPPING	0x20
#define T_INDEX_TYPE    0x40

#define STRING_MALLOC	0	/* Allocated by malloc() */
#define STRING_CONSTANT	1	/* Do not has to be freed at all */
#define STRING_SHARED	2	/* Allocated by the shared string library */

#define VECTOR_CLEAN 1

struct vector {
#ifndef NO_ARRAY_LINKS
  struct vector *prev_all;
  struct vector *next_all;
#endif
  short size;
  short flags; /* not yet used */
  int ref;
#ifdef DEBUG
  int extra_ref;
#endif
  struct wiz_list *user;	/* Save who made the vector */
  struct svalue item[1];
};

#define ALLOC_VECTOR(nelem) \
    (struct vector *)xalloc(sizeof (struct vector) + \
			    sizeof(struct svalue) * (nelem - 1))

struct lnode_def;
void free_vector PROT((struct vector *)), free_all_values();

/*
 * Control stack element.
 * 'prog' is usually same as 'ob->prog' (current_object), except when
 * when the current function is defined by inheritance.
 * The pointer, csp, will point to the values that will be used at return.
 */
struct control_stack {
    struct object *ob;		/* Current object */
    struct object *prev_ob;	/* Save previous object */
    struct program *prog;	/* Current program */
    int num_local_variables;	/* Local + arguments */
    char *pc;
    struct svalue *fp;
    int extern_call;		/* Flag if evaluator should return */
    struct function *funp;	/* Only used for tracebacks */
    int function_index_offset;	/* Used when executing functions in inherited
				   programs */
    int variable_index_offset;	/* Same */
};

#define IS_ZERO(x) (!(x) || (((x)->type == T_NUMBER) && ((x)->u.number == 0)))
#define IS_UNDEFINED(x) (!(x) || (((x)->type == T_NUMBER) && \
				          ((x)->subtype == T_UNDEFINED) && ((x)->u.number == 0)))
#define IS_NULL(x) (!(x) || (((x)->type == T_NUMBER) && \
			             ((x)->subtype == T_NULLVALUE) && ((x)->u.number == 0)))
