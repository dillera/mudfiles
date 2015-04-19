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
#define CASE(x) case x: MARK(x);
#else
#define CASE(x) case x:
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
struct vector *filter_mapping PROT((struct vector*, char*, struct object*,
				    struct svalue*));
struct vector *map_mapping PROT((struct vector*, char*, struct object*,
				 struct svalue*));

extern struct object *master_ob;

extern struct vector *get_action PROT((struct object *, char *));
extern struct vector *get_all_actions PROT((struct object *));
extern void print_svalue PROT((struct svalue *));
struct svalue *sapply PROT((char *, struct object *, int));
static void do_trace PROT((char *, char *, char *));
static int apply_low PROT((char *, struct object *, int));
static int inter_sscanf PROT((int));
static int strpref PROT((char *, char *));
extern int do_rename PROT((char *, char *));     

extern int data_size PROT((struct object *));
extern push_malloced_string PROT((char *));

extern struct object *previous_ob;
extern char *last_verb;
extern struct svalue const0, const1;
struct program *current_prog;
extern int current_time;
extern int eval_cost;
extern struct object *current_heart_beat, *current_interactive;


static int i;
extern struct svalue *sp;

#ifdef SOCKET
void f_socket_create(int num_arg)
{
  int fd;
  struct svalue *arg, *ret;
  struct vector *info;

  arg = sp - num_arg + 1;

  info = allocate_array(4);
  info->ref--;
  info->item[0].type = T_NUMBER;
  info->item[0].u.number = -1;
  assign_socket_owner(info->item + 1, current_object);
  info->item[2].type = T_STRING;
  info->item[2].string_type = STRING_CONSTANT;
  info->item[2].u.string = "N/A";
  info->item[3].type = T_NUMBER;
  info->item[3].u.number = -1;
	  
  push_object(current_object);
  push_string("create", STRING_CONSTANT);
  push_vector(info);
	  
  ret = apply_master_ob("valid_socket", 3);
  if (!IS_ZERO(ret))
  {
    if (num_arg == 2)
      fd = socket_create(arg[0].u.number, NULL, arg[1].u.string);
    else
      fd = socket_create(arg[0].u.number, arg[1].u.string, arg[2].u.string);
    pop_n_elems(num_arg);	/* pop both args off stack    */
    push_number(fd);		/* push return int onto stack */
  }
  else
  {
    pop_n_elems(num_arg);	/* pop both args off stack    */
    push_number(EESECURITY);	/* Security violation attempted */
  }
}

void f_socket_bind(int num_arg)
{
  int fd, port, i;
  struct svalue *ret;
  struct vector *info;
  char addr[ADDR_BUF_SIZE];

  fd = (sp-1)->u.number;
  get_socket_address(fd, addr, &port);
	  
  info = allocate_array(4);
  info->ref--;
  info->item[0].type = T_NUMBER;
  info->item[0].u.number = fd;
  assign_socket_owner(&info->item[1], get_socket_owner(fd));
  info->item[2].type = T_STRING;
  info->item[2].string_type = STRING_CONSTANT;
  info->item[2].u.string = addr;
  info->item[3].type = T_NUMBER;
  info->item[3].u.number = port;
	  
  push_object(current_object);
  push_string("bind", STRING_CONSTANT);
  push_vector(info);

  ret = apply_master_ob("valid_socket", 3);
  if (!IS_ZERO(ret))
  {
    i = socket_bind(fd, sp->u.number);
    pop_n_elems(2);		/* pop both args off stack    */
    push_number(i);		/* push return int onto stack */
  }
  else
  {
    pop_n_elems(2);		/* pop both args off stack    */
    push_number(EESECURITY);	/* Security violation attempted */
  }
	  
}

void f_socket_listen(int num_arg)
{
  int fd, port, i;
  struct svalue *ret;
  struct vector *info;
  char addr[ADDR_BUF_SIZE];
	  
  fd = (sp-1)->u.number;
  get_socket_address(fd, addr, &port);
	  
  info = allocate_array(4);
  info->ref--;
  info->item[0].type = T_NUMBER;
  info->item[0].u.number = fd;
  assign_socket_owner(&info->item[1], get_socket_owner(fd));
  info->item[2].type = T_STRING;
  info->item[2].string_type = STRING_CONSTANT;
  info->item[2].u.string = addr;
  info->item[3].type = T_NUMBER;
  info->item[3].u.number = port;

  push_object(current_object);
  push_string("listen",STRING_CONSTANT);
  push_vector(info);
	  
  ret = apply_master_ob("valid_socket",3);
  if (!IS_ZERO(ret))
  {
    i = socket_listen(fd, sp->u.string);
    pop_n_elems(2);		/* pop both args off stack    */
    push_number(i);		/* push return int onto stack */
  }
  else
  {
    pop_n_elems(2);		/* pop both args off stack    */
    push_number(EESECURITY);	/* Security violation attempted */
  }
}

void f_socket_accept(int num_arg)
{
  int fd, port, i;
  struct svalue *ret;
  struct vector *info;
  char addr[ADDR_BUF_SIZE];
	  
  fd = (sp-2)->u.number;
  get_socket_address(fd, addr, &port);
	  
  info = allocate_array(4);
  info->ref--;
  info->item[0].type = T_NUMBER;
  info->item[0].u.number = fd;
  assign_socket_owner(&info->item[1], get_socket_owner(fd));
  info->item[2].type = T_STRING;
  info->item[2].string_type = STRING_CONSTANT;
  info->item[2].u.string = addr;
  info->item[3].type = T_NUMBER;
  info->item[3].u.number = port;
	  
  push_object(current_object);
  push_string("accept", STRING_CONSTANT);
  push_vector(info);
	  
  ret = apply_master_ob("valid_socket", 3);
  if (!IS_ZERO(ret))
  {
    i = socket_accept(fd, (sp-1)->u.string, sp->u.string);
    pop_n_elems(3);		/* pop both args off stack    */
    push_number(i);		/* push return int onto stack */
  }
  else
  {
    pop_n_elems(3);		/* pop both args off stack    */
    push_number(EESECURITY);	/* Security violation attempted */
  }
}

void f_socket_connect(int num_arg)
{
  int fd, port, i;
  struct svalue *ret;
  struct vector *info;
  char addr[ADDR_BUF_SIZE];
	  
  fd = (sp-3)->u.number;
  get_socket_address(fd, addr, &port);

  info = allocate_array(4);
  info->ref--;
  info->item[0].type = T_NUMBER;
  info->item[0].u.number = fd;
  assign_socket_owner(&info->item[1], get_socket_owner(fd));
  info->item[2].type = T_STRING;
  info->item[2].string_type = STRING_CONSTANT;
  info->item[2].u.string = addr;
  info->item[3].type = T_NUMBER;
  info->item[3].u.number = port;
	  
  push_object(current_object);
  push_string("connect", STRING_CONSTANT);
  push_vector(info);
	  
  ret = apply_master_ob("valid_socket", 3);
  if (!IS_ZERO(ret))
  {
    i = socket_connect(fd, (sp-2)->u.string, (sp-1)->u.string,
		       sp->u.string);
    pop_n_elems(4);		/* pop all args off stack     */
    push_number(i);		/* push return int onto stack */
  }
  else
  {
    pop_n_elems(4);		/* pop all args off stack     */
    push_number(EESECURITY);	/* Security violation attempted */
  }
}


