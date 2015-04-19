%{
# line 3 "prelang.y"
/* The above line is to give proper line number references. Please mail me
 * if your compiler complains about it.
 */
/*
 * This is the grammar definition of LPC. The token table is built
 * automatically by make_func. The lang.y is constructed from this file,
 * the generated token list and post_lang.y. The reason of this is that there
 * is no #include-statment that yacc recognizes.
 */
#include <string.h>
#include <stdio.h>
#include "memory.h"
#if defined(sun)
#include <alloca.h>
#endif

#include "lint.h"
#include "interpret.h"
#include "object.h"
#include "exec.h"
#include "config.h"
#include "instrs.h"
#include "incralloc.h"
#include "switch.h"

#if defined(__GNUC__) && !defined(lint) && !defined(DEBUG)
#define INLINE  /*  inline */
#else
#define INLINE
#endif

#define DEBUG_SWITCH

#define YYMAXDEPTH	600

/* NUMPAREAS areas are saved with the program code after compilation.
 */
#define A_PROGRAM		0
#define A_FUNCTIONS		1
#define A_STRINGS		2
#define A_VARIABLES		3
#define A_LINENUMBERS		4
#define A_INHERITS		5
#define NUMSAVEAREAS            6   /* */
#define A_ARGUMENT_TYPES	6
#define A_ARGUMENT_INDEX	7
#define NUMPAREAS		8
#define A_CASE_NUMBERS		8
#define A_CASE_STRINGS		9
#define A_CASE_LABELS	       10
#define NUMAREAS	       11

#define BREAK_UNKNOWN	        0x80000


/* make shure that this struct has a size that is a power of two */
struct case_heap_entry { int key; short addr; short line; };
#define CASE_HEAP_ENTRY_ALIGN(offset) offset &= -sizeof(struct case_heap_entry)

#ifndef NO_OVERLAP_CHECK
struct case_overlap_check
{
   struct case_overlap_check *next;
   int lkey;
   int hkey;
   short line;
};

static struct case_overlap_check *top_case;
#endif

static struct mem_block mem_block[NUMAREAS];

/*
 * Some good macros to have.
 */

#define BASIC_TYPE(e,t) ((e) == TYPE_ANY ||\
			 (e) == (t) ||\
			 (t) == TYPE_ANY)

#define TYPE(e,t) (BASIC_TYPE((e) & TYPE_MOD_MASK, (t) & TYPE_MOD_MASK) ||\
		   (((e) & TYPE_MOD_POINTER) && ((t) & TYPE_MOD_POINTER) &&\
		    BASIC_TYPE((e) & (TYPE_MOD_MASK & ~TYPE_MOD_POINTER),\
			       (t) & (TYPE_MOD_MASK & ~TYPE_MOD_POINTER))))

#define FUNCTION(n) ((struct function *)mem_block[A_FUNCTIONS].block + (n))
#define VARIABLE(n) ((struct variable *)mem_block[A_VARIABLES].block + (n))
#define PC mem_block[A_PROGRAM].current_size

#define align(x) (((x) + 3) & ~3)

/*
 * If the type of the function is given, then strict types are
 * checked and required.
 */
static int exact_types;
extern int pragma_strict_types;	/* Maintained by lex.c */
extern int pragma_save_types;	/* Also maintained by lex.c */
int approved_object;		/* How I hate all these global variables */

extern int total_num_prog_blocks, total_prog_block_size;

extern int num_parse_error;
extern int d_flag;
static int heart_beat;		/* Number of the heart beat function */

static int current_break_address;
static int current_switch;
static int current_continue_address;
static int current_case_number_heap;
static int current_case_string_heap;
#define SOME_NUMERIC_CASE_LABELS 0x40000
#define NO_STRING_CASE_LABELS    0x80000
static int zero_case_label;
static int current_type;

static int last_push_local;
static int last_push_identifier;
static int last_push_index;
static int last_push_range;

/*
 * There is always function starting at address 0, which will execute
 * the initialization code. This code is spread all over the program,
 * with jumps to next initializer. The next variable keeps track of
 * the previous jump. After the last initializer, the jump will be changed
 * into a return(0) statement instead.
 *
 * A function named '__INIT' will be defined, which will contain the
 * initialization code. If there was no initialization code, then the
 * function will not be defined. That is the usage of the
 * first_last_initializer_end variable.
 *
 * When inheriting from another object, a call will automatically be made
 * to call __INIT in that code from the current __INIT.
 */
static int last_initializer_end;
static int first_last_initializer_end;
static int code_generation=1;
int bytes_won=0;
int bytes_optimized=0;

#ifndef NO_PROGRAM_STATUS
int arg_bytes_won = 0;

int progstruct_bytes = 0;
int byte_code_bytes = 0;
int function_bytes = 0;
int strings_bytes = 0;
int variables_bytes = 0;
int line_number_bytes = 0;
int inherits_bytes = 0;
#ifdef DO_SAVE_ARG_TYPES
int arg_types_bytes = 0;
int arg_index_bytes = 0;
#endif
#endif

static struct program NULL_program; /* marion - clean neat empty struct */

void epilog();
static int check_declared PROT((char *str));
static void prolog();
static char *get_two_types PROT((int type1, int type2));
void free_all_local_names(),
    add_local_name PROT((char *, int)), smart_log PROT((char *, int, char *));
extern int yylex();
static int verify_declared PROT((char *));
static void copy_variables();
static int copy_functions PROT((struct program *, int type));
void type_error PROT((char *, int));

char *xalloc(), *string_copy();

extern int current_line;
/*
 * 'inherit_file' is used as a flag. If it is set to a string
 * after yyparse(), this string should be loaded as an object,
 * and the original object must be loaded again.
 */
extern char *current_file, *inherit_file;

/*
 * The names and types of arguments and auto variables.
 */
char *local_names[MAX_LOCAL];
unsigned short type_of_locals[MAX_LOCAL];
int current_number_of_locals = 0;

/*
 * The types of arguments when calling functions must be saved,
 * to be used afterwards for checking. And because function calls
 * can be done as an argument to a function calls,
 * a stack of argument types is needed. This stack does not need to
 * be freed between compilations, but will be reused.
 */
static struct mem_block type_of_arguments;

struct program *prog;	/* Is returned to the caller of yyparse */

/*
 * Compare two types, and return true if they are compatible.
 */
static int compatible_types(t1, t2)
    int t1, t2;
{
    if (t1 == TYPE_UNKNOWN || t2 == TYPE_UNKNOWN)
	return 0;
    if ((t1 & TYPE_MOD_MASK) == (TYPE_MOD_MASK & t2))
	return 1;
    if (t1 == TYPE_ANY || t2 == TYPE_ANY)
	return 1;
    if ((t1 & TYPE_MOD_POINTER) && (t2 & TYPE_MOD_POINTER)) {
	if ((t1 & TYPE_MOD_MASK) == (TYPE_ANY|TYPE_MOD_POINTER) ||
	    (t2 & TYPE_MOD_MASK) == (TYPE_ANY|TYPE_MOD_POINTER))
	    return 1;
    }
    return 0;
}

/*
 * Add another argument type to the argument type stack
 */
INLINE
static void add_arg_type(type)
    unsigned short type;
{
    struct mem_block *mbp = &type_of_arguments;
    while (mbp->current_size + sizeof type > mbp->max_size) {
	mbp->max_size <<= 1;
	mbp->block = (char *) realloc((char *)mbp->block, mbp->max_size);
    }
    memcpy(mbp->block + mbp->current_size, &type, sizeof type);
    mbp->current_size += sizeof type;
}

/*
 * Pop the argument type stack 'n' elements.
 */
INLINE
static void pop_arg_stack(n)
    int n;
{
    type_of_arguments.current_size -= sizeof (unsigned short) * n;
}

/*
 * Get type of argument number 'arg', where there are
 * 'n' arguments in total in this function call. Argument
 * 0 is the first argument.
 */
INLINE
int get_argument_type(arg, n)
    int arg, n;
{
    return
	((unsigned short *)
	 (type_of_arguments.block + type_of_arguments.current_size))[arg - n];
}

INLINE
static void add_to_mem_block(n, data, size)
    int n, size;
    char *data;
{
    struct mem_block *mbp = &mem_block[n];
    while (mbp->current_size + size > mbp->max_size) {
	mbp->max_size <<= 1;
	mbp->block = (char *) realloc((char *)mbp->block, mbp->max_size);
    }
    memcpy(mbp->block + mbp->current_size, data, size);
    mbp->current_size += size;
}

static void ins_byte(b)
    char b;
{
  if(code_generation)
  {
    while (mem_block[A_PROGRAM].current_size >=
	   mem_block[A_PROGRAM].max_size)
    {
      mem_block[A_PROGRAM].max_size <<= 1;
      mem_block[A_PROGRAM].block = (char *) realloc((char *)mem_block[A_PROGRAM].block, mem_block[A_PROGRAM].max_size);
    }
    mem_block[A_PROGRAM].block[mem_block[A_PROGRAM].current_size++]=b;
  }
  else
    bytes_won++;
}