void f_socket_write(int num_arg)
{
  int fd, port, i;
  struct svalue *arg, *ret;
  struct vector *info;
  char addr[ADDR_BUF_SIZE];
	  
  arg = sp - num_arg + 1;
  fd = arg[0].u.number;
  get_socket_address(fd, addr, &port);
	  
  info = allocate_array(4);
  info->ref--;
  info->item[0].type = T_NUMBER;
  info->item[0].u.number = fd;
  assign_socket_owner(&info->item[1], get_socket_owner(fd));
  info->item[2].type = T_STRING;
  info->item[2].string_type = STRING_CONSTANT;
  info->item[2].u.string = addr;
  info->item[3].type = T_NUMBER;
  info->item[3].u.number = port;
	  
  push_object(current_object);
  push_string("write", STRING_CONSTANT);
  push_vector(info);

  ret = apply_master_ob("valid_socket", 3);
  if (!IS_ZERO(ret))
  {
    i = socket_write(fd, &arg[1],
		     (num_arg == 3) ? arg[2].u.string : (char *)NULL);
    pop_n_elems(num_arg);	/* pop both args off stack    */
    push_number(i);		/* push return int onto stack */
  }
  else
  {
    pop_n_elems(num_arg);	/* pop both args off stack    */
    push_number(EESECURITY);	/* Security violation attempted */
  }
}

void f_socket_close(int num_arg)
{
  int fd, port, i;
  struct svalue *ret;
  struct vector *info;
  char addr[ADDR_BUF_SIZE];
	  
  fd = sp->u.number;
  get_socket_address(fd, addr, &port);
	  
  info = allocate_array(4);
  info->ref--;
  info->item[0].type = T_NUMBER;
  info->item[0].u.number = fd;
  assign_socket_owner(&info->item[1], get_socket_owner(fd));
  info->item[2].type = T_STRING;
  info->item[2].string_type = STRING_CONSTANT;
  info->item[2].u.string = addr;
  info->item[3].type = T_NUMBER;
  info->item[3].u.number = port;
	  
  push_object(current_object);
  push_string("close", STRING_CONSTANT);
  push_vector(info);
	  
  ret = apply_master_ob("valid_socket", 3);
  if (!IS_ZERO(ret))
  {
    i = socket_close(fd);
    pop_stack();		/* pop int arg off stack      */
    push_number(i);		/* push return int onto stack */
  }
  else
  { 
    pop_stack();		/* pop int arg off stack      */
    push_number(EESECURITY);	/* Security violation attempted */
  }
}

void f_socket_release(int num_arg)
{
  int fd, port, i;
  struct svalue *ret;
  struct vector *info;
  char addr[ADDR_BUF_SIZE];
	  
  fd = (sp-2)->u.number;
  get_socket_address(fd, addr, &port);
	  
  info = allocate_array(4);
  info->ref--;
  info->item[0].type = T_NUMBER;
  info->item[0].u.number = fd;
  assign_socket_owner(&info->item[1], get_socket_owner(fd));
  info->item[2].type = T_STRING;
  info->item[2].string_type = STRING_CONSTANT;
  info->item[2].u.string = addr;
  info->item[3].type = T_NUMBER;
  info->item[3].u.number = port;
	  
  push_object(current_object);
  push_string("release", STRING_CONSTANT);
  push_vector(info);
	  
  ret = apply_master_ob("valid_socket", 3);
  if (!IS_ZERO(ret))
  {
    i = socket_release((sp-2)->u.number, (sp-1)->u.ob, sp->u.string);
    pop_n_elems(3);		/* pop all args off stack     */
    push_number(i);		/* push return int onto stack */
  }
  else
  {
    pop_n_elems(3);		/* pop all args off stack     */
    push_number(EESECURITY);	/* Security violation attempted */
  }
}

void f_socket_acquire(int num_arg)
{
  int fd, port, i;
  struct svalue *ret;
  struct vector *info;
  char addr[ADDR_BUF_SIZE];
	  
  fd = (sp-1)->u.number;
  get_socket_address(fd, addr, &port);
	  
  info = allocate_array(4);
  info->ref--;
  info->item[0].type = T_NUMBER;
  info->item[0].u.number = fd;
  assign_socket_owner(&info->item[1], get_socket_owner(fd));
  info->item[2].type = T_STRING;
  info->item[2].string_type = STRING_CONSTANT;
  info->item[2].u.string = addr;
  info->item[3].type = T_NUMBER;
  info->item[3].u.number = port;
	  
  push_object(current_object);
  push_string("acquire", STRING_CONSTANT);
  push_vector(info);

  ret = apply_master_ob("valid_socket", 3);
  if (!IS_ZERO(ret))
  {
    i = socket_acquire((sp-3)->u.number, (sp-2)->u.string,
		       (sp-1)->u.string, sp->u.string);
    pop_n_elems(4);		/* pop both args off stack    */
    push_number(i);		/* push return int onto stack */
  }
  else
  {
    pop_n_elems(4);		/* pop both args off stack    */
    push_number(EESECURITY);	/* Security violation attempted */
  }
}

void f_socket_error(int num_arg)
{
  extern char *socket_error();
  char *error;
	  
  error = socket_error(sp->u.number);
  pop_stack();			/* pop int arg off stack      */
  push_string(error, STRING_CONSTANT);

}

void f_socket_address(int num_arg)
{
  char *str;
  int port;
  char addr[ADDR_BUF_SIZE];
	  
  get_socket_address(sp->u.number, addr, &port);
  str = (char *)xalloc(strlen(addr) + 5 + 3);
  sprintf(str, "%s %d", addr, port);
  pop_stack();
  push_string(str, STRING_MALLOC);
}

void f_dump_socket_status(int num_arg)
{
  dump_socket_status();
  push_number(0);
}

#endif /* SOCKET */


float check_memory_usage(s,num)
    struct svalue *s;
    int num;
{
  float size; /* grin */

  size=0.0;
  eval_cost+=num>>2+1;  /* floats aren't cheap */
  while(num)
  {
    size+=(float)sizeof(struct svalue);
    switch(s->type)
    {
      case T_STRING:
        switch(s->string_type)
        {
          case STRING_MALLOC: size+=(float)strlen(s->u.string); break;
          case STRING_CONSTANT: break;
          case STRING_SHARED:
            size+=((float)strlen(s->u.string))/
	      (float)print_string_refs(s->u.string);
            /* Let's ignore the shared string overhead,
             * we still save memory with shared strings.
             */
        }
        break;
  
      case T_OBJECT:
      case T_NUMBER:
      case T_LVALUE:
        break;

      case T_MAPPING:
      case T_POINTER:
        size+=((float)(sizeof(struct vector)-sizeof(struct svalue))+
		check_memory_usage(s->u.vec->item,s->u.vec->size))
			/(float)(s->u.vec->ref);
        break;

    }

    num--;

    s++;
  }
  return size;
}