/*
 * Store a 2 byte number. It is stored in such a way as to be sure
 * that correct byte order is used, regardless of machine architecture.
 * Also beware that some machines can't write a word to odd addresses.
 */
static void ins_short(l)
    short l;
{
  if(!code_generation)
  {
    bytes_won+=2;
    return;
  }
#if 0
    add_to_mem_block(A_PROGRAM, (char *)&l + 0, 1);
    add_to_mem_block(A_PROGRAM, (char *)&l + 1, 1);
#else
    add_to_mem_block(A_PROGRAM, (char *)&l, sizeof(short));
#endif
  if(code_generation==2)
    code_generation=0;
}

static void upd_short(offset, l)
    int offset;
    short l;
{
  mem_block[A_PROGRAM].block[offset + 0] = ((char *)&l)[0];
  mem_block[A_PROGRAM].block[offset + 1] = ((char *)&l)[1];
}

static void upd_jump(int offset, int l)
{
  upd_short(offset, l - offset);
}

static void ins_jump(int where)
{
  ins_short(where - PC);
}

static short read_short(offset)
    int offset;
{
  short l;

  ((char *)&l)[0] = mem_block[A_PROGRAM].block[offset + 0];
  ((char *)&l)[1] = mem_block[A_PROGRAM].block[offset + 1];
  return l;
}

/*
 * Store a 4 byte number. It is stored in such a way as to be sure
 * that correct byte order is used, regardless of machine architecture.
 */
static void ins_long(l)
    int l;
{
  if(!code_generation) 
  {
    bytes_won+=4;
    return;
  }
#if 0
    add_to_mem_block(A_PROGRAM, (char *)&l+0, 1);
    add_to_mem_block(A_PROGRAM, (char *)&l+1, 1);
    add_to_mem_block(A_PROGRAM, (char *)&l+2, 1);
    add_to_mem_block(A_PROGRAM, (char *)&l+3, 1);
#else
    add_to_mem_block(A_PROGRAM, (char *)&l,sizeof(int));
#endif
}

#define INSTR_T unsigned int
#define BACKLOG_SIZE 256

struct instruction
{
  int offset;
  INSTR_T instr;
};

static struct instruction backlog[BACKLOG_SIZE];
static int backlog_end=-1;
static int backlog_size=0;

static void backup(int steps)
{
  backlog_end-=steps-1;
  if(backlog_end < 0) backlog_end+=BACKLOG_SIZE;
  PC=backlog[backlog_end].offset;
  backlog_end--;
  if(backlog_end < 0) backlog_end+=BACKLOG_SIZE;
  backlog_size-=steps;
}

static void ins_f_byte(INSTR_T);

static void replace_instr(int howmany, int instruction)
{
  backup(howmany);
  if(instruction)
  {
    bytes_won+=howmany-1;
    ins_f_byte(instruction);
  }else{
    bytes_won+=howmany;
  }
}

static int last_instr(int i)
{
  return backlog[((i+backlog_end)+BACKLOG_SIZE*100)%BACKLOG_SIZE].instr;
}

static void change_instr(int i,int new_instr)
{
  i=((i+backlog_end)+BACKLOG_SIZE*100)%BACKLOG_SIZE;
  backlog[i].instr=new_instr;
  mem_block[A_PROGRAM].block[backlog[i].offset]=(new_instr - F_OFFSET);
}

#define TWO_INSTR(X,Y) ((X)+(Y)*1024)
#define THREE_INSTR(X,Y,Z) ((X)+(Y)*1024+(Z)*1024*1024)