#ifdef F_ORDER_ALIST
void f_order_alist(int num_arg)
{
  extern struct vector *order_alist PROT((struct vector *));
  struct svalue *args;
  struct vector *list;
  int listsize,keynum;
  
  if (num_arg == 1 && sp->u.vec->size 
      && sp->u.vec->item[0].type == T_POINTER) {
    args     = sp->u.vec->item;
    listsize = sp->u.vec->size;
  } else {
    args = sp-num_arg+1;
    listsize = num_arg;
  }
  keynum = args[0].u.vec->size;
  list = allocate_array(listsize);
  for (i=0; i<listsize; i++) {
    if (args[i].type != T_POINTER
	|| args[i].u.vec->size != keynum) {
      free_vector(list);
      error("bad data array %d in call to order_alist",i);
    }
    list->item[i].type = T_POINTER;
    list->item[i].u.vec = slice_array(args[i].u.vec,0,keynum-1);
  }
  pop_n_elems(num_arg);
  sp++;
  sp->type = T_POINTER;
  sp->u.vec = order_alist(list);
  free_vector(list);
}
#endif /* F_ORDER_ALIST */

#ifdef F_INSERT_ALIST
void f_insert_alist(int num_arg)
{
  /* When the key list of an alist contains destructed objects
     it is better not to free them till the next reordering by
     order_alist to retain the alist property.
     */
  extern struct svalue *insert_alist
    PROT((struct svalue *key,struct svalue *key_data,
	  struct vector *list));
  struct vector *list;
  int listsize,keynum;
  struct svalue *key,*key_data,*ret;
  static struct vector tempvec = {
#ifndef NO_ARRAY_LINKS
     NULL,
     NULL,
#endif
     1,1,
  };
  
  if (sp->type != T_POINTER)
    bad_arg(num_arg,F_INSERT_ALIST);
  if ( !(listsize = sp->u.vec->size) ||
      sp->u.vec->item[0].type != T_POINTER ) {
    list = &tempvec;
    assign_svalue_no_free(list->item,sp);
    listsize = 1;
  } else
    list = sp->u.vec;
  keynum = list->item[0].u.vec->size;
  for (i=1; i<listsize; i++) {
    if (list->item[i].type != T_POINTER
	||list->item[i].u.vec->size != keynum)
      bad_arg(num_arg,F_INSERT_ALIST);
  }
  if (num_arg == 2) {
    if (sp[-1].type != T_POINTER) {
      key_data = (struct svalue*)NULL;
      key = sp-1;
    } else {
      if (sp[-1].u.vec->size != listsize)
	bad_arg(1,F_INSERT_ALIST);
      key_data = key = sp[-1].u.vec->item;
    }
  } else {
    if (num_arg - 1 != listsize)
      bad_arg(1,F_INSERT_ALIST);
    key_data = key = sp-num_arg+1;
  }
  ret = insert_alist(key,key_data,list);
  pop_n_elems(num_arg);
  sp++;
  *sp = *ret;
}
#endif /* F_INSERT_ALIST */

#ifdef F_ASSOC
void f_assoc(int num_arg)
{
  /* When the key list of an alist contains destructed objects
     it is better not to free them till the next reordering by
     order_alist to retain the alist property.
     */
  int assoc PROT((struct svalue *key, struct vector *keys));
  struct svalue *args = sp -num_arg +1;
  struct vector *keys,*data;
  struct svalue *fail_val;
  int ix;
  
  if ( !args[1].u.vec->size ||
      args[1].u.vec->item[0].type != T_POINTER ) {
    keys = args[1].u.vec;
    if (num_arg == 2) {
      data = (struct vector *)NULL;
    } else {
      if (args[2].type != T_POINTER ||
	  args[2].u.vec->size != keys->size) {
	bad_arg(3,F_ASSOC);
      }
      data = args[2].u.vec;
    }
    if (num_arg == 4) {
      fail_val = &args[3];
    } else {
      fail_val = &const0;
    }
  } else {
    keys = args[1].u.vec->item[0].u.vec;
    if (args[1].u.vec->size > 1) {
      if (args[1].u.vec->item[1].type != T_POINTER ||
	  args[1].u.vec->item[1].u.vec->size != keys->size) {
	bad_arg(2,F_ASSOC);
      }
      data = args[1].u.vec->item[1].u.vec;
    } else {
      data = (struct vector *)NULL;
    }
    if (num_arg == 3) fail_val = &args[2];
    else if (num_arg == 2) fail_val = &const0;
    else {
      error ("too many args to efun assoc");
    }
  }
  ix = assoc(&args[0],keys);
  if (data == (struct vector *)NULL) {
    pop_n_elems(num_arg);
    push_number(ix);
  } else {
    assign_svalue(args, ix==-1 ? fail_val : &data->item[ix]);
    pop_n_elems(num_arg-1);
  }
}
#endif /* F_ASSOC */

void f_set_bit()
{
  char *str;
  int len, old_len, ind;
  
  if (sp->u.number > MAX_BITS)
    error("set_bit: too big bit number: %d\n", sp->u.number);
  if(sp->u.number < 0)
    error("set_bit with negative bit number.\n");
  
  len = strlen((sp-1)->u.string);
  old_len = len;
  ind = sp->u.number/6;
  if (ind >= len)
    len = ind + 1;
  str = xalloc(len+1);
  str[len] = '\0';
  if (old_len)
    memcpy(str, (sp-1)->u.string, old_len);
  if (len > old_len)
    memset(str + old_len, ' ', len - old_len);
  if (str[ind] > 0x3f + ' ' || str[ind] < ' ')
    error("Illegal bit pattern in set_bit character %d\n", ind);
  str[ind] = (str[ind] - ' ' | 1 << sp->u.number % 6) + ' ';
  pop_n_elems(2);
  sp++;
  sp->u.string = str;
  sp->string_type = STRING_MALLOC;
  sp->type = T_STRING;
}

void f_clear_bit()
{
  char *str;
  int len, ind;
  
  if (sp->u.number > MAX_BITS)
    error("clear_bit: too big bit number: %d\n", sp->u.number);
  if(sp->u.number < 0)
    error("clear_bit with negative bit number.\n");
  len = strlen((sp-1)->u.string);
  ind = sp->u.number/6;
  if (ind >= len) {
    /* Return first argument unmodified ! */
    pop_stack();
    return;
  }
  str = xalloc(len+1);
  memcpy(str, (sp-1)->u.string, len+1);	/* Including null byte */
  if (str[ind] > 0x3f + ' ' || str[ind] < ' ')
    error("Illegal bit pattern in clear_bit character %d\n", ind);
  str[ind] = (str[ind] - ' ' & ~(1 << sp->u.number % 6)) + ' ';
  pop_n_elems(2);
  sp++;
  sp->type = T_STRING;
  sp->string_type = STRING_MALLOC;
  sp->u.string = str;
}

void f_test_bit()
{
  int len;
  
  len = strlen((sp-1)->u.string);
  if(sp->u.number < 0)
    error("test_bit with negative bit number.\n");
  if (sp->u.number/6 >= len) {
    pop_n_elems(2);
    push_number(0);
    return;
  }
  if ((sp-1)->u.string[sp->u.number/6] - ' ' & 1 << sp->u.number % 6) {
    pop_n_elems(2);
    push_number(1);
  } else {
    pop_n_elems(2);
    push_number(0);
  }
}

void f_filter_mapping(int num_arg)
{
  struct vector *v;
  struct svalue *arg;
  struct object *ob;
  
  arg = sp - num_arg + 1; ob = 0;
  
  if (arg[2].type == T_OBJECT)
    ob = arg[2].u.ob;
  else if (arg[2].type == T_STRING) 
    ob = find_object(arg[2].u.string);
  
  if (!ob)
    error("Bad third argument to filter_mapping()\n");
  
  if (arg[0].type == T_MAPPING) {
    check_map_for_destr(arg[0].u.vec);
    v = filter_mapping(arg[0].u.vec, arg[1].u.string, ob,
		       num_arg > 3 ? sp : (struct svalue *)0); 
  } else {
    v = 0;
  }
  
  pop_n_elems(num_arg);
  if (v) {
    push_mapping(v); /* This will make ref count == 2 */
    v->ref--;
  } else {
    push_number(0);
  }
}

void f_filter_array(int num_arg)
{
  struct object *ob;
  struct vector *v;
  struct svalue *arg;
  
  arg = sp - num_arg + 1; ob = 0;
  
  if (arg[2].type == T_OBJECT)
    ob = arg[2].u.ob;
  else if (arg[2].type == T_STRING) 
    ob = find_object(arg[2].u.string);
  
  if (!ob)
    error("Bad third argument to filter_array()\n");
  
  if (arg[0].type == T_POINTER) {
    check_for_destr(arg[0].u.vec);
    v = filter(arg[0].u.vec, arg[1].u.string, ob,
	       num_arg > 3 ? sp : (struct svalue *)0); 
  } else {
    v = 0;
  }
  
  pop_n_elems(num_arg);
  if (v) {
    push_vector(v); /* This will make ref count == 2 */
    v->ref--;
  } else {
    push_number(0);
  }
}

#if 1
int f_sscanf(int num_arg)
{
    struct svalue *arg = sp - num_arg + 1;
    char *fmt, *in_string;
    int skip_it, number_of_matches = 0;
    if(arg[0].type != T_STRING)
	bad_arg(1, F_SSCANF);
    in_string = arg[0].u.string;
    if(in_string == 0)
	return 0;
    if(arg[1].type != T_STRING)
	bad_arg(2, F_SSCANF);
    fmt = arg[1].u.string;
    for(num_arg -= 2, arg += 2; num_arg >= 0;
        number_of_matches++, num_arg--, arg++)
    {
        char *next_percent;
	int i, type;
        for(; fmt[0] && fmt[0] != '%'; fmt++, in_string++)
        {
           if(in_string[0] == '\0' || fmt[0] != in_string[0])
              return number_of_matches;
           if(fmt[1] == '%' && fmt[2] == '%')
           {
              if(in_string[1] != '%')
                 return number_of_matches;
              fmt += 2;
              in_string++;
           }
	}
	if(fmt[0] == '\0')
        {
	    if(in_string[0] && num_arg > 0)
            {
	      char *s;
	      s=make_shared_string(in_string);
	      free_svalue(arg->u.lvalue);
	      arg->u.lvalue->type = T_STRING;
	      arg->u.lvalue->u.string = s;
	      arg->u.lvalue->string_type = STRING_SHARED;
	      number_of_matches++;
	    }
	    break;
	}
#ifdef DEBUG
	if(fmt[0] != '%')
	    fatal("Should be a %% now !\n");
#endif
        fmt++;
        skip_it = 0;
	if(fmt[0] == '*')
        {
	    skip_it = 1;
	    fmt++;
	}
        if(!skip_it && num_arg < 1)
           error("Too few variables to sscanf().");  /*  Maybe continue;  */
	type = T_STRING;
	if(fmt[0] == 'd')
	    type = T_NUMBER;
	else if (fmt[0] != 's')
	    error("Bad type : '%%%c' in sscanf fmt string.", fmt[0]);
	fmt++;
	if(type == T_NUMBER)
        {
	    char *tmp = in_string;
	    int tmp_num;
	    tmp_num = (int) strtol(in_string, &in_string, 10);
	    if(tmp == in_string)
		break;
            if(!skip_it)
	    {
	       free_svalue(arg->u.lvalue);
	       arg->u.lvalue->type = T_NUMBER;
	       arg->u.lvalue->u.number = tmp_num;
	    }
            else
	    {
               arg--;
               num_arg++;
	    }
	    continue;
	}
	if(fmt[0] == '%')
	    error("Illegal to have 2 adjacent %%s in fmt string in sscanf.");
	if(fmt[0] == '\0')
        {
            if(!skip_it)
	    {
	      char *s;
	      s=make_shared_string(in_string);
	      free_svalue(arg->u.lvalue);
	      arg->u.lvalue->type = T_STRING;
	      arg->u.lvalue->u.string = s;
	      arg->u.lvalue->string_type = STRING_SHARED;
	    }
	    number_of_matches++;
	    break;
	}
        if((next_percent = strchr(fmt, '%')) == NULL)
           next_percent = fmt + strlen(fmt);
	for(i = 0; in_string[i]; i++)
        {
	    if(strncmp(in_string + i, fmt, next_percent - fmt) == 0)
            {
		char *match;
                if(!skip_it)
		{
		  match = xalloc(i + 1);
		   (void) strncpy(match, in_string, i);
		   match[i] = '\0';
		   free_svalue(arg->u.lvalue);
		   arg->u.lvalue->type = T_STRING;
		   arg->u.lvalue->u.string = match;
		   arg->u.lvalue->string_type = STRING_MALLOC;
		}
                else
		{
                   arg--;
                   num_arg++;
		}
		in_string += i + next_percent - fmt;
		fmt = next_percent;
		break;
	    }
	}
	if(fmt != next_percent)
	   break;
    }
    return number_of_matches;
}
#else
static char *find_percent(str)
    char *str;
{
    while(1) {
	str = strchr(str, '%');
	if (str == 0)
	    return 0;
	if (str[1] != '%')
	    return str;
	str++;
    }
}