static void ins_f_byte(INSTR_T b)
{
  int tmp;

  if(!code_generation)
  {
    bytes_won++;
    return;
  }

  backlog_end++;
  if(backlog_end >= BACKLOG_SIZE) backlog_end=0;

  backlog[backlog_end].instr=b;
  backlog[backlog_end].offset=PC;

  if(backlog_size < BACKLOG_SIZE) backlog_size++;

  while(backlog_size)
  {
    if(backlog_size>1)
    {
      switch(TWO_INSTR(last_instr(-1),last_instr(0)))
      {
      case TWO_INSTR(F_NOT, F_JUMP_WHEN_ZERO):
	replace_instr(2,F_JUMP_WHEN_NON_ZERO);
	return;

      case TWO_INSTR(F_NOT, F_JUMP_WHEN_NON_ZERO):
	replace_instr(2,F_JUMP_WHEN_ZERO);
	return;

      case TWO_INSTR(F_EQ, F_NOT):
	replace_instr(2,F_NE);
	return;

      case TWO_INSTR(F_NE, F_NOT):
	replace_instr(2,F_EQ);
	return;

      case TWO_INSTR(F_LT, F_NOT):
	replace_instr(2,F_GE);
	return;

      case TWO_INSTR(F_LE, F_NOT):
	replace_instr(2,F_GT);
	return;

      case TWO_INSTR(F_GT, F_NOT):
	replace_instr(2,F_LE);
	return;

      case TWO_INSTR(F_GE, F_NOT):
	replace_instr(2,F_LT);
	return;

      case TWO_INSTR(F_POP_VALUE, F_POP_VALUE):
	replace_instr(2,F_POP_VALUE2);
	return;

      case TWO_INSTR(F_BREAK_POINT, F_BREAK_POINT):
	replace_instr(1,0);
	return;

      case TWO_INSTR(F_CONST0, F_POP_VALUE):
      case TWO_INSTR(F_CONST1, F_POP_VALUE):
      case TWO_INSTR(F_CONST_1, F_POP_VALUE):
      case TWO_INSTR(F_BYTE, F_POP_VALUE):
      case TWO_INSTR(F_NEG_BYTE, F_POP_VALUE):
      case TWO_INSTR(F_SHORT, F_POP_VALUE):
      case TWO_INSTR(F_NUMBER, F_POP_VALUE):
      case TWO_INSTR(F_STRING, F_POP_VALUE):
      case TWO_INSTR(F_SHORT_STRING, F_POP_VALUE):
	replace_instr(2,0);
	return;

      case TWO_INSTR(F_CONST0, F_RETURN):
	replace_instr(2,F_RETURN_ZERO);
	return;

      case TWO_INSTR(F_CONST1, F_RETURN):
	replace_instr(2,F_RETURN_ONE);
	return;

      case TWO_INSTR(F_ASSIGN, F_POP_VALUE):
	replace_instr(2,F_ASSIGN_AND_POP);
	return;

      case TWO_INSTR(F_CONST_1, F_SUB_EQ):
      case TWO_INSTR(F_CONST1, F_ADD_EQ):
	replace_instr(2,F_INC);
	return;

      case TWO_INSTR(F_CONST_1, F_ADD_EQ):
      case TWO_INSTR(F_CONST1, F_SUB_EQ):
	replace_instr(2,F_DEC);
	return;

      case TWO_INSTR(F_DEC, F_POP_VALUE):
      case TWO_INSTR(F_POST_DEC, F_POP_VALUE):
	replace_instr(2,F_DEC_AND_POP);
	return;

      case TWO_INSTR(F_INC, F_POP_VALUE):
      case TWO_INSTR(F_POST_INC, F_POP_VALUE):
	replace_instr(2,F_INC_AND_POP);
	return;

      case TWO_INSTR(F_EQ, F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_NE, F_JUMP_WHEN_ZERO):
	replace_instr(2,F_JUMP_WHEN_EQ);
	return;

      case TWO_INSTR(F_NE, F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_EQ, F_JUMP_WHEN_ZERO):
	replace_instr(2,F_JUMP_WHEN_NE);
	return;

      case TWO_INSTR(F_LT, F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_GE, F_JUMP_WHEN_ZERO):
	replace_instr(2,F_JUMP_WHEN_LT);
	return;

      case TWO_INSTR(F_GE, F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_LT, F_JUMP_WHEN_ZERO):
	replace_instr(2,F_JUMP_WHEN_GE);
	return;

      case TWO_INSTR(F_GT, F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_LE, F_JUMP_WHEN_ZERO):
	replace_instr(2,F_JUMP_WHEN_GT);
	return;

      case TWO_INSTR(F_LE, F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_GT, F_JUMP_WHEN_ZERO):
	replace_instr(2,F_JUMP_WHEN_LE);
	return;

      case TWO_INSTR(F_CONST0,F_NEGATE):
	replace_instr(1,0);
	return;

      case TWO_INSTR(F_CONST1,F_NEGATE):
	replace_instr(2,F_CONST_1);
	return;

      case TWO_INSTR(F_CONST_1,F_NEGATE):
	replace_instr(2,F_CONST1);
	return;

      case TWO_INSTR(F_BYTE,F_NEGATE):
	bytes_won++;
	backup(1);
	change_instr(0,F_NEG_BYTE);
	return;

      case TWO_INSTR(F_NEG_BYTE,F_NEGATE):
	bytes_won++;
	backup(1);
	change_instr(0,F_BYTE);
	return;

      case TWO_INSTR(F_CONST0,F_EQ):
	replace_instr(2,F_NOT);
	return;

      case TWO_INSTR(F_M_INDICES,F_SIZEOF):
      case TWO_INSTR(F_M_VALUES,F_SIZEOF):
	replace_instr(2,F_M_SIZEOF);
	return;

      case TWO_INSTR(F_STRING,F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_SHORT_STRING,F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_SHORT,F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_NEG_BYTE,F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_BYTE,F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_CONST_1,F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_CONST1,F_JUMP_WHEN_NON_ZERO):
      case TWO_INSTR(F_CONST0,F_JUMP_WHEN_ZERO):
	replace_instr(2,F_JUMP);
	return;

      case TWO_INSTR(F_CONST0,F_JUMP_WHEN_NE):
	replace_instr(2,F_JUMP_WHEN_NON_ZERO);
	return;
	
      case TWO_INSTR(F_CONST0,F_JUMP_WHEN_EQ):
	replace_instr(2,F_JUMP_WHEN_ZERO);
	return;
      }


      switch(THREE_INSTR(last_instr(-2),last_instr(-1),last_instr(0)))
      {
      case THREE_INSTR(F_CONST0,F_LOCAL_NAME,F_JUMP_WHEN_EQ):
	tmp=mem_block[A_PROGRAM].block[PC-2];
	bytes_won++;
	backup(3);
	ins_f_byte(F_LOCAL_NAME);
	ins_byte(tmp);
	ins_f_byte(F_JUMP_WHEN_ZERO);
	return;

      case THREE_INSTR(F_CONST0,F_IDENTIFIER,F_JUMP_WHEN_EQ):
	tmp=mem_block[A_PROGRAM].block[PC-2];
	bytes_won++;
	backup(3);
	ins_f_byte(F_IDENTIFIER);
	ins_byte(tmp);
	ins_f_byte(F_JUMP_WHEN_ZERO);
	return;

      case THREE_INSTR(F_CONST0,F_LOCAL_NAME,F_JUMP_WHEN_NE):
	tmp=mem_block[A_PROGRAM].block[PC-2];
	bytes_won++;
	backup(3);
	ins_f_byte(F_LOCAL_NAME);
	ins_byte(tmp);
	ins_f_byte(F_JUMP_WHEN_NON_ZERO);
	return;

      case THREE_INSTR(F_CONST0,F_IDENTIFIER,F_JUMP_WHEN_NE):
	tmp=mem_block[A_PROGRAM].block[PC-2];
	bytes_won++;
	backup(3);
	ins_f_byte(F_IDENTIFIER);
	ins_byte(tmp);
	ins_f_byte(F_JUMP_WHEN_NON_ZERO);
	return;
      }
    }

    break;
  }

  ins_byte(b-F_OFFSET);

  switch(b)
  {
    case F_RETURN:
    case F_RETURN_ZERO:
    case F_RETURN_ONE:
    case F_BREAK:
      backlog_size=0;
      code_generation=0;
      break;

    case F_JUMP:
      backlog_size=0;
      code_generation=2;
  }
}

static int do_label()
{
  backlog_size=0;
  code_generation=1;
  return PC;
}

static void pop_jump_list(int stack, int to)
{
  int i;
  stack &= 0xffff;
  while(stack)
  {
    i=read_short(stack) & 0xffff;
    upd_jump(stack,to);
    stack=i;
  }
}

/*
 * Return the index of the function found, otherwise -1.
 */
static int defined_function(s)
    char *s;
{
    int offset;
    struct function *funp;
    char *interned;
    extern char * findstring PROT((char*));

    if (interned = findstring(s)) /* Only search if amongst strings */
	for (offset = 0; offset < mem_block[A_FUNCTIONS].current_size;
	     offset += sizeof (struct function)) {
	    funp = (struct function *)&mem_block[A_FUNCTIONS].block[offset];
	    if (funp->flags & NAME_HIDDEN)
		continue;
	    /* Pointer comparison is possible since we use unique strings */
	    if (funp->name == interned) 
		return offset / sizeof (struct function);
	}
    return -1;
}

/*
 * A mechanism to remember addresses on a stack. The size of the stack is
 * defined in config.h.
 */
static int comp_stackp;
static int comp_stack[COMPILER_STACK_SIZE];

static void push_address() {
    if (comp_stackp >= COMPILER_STACK_SIZE) {
	yyerror("Compiler stack overflow");
	comp_stackp++;
	return;
    }
    comp_stack[comp_stackp++] = do_label();
}