int f_sscanf(int num_arg)
{
    char *fmt;		/* Format description */
    char *in_string;	/* The string to be parsed. */
    int number_of_matches;
    char *cp;
    struct svalue *arg = sp - num_arg + 1;

    /*
     * First get the string to be parsed.
     */
    if (arg[0].type != T_STRING)
	bad_arg(1, F_SSCANF);
    in_string = arg[0].u.string;
    if (in_string == 0)
	return 0;
    /*
     * Now get the format description.
     */
    if (arg[1].type != T_STRING)
	bad_arg(2, F_SSCANF);
    fmt = arg[1].u.string;
    /*
     * First, skip and match leading text.
     */
    for (cp=find_percent(fmt); fmt != cp; fmt++, in_string++) {
	if (in_string[0] == '\0' || fmt[0] != in_string[0])
	    return 0;
    }
    /*
     * Loop for every % or substring in the format. Update num_arg and the
     * arg pointer continuosly. Assigning is done manually, for speed.
     */
    num_arg -= 2;
    arg += 2;
    for (number_of_matches=0; num_arg > 0;
	 number_of_matches++, num_arg--, arg++) {
	int i, type;

	if (fmt[0] == '\0') {
	    /*
	     * We have reached end of the format string.
	     * If there are any chars left in the in_string,
	     * then we put them in the last variable (if any).
	     */
	    if (in_string[0]) {
		free_svalue(arg->u.lvalue);
		arg->u.lvalue->type = T_STRING;
		arg->u.lvalue->u.string = string_copy(in_string);
		arg->u.lvalue->string_type = STRING_MALLOC;
		number_of_matches++;
	    }
	    break;
	}
#ifdef DEBUG
	if (fmt[0] != '%')
	    fatal("Should be a %% now !\n");
#endif
	type = T_STRING;
	if (fmt[1] == 'd')
	    type = T_NUMBER;
	else if (fmt[1] != 's')
	    error("Bad type : '%%%c' in sscanf fmt string.", fmt[1]);
	fmt += 2;
	/*
	 * Parsing a number is the easy case. Just use strtol() to
	 * find the end of the number.
	 */
	if (type == T_NUMBER) {
	    char *tmp = in_string;
	    int tmp_num;

	    tmp_num = (int) strtol(in_string, &in_string, 10);
	    if(tmp == in_string) {
		/* No match */
		break;
	    }
	    free_svalue(arg->u.lvalue);
	    arg->u.lvalue->type = T_NUMBER;
	    arg->u.lvalue->u.number = tmp_num;
	    while(fmt[0] && fmt[0] == in_string[0])
		fmt++, in_string++;
	    if (fmt[0] != '%') {
		number_of_matches++;
		break;
	    }
	    continue;
	}
	/*
	 * Now we have the string case.
	 */
	cp = find_percent(fmt);
	if (cp == fmt)
	    error("Illegal to have 2 adjacent %%s in fmt string in sscanf.");
	if (cp == 0)
	    cp = fmt + strlen(fmt);
	/*
	 * First case: There was no extra characters to match.
	 * Then this is the last match.
	 */
	if (cp == fmt) {
	    free_svalue(arg->u.lvalue);
	    arg->u.lvalue->type = T_STRING;
	    arg->u.lvalue->u.string = string_copy(in_string);
	    arg->u.lvalue->string_type = STRING_MALLOC;
	    number_of_matches++;
	    break;
	}
	for (i=0; in_string[i]; i++) {
	    if (strncmp(in_string+i, fmt, cp - fmt) == 0) {
		char *match;
		/*
		 * Found a match !
		 */
		match = xalloc(i+1);
		(void)strncpy(match, in_string, i);
		in_string += i + cp - fmt;
		match[i] = '\0';
		free_svalue(arg->u.lvalue);
		arg->u.lvalue->type = T_STRING;
		arg->u.lvalue->u.string = match;
		arg->u.lvalue->string_type = STRING_MALLOC;
		fmt = cp;	/* Advance fmt to next % */
		break;
	    }
	}
	if (fmt == cp)	/* If match, then do continue. */
	    continue;
	/*
	 * No match was found. Then we stop here, and return
	 * the result so far !
	 */
	break;
    }
    return number_of_matches;
}
#endif


void f_map_array(int num_arg)
{
  struct vector *res;
  struct svalue *arg;
  struct object *ob;
  
  arg = sp - num_arg + 1; ob = 0;
  
  if (arg[2].type == T_OBJECT)
    ob = arg[2].u.ob;
  else if (arg[2].type == T_STRING) 
    ob = find_object(arg[2].u.string);
  
  if (!ob)
    bad_arg (3, F_MAP_ARRAY);
  
  if (arg[0].type == T_POINTER) {
    check_for_destr(arg[0].u.vec);
    res = map_array (arg[0].u.vec, arg[1].u.string, ob,
		     num_arg > 3 ? sp : (struct svalue *)0);
  } else {
    res = 0;
  }
  pop_n_elems (num_arg);
  simple_push_vector(res);
}

void f_map_mapping(int num_arg)
{
  struct vector *res;
  struct svalue *arg;
  struct object *ob;
  
  arg = sp - num_arg + 1; ob = 0;
  
  if (arg[2].type == T_OBJECT)
    ob = arg[2].u.ob;
  else if (arg[2].type == T_STRING) 
    ob = find_object(arg[2].u.string);
  
  if (!ob)
    bad_arg (3, F_MAP_MAPPING);
  
  if (arg[0].type == T_MAPPING) {
    check_map_for_destr(arg[0].u.vec);
    res = map_mapping (arg[0].u.vec, arg[1].u.string, ob,
		       num_arg > 3 ? sp : (struct svalue *)0);
  } else {
    res = 0;
  }
  pop_n_elems (num_arg);
  if (res) {
    push_mapping (res);	/* This will make ref count == 2 */
    res->ref--;
  } else
    push_number (0);
}

void f_sort_array(int num_arg)
{
  extern struct vector *sort_array
    PROT((struct vector*,char *,struct object *));
  struct vector *res;
  struct svalue *arg;
  struct object *ob;
  
  arg = sp - 2; ob = 0;
  
  if (arg[2].type == T_OBJECT)
    ob = arg[2].u.ob;
  else if (arg[2].type == T_STRING) 
    ob = find_object(arg[2].u.string);
  
  if (!ob)
    bad_arg (3, F_SORT_ARRAY);
  
  if (arg[0].type == T_POINTER) {
    /* sort_array already takes care of destructed objects */
    res = sort_array (
		      slice_array(arg[0].u.vec, 0, arg[0].u.vec->size-1),
		      arg[1].u.string, ob);
  } else
    res = 0;
  pop_n_elems (3);
  sp++;
  if (res) {
    sp->type = T_POINTER;
    sp->u.vec = res;
  }
  else     *sp = const0;
}

#ifdef F_DEBUG_INFO
void f_debug_info(int num_arg)
{
  struct object *ob;
  struct svalue *arg = sp-num_arg+1;
  struct svalue res;
  res=const0;
  
  switch ( arg[0].u.number ) {
  case 0:
  {
    int flags;
    struct object *obj2;
    
    if (num_arg != 2)
      error("bad number of arguments to debug_info");
    if ( arg[1].type != T_OBJECT)
      bad_arg(1, F_DEBUG_INFO);
    ob = arg[1].u.ob;
    flags = ob->flags;
    add_message("O_HEART_BEAT        : %s\n",
		flags&O_HEART_BEAT      ?"TRUE":"FALSE");
#ifdef O_IS_WIZARD
    add_message("O_IS_WIZARD         : %s\n",
		flags&O_IS_WIZARD       ?"TRUE":"FALSE");
#endif
#ifdef O_VIRTUAL
    add_message("O_VIRTUAL           : %s\n",
		flags & O_VIRTUAL       ?"TRUE":"FALSE");
#endif
    add_message("O_ENABLE_COMMANDS   : %s\n",
		flags&O_ENABLE_COMMANDS ?"TRUE":"FALSE");
    add_message("O_CLONE             : %s\n",
		flags&O_CLONE           ?"TRUE":"FALSE");
    add_message("O_DESTRUCTED        : %s\n",
		flags&O_DESTRUCTED      ?"TRUE":"FALSE");
#if TIME_TO_SWAP > 0
    add_message("O_SWAPPED           : %s\n",
		flags&O_SWAPPED          ?"TRUE":"FALSE");
#endif
    add_message("O_ONCE_INTERACTIVE  : %s\n",
		flags&O_ONCE_INTERACTIVE?"TRUE":"FALSE");
    add_message("O_APPROVED          : %s\n",
		flags&O_APPROVED        ?"TRUE":"FALSE");
    add_message("O_RESET_STATE       : %s\n",
		flags&O_RESET_STATE     ?"TRUE":"FALSE");
#if TIME_TO_CLEAN_UP > 0
    add_message("O_WILL_CLEAN_UP     : %s\n",
		flags&O_WILL_CLEAN_UP   ?"TRUE":"FALSE");
#endif
    add_message("O_IN_DESTRUCT       : %s\n",
		flags&O_IN_DESTRUCT   ?"TRUE":"FALSE");
    add_message("O_EFUN_SOCKET       : %s\n",
		flags&O_EFUN_SOCKET ?"TRUE":"FALSE");
    add_message("O_OBJECT_LOCKED     : %s\n",
		flags&O_OBJECT_LOCKED   ?"TRUE":"FALSE");
    add_message("O_NEVER_RESET       : %s\n",
		flags&O_NEVER_RESET   ?"TRUE":"FALSE");
    add_message("O_PROGRAM_THROWN    : %s\n",
		flags&O_PROGRAM_THROWN   ?"TRUE":"FALSE");
    add_message("O_PROGRAM_NOT_THROWN: %s\n",
		flags&O_PROGRAM_NOT_THROWN   ?"TRUE":"FALSE");
    
    add_message("total light : %d\n", ob->total_light);
    add_message("next_reset  : %d\n", ob->next_reset);
    add_message("time_of_ref : %d\n", ob->time_of_ref);
    add_message("ref         : %d\n", ob->ref);
#ifdef DEBUG
    add_message("extra_ref   : %d\n", ob->extra_ref);
#endif
#if TIME_TO_SWAP > 0
    add_message("swap_num    : %ld\n", ob->swap_num);
#endif
    add_message("name        : '%s'\n", ob->name);
    add_message("next_all    : OBJ(%s)\n",
		ob->next_all?ob->next_all->name:"NULL");
    if (obj_list == ob) add_message(
				    "This object is the head of the object list.\n");
    for (obj2=obj_list,i=1; obj2; obj2=obj2->next_all,i++)
      if (obj2->next_all == ob) {
	add_message(
		    "Previous object in object list: OBJ(%s)\n",
		    obj2->name);
	add_message("position in object list:%d\n",i);
      }
    assign_svalue_no_free(&res,&const0);
    break;
  }
  case 1: {
    if (num_arg != 2)
      error("bad number of arguments to debug_info");
    if ( arg[1].type != T_OBJECT)
      bad_arg(1,F_DEBUG_INFO);
    ob = arg[1].u.ob;
    
    add_message("program ref's %d\n", ob->prog->ref);
    add_message("Name %s\n", ob->prog->name);
    add_message("program size %d\n",
		ob->prog->program_size);
    add_message("num func's %d (%d) \n", ob->prog->num_functions
		,ob->prog->num_functions * sizeof(struct function));
    add_message("num strings %d\n", ob->prog->num_strings);
    add_message("num vars %d (%d)\n", ob->prog->num_variables
		,ob->prog->num_variables * sizeof(struct variable));
    add_message("num inherits %d (%d)\n", ob->prog->num_inherited
		,ob->prog->num_inherited * sizeof(struct inherit));
    add_message("total size %d\n",
		ob->prog->total_size+data_size(ob)); /* Mats 930322 */
    assign_svalue_no_free(&res,&const0);
    break;
  }
  case 2: {
    extern int print_string_refs();
    
    if(arg[1].type != T_STRING)
      bad_arg(1, F_DEBUG_INFO);
    add_message("Ref count for: %s is %d\n", arg[1].u.string, print_string_refs(arg[1].u.string));
    break;
  }
    
  case 3:
  {
    extern void count_malloced_strings(int);
    if(num_arg > 1 && arg[1].type==T_NUMBER)
    {
      count_malloced_strings(arg[1].u.number);
    }else{
      count_malloced_strings(0);
    }
    break;
  }
  default: bad_arg(1,F_DEBUG_INFO);
    
    
  case 4:
    if(num_arg>1)
    {
      switch(arg[1].type)
      {
      case T_POINTER:
      case T_MAPPING:
	res.u.number=arg[1].u.vec->ref;
	break;
	
      case T_STRING:
	res.u.number=print_string_refs(arg[1].u.string);
	break;
      case T_OBJECT:
	res.u.number=arg[1].u.ob->ref;
	break;
	
      default:
      }
    }
  break;
  }
  pop_n_elems(num_arg);
  sp++;
  *sp=res;
}
#endif /* F_DEBUG_INFO */

#ifdef F_SAY
void f_say(int num_arg)
{
  static struct {
    struct vector v;
    struct svalue second_item[1];
  } vtmp = { {
#ifndef NO_ARRAY_LINKS
     NULL,
     NULL,
#endif
     2, 0, 1,
#ifdef DEBUG
     1,
#endif
     0,
     { { T_NUMBER } } }, { { T_OBJECT } }
  };
  
  if (num_arg == 1)
  {
    vtmp.v.item[0].type = T_NUMBER; /* this marks the place for the
				       command_giver
				       */
    vtmp.v.item[1].type = T_NUMBER; /* will not match any object... */
    say(sp, &vtmp.v);
  }
  else
  {
    if ( sp->type == T_POINTER )
    {
      if (sp->u.vec->ref > 1)
      {
	struct vector *vtmpp =
	  slice_array(sp->u.vec, 0, sp->u.vec->size-1);
	say(sp-1, vtmpp);
	free_vector(vtmpp);
      } else
	say(sp-1, sp->u.vec);
    }
    else
    {
      vtmp.v.item[0].type = T_NUMBER;
      vtmp.v.item[1].type = T_OBJECT;
      vtmp.v.item[1].u.ob = sp->u.ob;
      add_ref(sp->u.ob, "say");
      say(sp-1, &vtmp.v);
    }
    pop_stack();
  }
}
#endif

#ifdef F_TELL_ROOM
void f_tell_room(int num_arg)
{
  extern struct vector null_vector;
  struct object *ob;
  struct svalue *arg = sp- num_arg + 1;
  struct vector *avoid;

  if (arg[0].type == T_OBJECT)
    ob = arg[0].u.ob;
  else
  {
    ob = find_object(arg[0].u.string);
    if (ob == 0)
      error("Object not found.\n");
  }
  if (num_arg == 2)
  {
    avoid = &null_vector;
    avoid->ref++;
  }
  else
  {
    extern struct vector *order_alist PROT((struct vector *));
    struct vector *vtmpp;
    static struct vector vtmp =
    {
#ifndef NO_ARRAY_LINKS
       NULL,
       NULL,
#endif
       1, 0, 1,
#ifdef DEBUG
       1,
#endif
       0,
       { { T_POINTER } }
    };

    if (arg[2].type != T_POINTER)
      bad_arg(3, F_TELL_ROOM);
    vtmp.item[0].u.vec = arg[2].u.vec;
    if (vtmp.item[0].u.vec->ref > 1)
    {
      vtmp.item[0].u.vec->ref--;
      vtmp.item[0].u.vec = slice_array(
				       vtmp.item[0].u.vec, 0, vtmp.item[0].u.vec->size-1);
    }
    sp--;
    vtmpp = order_alist(&vtmp);
    avoid = vtmpp->item[0].u.vec;
    vtmpp->item[0].u.vec = vtmp.item[0].u.vec;
    free_vector(vtmpp);
  }
  tell_room(ob, sp, avoid);
  free_vector(avoid);
  pop_stack();
}
#endif