static void push_explicit(address)
    int address;
{
    if (comp_stackp >= COMPILER_STACK_SIZE) {
	yyerror("Compiler stack overflow");
	comp_stackp++;
	return;
    }
    comp_stack[comp_stackp++] = address;
}

static int pop_address() {
    if (comp_stackp == 0)
	fatal("Compiler stack underflow.\n");
    if (comp_stackp > COMPILER_STACK_SIZE) {
	--comp_stackp;
	return 0;
    }
    return comp_stack[--comp_stackp];
}

/*
 * Patch a function definition of an inherited function, to what it really
 * should be.
 * The name of the function can be one of:
 *    object::name
 *    ::name
 *    name
 * Where 'object' is the name of the superclass.
 */
static void find_inherited(funp)
    struct function *funp;
{
    int i;
    struct inherit *ip;
    int num_inherits, super_length;
    char *real_name, *super_name = 0, *p;
    char *interned;
    extern char * findstring PROT((char*));

    real_name = funp->name;
    if (real_name[0] == ':')
	real_name = real_name + 2;	/* There will be exactly two ':' */
    else if (p = strchr(real_name, ':')) {
	real_name = p+2;
	super_name = funp->name;
	super_length = real_name - super_name - 2;
    }
    num_inherits = mem_block[A_INHERITS].current_size /
	sizeof (struct inherit);
    ip = (struct inherit *)mem_block[A_INHERITS].block;
    if (interned = findstring(real_name)) /* Only search if amongst strings */
	for (; num_inherits > 0; ip++, num_inherits--) {
	    if (super_name) {
		int l = strlen(ip->prog->name);	/* Including .c */
		if (l - 2 < super_length)
		    continue;
		if (strncmp(super_name, ip->prog->name + l - 2 - super_length,
			    super_length) != 0)
		    continue;
	    }
	    for (i=0; i < ip->prog->num_functions; i++) {
		if (ip->prog->functions[i].flags & (NAME_UNDEFINED|NAME_HIDDEN))
		    continue;
		/* Pointer comparison possible since we use unique strings */
		if (ip->prog->functions[i].name != interned)
		    continue;
		funp->inherit_offset = ip - (struct inherit *)mem_block[A_INHERITS].block;
		funp->offset = i;
		funp->flags = ip->prog->functions[i].flags | NAME_INHERITED;
		funp->num_local = ip->prog->functions[i].num_local;
		funp->num_arg = ip->prog->functions[i].num_arg;
		funp->type = ip->prog->functions[i].type;
		return;
	    }
	}
    return;
}

/*
 * Define a new function. Note that this function is called at least twice
 * for alll function definitions. First as a prototype, then as the real
 * function. Thus, there are tests to avoid generating error messages more
 * than once by looking at (flags & NAME_PROTOTYPE).
 */