#ifdef F__FIND_SHORTEST_PATH
struct find_tmp
{
  int seenfrom;
  int next;
};

void f__find_shortest_path(int num_arg)
{
  struct svalue *argp;
  struct find_tmp *tmp;
  struct vector *v,*w;
  int e,d,from,to,next;
  int maxlen;

  argp=sp-num_arg+1;
  if(argp[2].type != T_NUMBER)
    bad_arg(3,F__FIND_SHORTEST_PATH);

  if(num_arg > 3)
  {
    if(argp[3].type != T_NUMBER)
      bad_arg(4,F__FIND_SHORTEST_PATH);
    maxlen=argp[3].u.number;
  }else{
    maxlen=0x7fffffff;
  }

  from=argp[1].u.number;
  to=argp[2].u.number;

  if(to==from)
  {
    pop_n_elems(num_arg);
    v=allocate_array(0);
    push_vector(v);
    v->ref--;
    return;
  }

  v=argp[0].u.vec;

  if(from<0 || from>=v->size)
    error("'from' argument out of range.\n");

  if(to<0 || to>=v->size)
    error("'to' argument out of range.\n");

  tmp=(struct find_tmp *)xalloc(sizeof(struct find_tmp)*v->size);

  for(e=0;e<v->size;e++)
  {
    tmp[e].seenfrom=-1;
    tmp[e].next=-1;
  }

  tmp[from].seenfrom=-2;

  for(;from!=-1;from=next)
  {
    eval_cost++;
    for(next=-1;from!=-1;from=tmp[from].next)
    {
      if(v->item[from].type!=T_POINTER)
      {
        free((char *)tmp);
        error("graph should be array of array.\n");
      }

      w=v->item[from].u.vec;
      for(d=0;d<w->size;d++)
      {
        if(w->item[d].type!=T_NUMBER)
        {
          free((char *)tmp);
          error("graph should be array of array of int.\n");
        }

        e=w->item[d].u.number;
        if(e<0 || e>=v->size)
        {
          free((char *)tmp);
          error("graph contains indexes out of range.\n");
        }

        if(tmp[e].seenfrom!=-1) continue;

        tmp[e].seenfrom=from;
        tmp[e].next=next;
        next=e;

        if(e==to)
        {
          for(e=0,d=to;d!=-2;d=tmp[d].seenfrom) e++;
          w=allocate_array(e);
          for(d=to;d!=-2;d=tmp[d].seenfrom)
          {
            e--;
            w->item[e].type=T_NUMBER;
            w->item[e].u.number=d;
          }
          free((char *)tmp);
          pop_n_elems(num_arg);
          push_vector(w);
          w->ref--;
	  return;
        }
      }
    }
    if(--maxlen <= 0) break;
  }
  free((char *)tmp);  /* no path found */
  pop_n_elems(num_arg);
  push_number(0);
}
#endif

/*
 * May current_object shadow object 'ob' ? We rely heavily on the fact that
 * function names are pointers to shared strings, which means that equality
 * can be tested simply through pointer comparison.
 */
int validate_shadowing(ob)
    struct object *ob;
{
    int i, j;
    struct program *shadow = current_object->prog, *victim = ob->prog;
    struct svalue *ret;

/* Nice touch to shadow yourself but I don't think it's a good idea so... */
    if(current_object == ob)
       error("Shadow yourself? You must be kidding...\n");

    if (current_object->shadowing)
	error("shadow: Already shadowing.\n");
    if (current_object->shadowed)
	error("shadow: Can't shadow when shadowed.\n");
    if (current_object->super)
	error("The shadow must not reside inside another object.\n");
    if (ob->shadowing)
	error("Can't shadow a shadow.\n");
    for (i=0; i < shadow->num_functions; i++) {
	for (j=0; j < victim->num_functions; j++) {
	    if (shadow->functions[i].name != victim->functions[j].name)
		continue;
	    if (victim->functions[j].type & TYPE_MOD_NO_MASK)
		error("Illegal to shadow 'nomask' function \"%s\".\n",
		      victim->functions[j].name);
	}
    }
    push_object(ob);
    ret = apply_master_ob("query_allow_shadow", 1);
    if (!(ob->flags & O_DESTRUCTED) &&
	ret && !(ret->type == T_NUMBER && ret->u.number == 0))
    {
	return 1;
    }
    return 0;
}

void f_shadow()
{
  struct object *ob;

  ob = (sp-1)->u.ob;
  if (sp->u.number == 0)
  {
    ob = ob->shadowed;
    pop_n_elems(2);
    push_object(ob);
    return;
  }
  if (validate_shadowing(ob))
  {
    /*
     * The shadow is entered first in the chain.
     */
    while (ob->shadowed)
      ob = ob->shadowed;
    current_object->shadowing = ob;
    ob->shadowed = current_object;
    pop_n_elems(2);
    push_object(ob);
    return;
  }
  pop_n_elems(2);
  push_number(0);
}

#ifdef F__NEXT_CLONE
void f__next_clone()
{
  extern struct object *obj_list;
  struct object *o;
  int len;
  char *name,*tmp;
	
  name=(o=sp->u.ob)->name;
	
  tmp=strchr(name,'#');
  if(tmp)
    len=tmp-name;
  else
    len=strlen(name);
	
  pop_stack();
  do{
      o=o->next_all;
      if(!o) o=obj_list;
      eval_cost++;
    }while(strncmp(o->name,name,len) ||
	   (o->name[len]!=0 && o->name[len]!='#'));
  push_object(o);
}
#endif

void f_query_snoop()
{
  struct object *ob;
#ifdef COMPAT_MODE
  struct svalue *arg1;
  int level;
#endif
      
  if (current_interactive == 0 || !current_interactive->interactive ||
      (current_interactive->flags & O_DESTRUCTED) ||
      sp->u.ob->interactive == 0)
  {
    pop_stack();
    push_number(0);
    return;
  }
#ifdef COMPAT_MODE
  arg1 = sapply("query_level", current_interactive, 0);
  if (arg1 == 0 || arg1->type != T_NUMBER || arg1->u.number < 22)
  {
    pop_stack();
    push_number(0);
    return;
  }
  level=arg1->u.number;
  arg1=sapply("query_level",sp->u.ob,0);
  if (arg1 == 0 || arg1->type!=T_NUMBER || arg1->u.number >= level)
  {
    pop_stack();
    push_number(0);
    return;
  }
  ob = query_snoop(sp->u.ob);
#else
  assert_master_ob_loaded();
  if (current_object == master_ob)
    ob = query_snoop(sp->u.ob);
  else
    ob = 0;
#endif
  pop_stack();
  push_object(ob);
}

void f_trace()
{
  int ot = -1;
  if (command_giver && command_giver->interactive)
  {
    struct svalue *arg;
    push_constant_string("trace");
    arg = apply_master_ob("query_player_level", 1);
    if (arg && (arg->type != T_NUMBER || arg->u.number != 0))
    {
      ot = command_giver->interactive->trace_level;
      command_giver->interactive->trace_level = sp->u.number;
    }
  }
  pop_stack();
  push_number(ot);
}