static int define_new_function(name, num_arg, num_local, offset, flags, type)
    char *name;
    int num_arg, num_local;
    int offset, flags, type;
{
    int num;
    struct function fun;
    unsigned short argument_start_index;

    num = defined_function(name);
    if (num >= 0) {
	struct function *funp;

	/*
	 * The function was already defined. It may be one of several reasons:
	 *
	 * 1.	There has been a prototype.
	 * 2.	There was the same function defined by inheritance.
	 * 3.	This function has been called, but not yet defined.
	 * 4.	The function is doubly defined.
	 * 5.	A "late" prototype has been encountered.
	 */
	funp = (struct function *)(mem_block[A_FUNCTIONS].block) + num;
	if (!(funp->flags & NAME_UNDEFINED) &&
	    !(flags & NAME_PROTOTYPE) &&
	    !(funp->flags & NAME_INHERITED))
	{
	    char buff[500];
	    sprintf(buff, "Redeclaration of function %s.", name);
	    yyerror(buff);
	    return num;
	}
	/*
	 * It was either an undefined but used funtion, or an inherited
	 * function. In both cases, we now consider this to be THE new
	 * definition. It might also have been a prototype to an already
	 * defined function.
	 *
	 * Check arguments only when types are supposed to be tested,
	 * and if this function really has been defined already.
	 *
	 * 'nomask' functions may not be redefined.
	 */
	if ((funp->type & TYPE_MOD_NO_MASK) &&
	    !(funp->flags & NAME_PROTOTYPE) &&
	    !(flags & NAME_PROTOTYPE))
	{
	    char *p = (char *)alloca(80 + strlen(name));
	    sprintf(p, "Illegal to redefine 'nomask' function \"%s\"",name);
	    yyerror(p);
	}
	if (exact_types && funp->type != TYPE_UNKNOWN) {
	    int i;
#if 0 /* Profezzorn
	    if (funp->num_arg != num_arg && !(funp->type & TYPE_MOD_VARARGS))
		yyerror("Incorrect number of arguments.");
	    else if (!(funp->flags & NAME_STRICT_TYPES))
		yyerror("Called function not compiled with type testing.");
	    else {
		/* Now check that argument types wasn't changed. */
		for (i=0; i < num_arg; i++) {
		}
	    }
#endif
	}
	/* If it was yet another prototype, then simply return. */
	if (flags & NAME_PROTOTYPE)
	    return num;
	funp->num_arg = num_arg;
	funp->num_local = num_local;
	funp->flags = flags;
	funp->offset = offset;
	funp->inherit_offset = 0;
	funp->type = type;
	if (exact_types)
	    funp->flags |= NAME_STRICT_TYPES;
	return num;
    }
    if (strcmp(name, "heart_beat") == 0)
	heart_beat = mem_block[A_FUNCTIONS].current_size /
	    sizeof (struct function);
    fun.name = make_shared_string(name);
    fun.offset = offset;
    fun.flags = flags;
    fun.num_arg = num_arg;
    fun.num_local = num_local;
    fun.inherit_offset = 0;
    fun.type = type;
    if (exact_types)
	fun.flags |= NAME_STRICT_TYPES;
    num = mem_block[A_FUNCTIONS].current_size / sizeof fun;
    /* Number of local variables will be updated later */
    add_to_mem_block(A_FUNCTIONS, (char *)&fun, sizeof fun);

    if (exact_types == 0 || num_arg == 0) {
	argument_start_index = INDEX_START_NONE;
    } else {
	int i;

	/*
	 * Save the start of argument types.
	 */
	argument_start_index =
	    mem_block[A_ARGUMENT_TYPES].current_size /
		sizeof (unsigned short);
	for (i=0; i < num_arg; i++) {
	    add_to_mem_block(A_ARGUMENT_TYPES, &type_of_locals[i],
			     sizeof type_of_locals[i]);
	}
    }
    add_to_mem_block(A_ARGUMENT_INDEX, &argument_start_index,
		     sizeof argument_start_index);
    return num;
}

static void define_variable(name, type, flags)
    char *name;
    int type;
    int flags;
{
    struct variable dummy;
    int n;

    n = check_declared(name);
    if (n != -1 && (VARIABLE(n)->type & TYPE_MOD_NO_MASK)) {
	char *p = (char *)alloca(80 + strlen(name));
	sprintf(p, "Illegal to redefine 'nomask' variable \"%s\"", name);
	yyerror(p);
    }
    dummy.name = make_shared_string(name);
    dummy.type = type;
    dummy.flags = flags;
    add_to_mem_block(A_VARIABLES, (char *)&dummy, sizeof dummy);
}

short store_prog_string(str)
    char *str;
{
    short i;
    char **p;

    p = (char **) mem_block[A_STRINGS].block;
    str = make_shared_string(str);
    for (i=mem_block[A_STRINGS].current_size / sizeof str -1; i>=0; --i)
	if (p[i] == str)  {
	    free_string(str); /* Needed as string is only free'ed once. */
	    return i;
	}

    add_to_mem_block(A_STRINGS, &str, sizeof str);
    return mem_block[A_STRINGS].current_size / sizeof str - 1;
}

void add_to_case_heap(block_index,entry)
    int block_index;
    struct case_heap_entry *entry;
{
    char *heap_start;
    int offset,parent;
    int current_heap;