void f_traceprefix()
{
  char *old = 0;
      
  if (command_giver && command_giver->interactive)
  {
    struct svalue *arg;
    push_constant_string("trace");
    arg = apply_master_ob("query_player_level",1);
    if (arg && (arg->type != T_NUMBER || arg->u.number))
    {
      old = command_giver->interactive->trace_prefix;
      if (sp->type == T_STRING)
      {
	command_giver->interactive->trace_prefix = 
	  make_shared_string(sp->u.string);
      } else
	command_giver->interactive->trace_prefix = 0;
    }
  }
  pop_stack();
  if (old)
  {
    push_string(old, STRING_SHARED); /* Will incr ref count */
    free_string(old);
  }
  else
  {
    push_number(0);
  }
}

void f_extract(int num_arg)
{
  int len, from, to;
  struct svalue *arg;
  char *res;

  arg = sp - num_arg + 1;
  len = strlen(arg[0].u.string);
  if (num_arg == 1)
    return;			/* Simply return argument */
  from = arg[1].u.number;
  if (from < 0)
    from = len + from;
  if (from >= len)
  {
    pop_n_elems(num_arg);
    push_string("", STRING_CONSTANT);
    return;
  }
  if (num_arg == 2)
  {
    res = string_copy(arg->u.string + from);
    pop_n_elems(2);
    push_malloced_string(res);
    return;
  }
  if (arg[2].type != T_NUMBER)
    error("Bad third argument to extract()\n");
  to = arg[2].u.number;
  if (to < 0)
    to = len + to;
  if (to < from)
  {
    pop_n_elems(3);
    push_string("", STRING_CONSTANT);
    return;
  }
  if (to >= len)
    to = len-1;
  if (to == len-1)
  {
    res = string_copy(arg->u.string + from);
    pop_n_elems(3);
    push_malloced_string(res);
    return;
  }
  res = xalloc(to - from + 2);
  strncpy(res, arg[0].u.string + from, to - from + 1);
  res[to - from + 1] = '\0';
  pop_n_elems(3);
  push_malloced_string(res);
}


void f_snoop(int num_arg)
#ifdef COMPAT_MODE
{
  struct svalue *arg1, *arg2;
  int cmd_level;

	  
  if (!command_giver || !command_giver->interactive || num_arg == 2)
  {
    pop_n_elems (num_arg);
    push_number (0);
  }
  else
  {
    if (num_arg == 0)
    {
      set_snoop(command_giver, 0);
      push_number(0);
    }
    else
    {
#if 1				/* pen 920111 */
      /* Only wizards may call snoop */
      arg1 = sapply("query_level_sec", command_giver, 0);
      if (arg1 == 0 || arg1->type != T_NUMBER || arg1->u.number < 20)
      {
	pop_n_elems(num_arg);
	push_number(0);
	return;
      }
      cmd_level = arg1->u.number;
	      
      /* May only snoop players of level < own_level */
      arg2 = sapply("query_level_sec", sp->u.ob, 0);
      if (!arg2 || arg2->type != T_NUMBER ||
	  arg2->u.number >= cmd_level || arg2->u.number < 1)
	/* Wing 920501, you should not be able to snoop logon
	   (level -1)... */
      {
	pop_n_elems(num_arg);
	push_number(0);
	return;
      }
#endif
      /* The argument object is returned. */
      set_snoop(command_giver, sp->u.ob);
    }
  }
}
#else
{
  if (!command_giver) {
    pop_n_elems(num_arg);
    push_number(0);
  } else {
    ob = 0; /* Do not remove this, it is not 0 by default */
    switch (num_arg)
    {
    case 1:
      if (new_set_snoop(sp->u.ob, 0))
	ob = sp->u.ob;
      break;

    case 2:
      if (new_set_snoop((sp-1)->u.ob, sp->u.ob))
	ob = sp->u.ob;
      break;

    default:
      ob = 0;
      break;
    }
    pop_n_elems(num_arg);
    push_object(ob);
  }
}
#endif

void f_ed(int num_arg)
{
  if (num_arg == 0)
  {
    struct svalue *arg;
    char *err_file;

    if (command_giver == 0 || (command_giver->flags & O_DESTRUCTED) ||
	command_giver->interactive == 0)
    {
      push_number(0);
      return;
    }
    arg = sapply("query_real_name", command_giver, 0);
    if (arg == 0 || arg->type != T_STRING)
    {
      push_number(0);
      return;
    }
    err_file = get_error_file(arg->u.string);
    if (err_file == 0)
    {
      push_number(0);
      return;
    }
    ed_start(err_file, 0, 0);
    push_number(1);
    return;
  }
  else if (num_arg == 1)
  {
    ed_start(sp->u.string, 0, 0);
  }
  else
  {
    if (sp->type == T_STRING)
      ed_start((sp-1)->u.string, sp->u.string, current_object);
    else
      ed_start((sp-1)->u.string, 0 , 0);
    pop_stack();
  }
}

#ifdef F__SUPER_INVIS
void f__super_invis(int num_arg)
{
  struct svalue *arg1;
  if(!command_giver || !command_giver->interactive ||
     (command_giver->flags & O_DESTRUCTED))
  {
    if (num_arg) 
      pop_stack();
    push_number(0);
    return;
  }
  else 
  {
    arg1 = sapply("query_level_sec", command_giver, 0);
    if (!arg1 || arg1->type != T_NUMBER || arg1->u.number < 22)
    {
      if (num_arg) 
	pop_stack();
      push_number(0);
      return;
    }
  }
  if (num_arg)
  {
    if (sp->u.number)
    {
      command_giver->flags |= O_SUPER_INVIS;
      sendmudwhologout(command_giver);
    }
    else
      command_giver->flags &= ~O_SUPER_INVIS;
    pop_stack();
  }
  else
    if (command_giver->flags & O_SUPER_INVIS)
      command_giver->flags &= ~O_SUPER_INVIS;
    else
    {
      command_giver->flags |= O_SUPER_INVIS;
      sendmudwhologout(command_giver);
    }
  push_number(1);
}

#endif

void f__driver_stat()
{
  extern int cur_mem_use(void);
  extern int start_time;
  struct vector *v;
  v=allocate_array(2);
  v->item[0].u.number=start_time;
  v->item[1].u.number=cur_mem_use();
  simple_push_vector(v);
}

#ifdef F__GET_CLONE_BY_NUMBER
struct object *f__get_clone_by_number(int num)
{
   extern struct object *obj_list;
   struct object *obj;
   char *ptr;
   int t;
   for(obj = obj_list; obj; obj = obj->next_all)
   {
      if((ptr = strrchr(obj->name, '#')) == NULL)
	 continue;
      for(t = 0; isdigit(*++ptr); )
	 t = t * 10 + *ptr - '0';
      if(*ptr == '\0' && t == num)
	 break;
   }
   return obj;
}
#endif

#ifdef F__CHMOD
int f__chmod(char *fname, int mode)
{
   char *path;
   if(current_object != master_ob)
   {
      error("Illegal call of efun _chmod()!\n");
      return 0;  /* Paranoia */
   }
   if(cfg_use_acls)
      path = acl_checkrights(fname, ACL_P, 0, 0);
   else
   {
#ifdef COMPAT_MODE
      path = check_file_name(fname, 1);
#else
      path = check_valid_path(fname, current_object->eff_user, "_chmod", 1);
#endif
   }
   if(!path)
      return 0;
   if(chmod(path, mode & 0666) == -1)
      return 0;
   return 1;
}
#endif