    if ( block_index == A_CASE_NUMBERS )
        current_heap = current_case_number_heap;
    else
        current_heap = current_case_string_heap;
    offset = mem_block[block_index].current_size - current_heap;
    heap_start = mem_block[block_index].block + current_heap;
    add_to_mem_block(block_index, (char*)entry, sizeof(*entry) );
    for ( ; offset; offset = parent ) {
        parent = ( offset - sizeof(struct case_heap_entry) ) >> 1 ;
        CASE_HEAP_ENTRY_ALIGN(parent);
        if ( ((struct case_heap_entry*)(heap_start+offset))->key <
             ((struct case_heap_entry*)(heap_start+parent))->key )
        {
            *(struct case_heap_entry*)(heap_start+offset) =
            *(struct case_heap_entry*)(heap_start+parent);
            *(struct case_heap_entry*)(heap_start+parent) = *entry;
        }
    }
#ifdef DEBUG_SWITCH
#if 0
    {
       struct case_heap_entry *ptr = (struct case_heap_entry *) heap_start;
       int noff = mem_block[block_index].current_size - current_heap;
       printf("Heap start: 0x%08x     Size: %d\n", ptr, noff);
       printf("Case heap entries: %d\n",noff / sizeof(struct case_heap_entry));
       for( ; (char *) ptr < heap_start + noff; ptr++)
       {
	  printf("   Key: %d   Addr: %d   Line: %d\n",
		 ptr->key, ptr->addr, ptr->line);
       }
    }
#endif
#endif
}

/*
 * Arrange a jump to the current position for the initialization code
 * to continue.
 */
static void transfer_init_control() {
  if (PC - 2 == last_initializer_end)
  {
    backup(1);
    do_label();
  }else {
    /*
     * Change the address of the last jump after the last
     * initializer to this point.
     */
    upd_jump(last_initializer_end, do_label());
  }
}

struct saved_code
{
  struct saved_code *next;
  int size;
  char code[1];
};

static struct saved_code *saved_code_stack = 0;

void save_code_block(int from)
{
  int size;
  struct saved_code *tmp;
  do_label();

  size=PC - from;
  tmp=(struct saved_code *)xalloc(size  + sizeof(struct saved_code) -1 );
  tmp->size=size;
  tmp->next=saved_code_stack;
  memcpy(tmp->code, mem_block[A_PROGRAM].block + from, size);
  PC -= size;
  saved_code_stack = tmp;

  last_push_local=-1;
  last_push_identifier=-1;
  last_push_index=-1;
  last_push_range=-1;
}

void pop_code_block()
{
  struct saved_code *tmp;
  tmp=saved_code_stack;
  saved_code_stack=tmp->next;
  add_to_mem_block(A_PROGRAM, tmp->code, tmp->size);
  free((char *)tmp);
  do_label();
}

void add_new_init_jump();
%}

/*
 * These values are used by the stack machine, and can not be directly
 * called from LPC.
 */
%token F_ILLEGAL
%token F_JUMP F_JUMP_WHEN_ZERO F_JUMP_WHEN_NON_ZERO
%token F_JUMP_WHEN_EQ F_JUMP_WHEN_NE
%token F_JUMP_WHEN_LT F_JUMP_WHEN_GT
%token F_JUMP_WHEN_LE F_JUMP_WHEN_GE
%token F_POP_VALUE F_DUP F_STRING F_SHORT_STRING
%token F_CALL_FUNCTION_BY_ADDRESS F_IDENTIFIER
%token F_PUSH_IDENTIFIER_LVALUE F_PUSH_LOCAL_VARIABLE_LVALUE
%token F_PUSH_INDEXED_LVALUE F_INDIRECT F_INDEX /*W*/F_PUSH_RANGE_LVALUE
%token F_CONST0 F_CONST1 F_CONST_1 F_BYTE F_NEG_BYTE F_SHORT

/*
 * These are the predefined functions that can be accessed from LPC.
 */

%token F_RETURN F_RETURN_ZERO
%token F_INC F_DEC F_POST_INC F_POST_DEC F_INC_AND_POP F_DEC_AND_POP
%token F_NUMBER F_ASSIGN F_ADD F_SUBTRACT F_MULTIPLY F_POP_VALUE2
%token F_DIVIDE F_LT F_GT F_EQ F_GE F_LE F_NE F_RETURN_ONE
%token F_ADD_EQ F_SUB_EQ F_DIV_EQ F_MULT_EQ F_ASSIGN_AND_POP
%token F_NEGATE F_SWITCH F_LAND F_LOR
%token F_SSCANF F_PARSE_COMMAND F_LOCAL_NAME
%token F_MOD F_MOD_EQ F_AGGREGATE F_M_AGGREGATE
%token F_COMPL F_AND F_AND_EQ F_OR F_OR_EQ F_XOR F_XOR_EQ
%token F_LSH F_LSH_EQ F_RSH F_RSH_EQ F_CATCH F_NOT F_RANGE
