%{
void ins_int(int i)
{
  if ( i == 0 ) {
    ins_f_byte(F_CONST0);
  }else if ( i == 1 ) {
    ins_f_byte(F_CONST1);
  } else if ( i == -1) {
    ins_f_byte(F_CONST_1);
  } else if ( i>1 && i<258) {
    ins_f_byte(F_BYTE); ins_byte(i-2);
  } else if ( i<-1 && i>-258) {
    ins_f_byte(F_NEG_BYTE); ins_byte(-i-2);
  } else if( i>=-37768 && i<32768){
    ins_f_byte(F_SHORT); ins_short(i);
  }else{
    ins_f_byte(F_NUMBER); ins_long(i);
  }
}

void ins_string(char *s)
{
  int i;
  if(!code_generation)
  {
    bytes_won+=2; /* at least! */
    return;
  }

  i=store_prog_string(s);
  if(i<256)
  {
    ins_f_byte(F_SHORT_STRING);
    ins_byte(i);
  }else{
    ins_f_byte(F_STRING);
    ins_short(i);
  }
}
%}

/*
 * These are token values that needn't have an associated code for the
 * compiled file
 */

%token F_CASE F_DEFAULT F_STATUS F_IF F_FLOAT
%token F_FLOAT_DECL F_COMMA F_INT F_SUBSCRIPT
%token F_WHILE F_DO F_FOR F_STRING_DECL F_ELSE F_CONTINUE F_BREAK
%token F_INHERIT F_COLON_COLON F_STATIC F_ARROW
%token F_OBJECT F_VOID F_MIXED F_PRIVATE F_NO_MASK F_MAPPING
%token F_PROTECTED F_PUBLIC F_VARARGS

%union
{
	int number;
	unsigned int address;	/* Address of an instruction */
	char *string;
	short type;
	struct { int key; char block; } case_label;
	struct function *funp;
	double fnumber;
}

%type <fnumber> F_FLOAT
%type <number> assign F_NUMBER constant F_LOCAL_NAME expr_list
%type <number> const1 const2 const3 const4 const5 const6 const7 const8 const9
%type <number> lvalue_list argument type basic_type optional_star expr_list2
%type <number> type_modifier type_modifier_list opt_basic_type block_or_semi
%type <number> argument_list
%type <number> m_expr_list m_expr_list2
%type <string> F_IDENTIFIER F_STRING string_constant function_name
%type <string> string_con1 string_lit1 string_lit2 string_lit3

%type <case_label> case_label

/* The following symbos return type information */

%type <type> string_plus string /*expr_index*/ expr_arrow
%type <type> function_call lvalue cast expr28 expr01 comma_expr
%type <type> expr2 expr211 expr1 expr212 expr213 expr24 expr22 expr23 expr25
%type <type> expr27 /*expr28 expr24*/ expr3 expr31 expr4 number expr0
%type <type> expr42 index_lvalue exprs
%%

all: program;

program: program def possible_semi_colon
       |	 /* empty */ ;

possible_semi_colon: /* empty */
                   | ';' { yyerror("Extra ';'. Ignored."); };

inheritance: type_modifier_list F_INHERIT string_con1 ';'
		{
		    struct object *ob;
		    struct inherit inherit;
		    int initializer;

		    ob = find_object2($3);
		    if (ob == 0) {
			inherit_file = $3;
			/* Return back to load_object() */
			YYACCEPT;
		    }
		    free($3);
		    if (ob->flags & O_APPROVED)
			approved_object = 1;
		    inherit.prog = ob->prog;
		    inherit.function_index_offset =
			mem_block[A_FUNCTIONS].current_size /
			    sizeof (struct function);
		    inherit.variable_index_offset =
			mem_block[A_VARIABLES].current_size /
			    sizeof (struct variable);
		    add_to_mem_block(A_INHERITS, &inherit, sizeof inherit);
		    copy_variables(ob->prog, $1);
		    initializer = copy_functions(ob->prog, $1);
		    if (initializer > 0) {
			struct function *funp;
			int f;
			f = define_new_function("::__INIT", 0, 0, 0, 0, 0);
			funp = FUNCTION(f);
			funp->inherit_offset = mem_block[A_INHERITS].current_size /
			    sizeof (struct inherit) - 1;
			funp->flags = NAME_STRICT_TYPES |
			    NAME_INHERITED | NAME_HIDDEN;
			funp->type = TYPE_VOID;
			funp->offset = initializer;
			transfer_init_control();
			ins_f_byte(F_CALL_FUNCTION_BY_ADDRESS);
			ins_short(f);
			ins_byte(0);	/* Actual number of arguments */
			ins_f_byte(F_POP_VALUE);
			add_new_init_jump();
		    }
		}

number: F_NUMBER
	{
	  ins_int($1);
	  if($1) $$=TYPE_NUMBER;
	  else $$=TYPE_ANY;
	};

optional_star: /* empty */ { $$ = 0; } | '*' { $$ = TYPE_MOD_POINTER; } ;

block_or_semi: block { $$ = 0; } | ';' { $$ = ';'; } ;

def: type optional_star F_IDENTIFIER
	{
	    /* Save start of function. */
	    push_explicit(do_label());

	    if ($1 & TYPE_MOD_MASK) {
		exact_types = $1 | $2;
	    } else {
		if (pragma_strict_types)
		    yyerror("\"#pragma strict_types\" requires type of function");
		exact_types = 0;
	    }
	}
	'(' argument ')'
	{
	    /*
	     * Define a prototype. If it is a real function, then the
	     * prototype will be replaced below.
	     */
	    define_new_function($3, $6, 0, 0,
				NAME_UNDEFINED|NAME_PROTOTYPE, $1 | $2);
	}
        block_or_semi
	{
	  char tmp;

	    /* Either a prototype or a block */
	    if ($9 == ';') {
		(void)pop_address(); /* Not used here */
	    } else {
		define_new_function($3, $6, current_number_of_locals - $6,
			pop_address(), 0, $1 | $2);
		ins_f_byte(F_RETURN_ZERO);
	    }
	    free_all_local_names();
	    free($3);		/* Value was copied above */
	}
   | type name_list ';' { if ($1 == 0) yyerror("Missing type"); }
   | inheritance ;

new_arg_name: type optional_star F_IDENTIFIER
	{
	    if (exact_types && $1 == 0) {
		yyerror("Missing type for argument");
		add_local_name($3, TYPE_ANY);	/* Supress more errors */
	    } else {
		add_local_name($3, $1 | $2);
	    }
	}
	  | type F_LOCAL_NAME
		{yyerror("Illegal to redeclare local name"); } ;

argument: /* empty */ { $$ = 0; }
	  | argument_list ;

argument_list: new_arg_name { $$ = 1; }
	     | argument_list ',' new_arg_name { $$ = $1 + 1; } ;

type_modifier: F_NO_MASK { $$ = TYPE_MOD_NO_MASK; }
	     | F_STATIC { $$ = TYPE_MOD_STATIC; }
	     | F_PRIVATE { $$ = TYPE_MOD_PRIVATE; }
	     | F_PUBLIC { $$ = TYPE_MOD_PUBLIC; }
	     | F_VARARGS { $$ = TYPE_MOD_VARARGS; }
	     | F_PROTECTED { $$ = TYPE_MOD_PROTECTED; } ;

type_modifier_list: /* empty */ { $$ = 0; }
		  | type_modifier type_modifier_list { $$ = $1 | $2; } ;

type: type_modifier_list opt_basic_type { $$ = $1 | $2; current_type = $$; } ;

cast: '(' basic_type optional_star ')'
	{
	    $$ = $2 | $3;
	} ;

opt_basic_type: basic_type | /* empty */ { $$ = TYPE_UNKNOWN; } ;

basic_type: F_STATUS { $$ = TYPE_NUMBER; current_type = $$; }
	| F_INT { $$ = TYPE_NUMBER; current_type = $$; }
	| F_STRING_DECL { $$ = TYPE_STRING; current_type = $$; }
	| F_OBJECT { $$ = TYPE_OBJECT; current_type = $$; }
	| F_VOID {$$ = TYPE_VOID; current_type = $$; }
	| F_MIXED { $$ = TYPE_ANY; current_type = $$; }
	| F_MAPPING { $$ = TYPE_MAPPING; current_type = $$; }
	| F_FLOAT_DECL { $$ = TYPE_FLOAT; current_type = $$; };     

name_list: new_name
	 | new_name ',' name_list;

new_name: optional_star F_IDENTIFIER
	{
	    define_variable($2, current_type | $1, 0);
	    free($2);
	}
| optional_star F_IDENTIFIER
	{
	    int var_num;
	    define_variable($2, current_type | $1, 0);
	    var_num = verify_declared($2);
	    transfer_init_control();
	    ins_f_byte(F_PUSH_IDENTIFIER_LVALUE);
	    ins_byte(var_num);
	}
	'=' expr0
	{
	    if (!compatible_types((current_type | $1) & TYPE_MOD_MASK, $5)){
		char buff[100];
		sprintf(buff, "Type mismatch %s when initializing %s",
			get_two_types(current_type | $1, $5), $2);
		yyerror(buff);
	    }
	    ins_f_byte(F_ASSIGN);
	    ins_f_byte(F_POP_VALUE);
	    add_new_init_jump();
	    free($2);
	} ;
block: '{' local_declarations statements '}'
	{ ; };

local_declarations: /* empty */
		  | local_declarations basic_type local_name_list ';' ;

new_local_name: optional_star F_IDENTIFIER
	{
	    add_local_name($2, current_type | $1);
	} ;

local_name_list: new_local_name
	| new_local_name ',' local_name_list ;

statements: /* empty */
	  | statements statement
	  | error ';' ;

statement: comma_expr ';'
	{
	    ins_f_byte(F_POP_VALUE);
	    if (d_flag)
	    {
	      ins_f_byte(F_BREAK_POINT);
	      ins_f_byte(F_POP_VALUE);
	    }
	    /* if (exact_types && !TYPE($1,TYPE_VOID))
		yyerror("Value thrown away"); */
	}
	 | cond | while | do | for | switch | case | default | return ';'
	 | block
  	 | /* empty */ ';'
	 | F_BREAK ';'	/* This code is a jump to a jump */
		{
		    if (current_break_address == 0)
			yyerror("break statement outside loop or switch");

		    if(code_generation)
		    {
		      if(current_break_address & BREAK_UNKNOWN)
		      {
			int i;
			ins_f_byte(F_JUMP);
			i=PC;
			ins_short(current_break_address & 0xffff);
			current_break_address = i | BREAK_UNKNOWN;
		      }
		      else
		      {
			ins_f_byte(F_JUMP);
			ins_jump(current_break_address);
		      }
		    }else{
		      bytes_won+=3;
		    }

		}
	 | F_CONTINUE ';'	/* This code is a jump to a jump */
		{
		  if(code_generation)
		  {

		    if (current_continue_address == 0)
		    {
		      yyerror("continue statement outside loop");
		    }
		    else if(current_continue_address & BREAK_UNKNOWN)
		    {
		      int i;
		      ins_f_byte(F_JUMP);
		      i=PC;
		      ins_short(current_continue_address & 0xffff);
		      current_continue_address = i | BREAK_UNKNOWN;
		    } else {
		      ins_f_byte(F_JUMP);
		      ins_jump(current_continue_address);
		    }
		  }else{
		    bytes_won+=3;
		  }
		}
         ;

while:  F_WHILE
        {
	  int i;
	  push_explicit(current_continue_address);
	  push_explicit(current_break_address);
	  bytes_optimized+=6;
	  current_break_address = BREAK_UNKNOWN;
          current_continue_address = BREAK_UNKNOWN;
	  $<number>$=do_label();
	}
        '(' comma_expr ')'
	{
          save_code_block($<number>2);
          ins_f_byte(F_JUMP);
	  ins_jump(0);
          $<number>$=do_label();
	}
        statement
	{
	  upd_jump($<number>6 -2, do_label());
	  pop_jump_list(current_continue_address, do_label());
	  pop_code_block();
	  ins_f_byte(F_JUMP_WHEN_NON_ZERO);
          ins_jump($<number>6);
	  pop_jump_list(current_break_address, do_label());

	  current_break_address = pop_address();
	  current_continue_address = pop_address();
        }

do: F_DO 
    {
      int tmp_save;
      push_explicit(current_continue_address);
      push_explicit(current_break_address);

      bytes_optimized+=6;

      current_break_address = BREAK_UNKNOWN;
      current_continue_address = do_label();
    }
    statement F_WHILE '(' comma_expr ')' ';'
    {
      code_generation=1;
      ins_f_byte(F_JUMP_WHEN_NON_ZERO);
      ins_jump(current_continue_address);
      pop_jump_list(current_break_address, do_label());
      current_break_address = pop_address();
      current_continue_address = pop_address();
    }

for: F_FOR '(' for_expr ';'
   {
     push_explicit(current_continue_address);
     push_explicit(current_break_address);
     ins_f_byte(F_POP_VALUE);

     $<number>$=do_label();
     bytes_optimized+=9;
     
     current_continue_address=BREAK_UNKNOWN;
     current_break_address=BREAK_UNKNOWN;
   }
   for_expr ';'
   {
     save_code_block($<number>5);
   }
   for_expr ')'
   {
     ins_f_byte(F_POP_VALUE);
     save_code_block($<number>5);

     ins_f_byte(F_JUMP);
     ins_jump(0);
     $<number>$=do_label();
   }
   statement
   {
     pop_jump_list(current_continue_address,do_label());
     pop_code_block();  /* the update statement (i++) */
     upd_jump($<number>11 - 2, do_label());
     pop_code_block();  /* the test  (i<sizeof(foo)) */
     ins_f_byte(F_JUMP_WHEN_NON_ZERO);
     ins_jump($<number>11);
     pop_jump_list(current_break_address,do_label());

     current_break_address = pop_address();
     current_continue_address = pop_address();
   }

for_expr: /* EMPTY */ { ins_f_byte(F_CONST1); }
        | comma_expr { ; } ;

switch: F_SWITCH '(' comma_expr ')'
    {
	push_explicit(current_case_number_heap);
	push_explicit(current_case_string_heap);
	push_explicit(zero_case_label);
	push_explicit(current_break_address);
	push_explicit(current_switch);
#ifndef NO_OVERLAP_CHECK
	/* BEWARE: This won't work if sizeof(int) < sizeof(pointer) */
	push_explicit(top_case);
	top_case = NULL;
#endif
	do_label();
	ins_f_byte(F_SWITCH);
	ins_byte(0xff); /* kind of table */
	current_case_number_heap = mem_block[A_CASE_NUMBERS].current_size;
	current_case_string_heap = mem_block[A_CASE_STRINGS].current_size;
	zero_case_label = NO_STRING_CASE_LABELS;
	current_break_address = BREAK_UNKNOWN;
	current_switch=do_label();
	ins_short(0); /* address of table */
	ins_short(0); /* end of table */
	ins_short(0); /* default address */
    }
    statement
    {
	char *heap_start;
	int heap_end_offs;
	int i,o;
	int current_key,last_key;
	/* int size_without_table; */
	int block_index;
	int current_case_heap;
	int lookup_start;
	int lookup_start_key;
	int jumpout;
#ifdef DEBUG_SWITCH
	int switch_code_start, switch_code_end;
#endif

	if(code_generation)
	{
	  ins_f_byte(F_JUMP);
	  jumpout=do_label();
	  ins_short(0);
	  do_label();
	} else {
	  jumpout=0;
	}

	if(zero_case_label & (NO_STRING_CASE_LABELS|SOME_NUMERIC_CASE_LABELS))
	{
	    block_index = A_CASE_NUMBERS;
	    current_case_heap = current_case_number_heap;
	} else {
	    block_index = A_CASE_STRINGS;
	    current_case_heap = current_case_string_heap;
	    if (zero_case_label&0xffff) {
		struct case_heap_entry temp;

		temp.key = (int) ZERO_AS_STR_CASE_LABEL;
		temp.addr = zero_case_label;
		temp.line = 0; /* if this is accessed later, something is
				* really wrong				  */
		add_to_case_heap(A_CASE_STRINGS,&temp);
	    }
	}
	heap_start = mem_block[block_index].block + current_case_heap ;
	heap_end_offs = mem_block[block_index].current_size -current_case_heap;
	if (!heap_end_offs)
	  yyerror("switch without case not supported");

        /* add a dummy entry so that we can always
        * assume we have no or two childs
        */
        add_to_mem_block(block_index, "\0\0\0\0\0\0\0\0",
            sizeof(struct case_heap_entry) );

        /* read out the heap and build a sorted table */
	/* the table could be optimized better, but let's first see
	* how much switch is used at all when it is full-featured...
	*/
	mem_block[A_CASE_LABELS].current_size = 0;
	lookup_start = 0;
	last_key=0; /* Hope this is right /Profezzorn */
	lookup_start_key = ((struct case_heap_entry*)heap_start)->key;
        for( ; ((struct case_heap_entry*)heap_start)->addr; )
        {
            int offset;
	    int curr_line,last_line;
	    unsigned short current_addr,last_addr = 0xffff;
	    int range_start;

            current_key = ((struct case_heap_entry*)heap_start)->key ;
            curr_line = ((struct case_heap_entry*)heap_start)->line ;
            current_addr = ((struct case_heap_entry*)heap_start)->addr ;
            if ( current_key == last_key &&
              mem_block[A_CASE_LABELS].current_size )
            {
                char buf[90];

                sprintf(buf,"Duplicate case in line %d and %d",
		    last_line, curr_line);
                yyerror(buf);
            }
	    if (curr_line) {
		if (last_addr == 1) {
                    char buf[120];
    
		    sprintf(buf,
"Discontinued case label list range, line %d by line %d",
		      last_line, curr_line);
                    yyerror(buf);
		}
		  else if (current_key == last_key + 1
		    && current_addr == last_addr) {
		    if (mem_block[A_CASE_LABELS].current_size
		      != range_start + 6) {
		      *(short*)(mem_block[A_CASE_LABELS].block+range_start+4)
			=1;
		      mem_block[A_CASE_LABELS].current_size = range_start + 6;
		    }
		} else {
		    range_start = mem_block[A_CASE_LABELS].current_size;
		}
	    }
            last_key = current_key;
	    last_line = curr_line;
	    last_addr = current_addr;
	    add_to_mem_block(A_CASE_LABELS,
                (char *)&current_key, sizeof(long) );
	    add_to_mem_block(A_CASE_LABELS,
		(char *)&current_addr, sizeof(short) );
            for ( offset = 0; ; )
            {

                int child1,child2;

                child1 = ( offset << 1 ) + sizeof(struct case_heap_entry);
                child2 = child1 + sizeof(struct case_heap_entry);
                if ( child1 >= heap_end_offs ) break;

                if ( ((struct case_heap_entry*)(heap_start+child1))->addr &&
                  ( !((struct case_heap_entry*)(heap_start+child2))->addr ||
                   ((struct case_heap_entry*)(heap_start+child1))->key <=
                   ((struct case_heap_entry*)(heap_start+child2))->key  ) )
                {
                    *(struct case_heap_entry*)(heap_start+offset) =
                    *(struct case_heap_entry*)(heap_start+child1);
                    offset = child1;
                }
		else if (((struct case_heap_entry*)(heap_start+child2))->addr )
		{
		  *(struct case_heap_entry*)(heap_start+offset) =
		    *(struct case_heap_entry*)(heap_start+child2);
		  offset = child2;
		}
		else
		{
		  break;
		}
		  
            }
            ((struct case_heap_entry*)(heap_start+offset))->addr = 0;
        }

#ifdef DEBUG_SWITCH
       switch_code_start = current_switch + 6;
       switch_code_end = mem_block[A_PROGRAM].current_size;
#endif

	/* write start of table */
        upd_short(current_switch, do_label());

	add_to_mem_block(A_PROGRAM, mem_block[A_CASE_LABELS].block,
            mem_block[A_CASE_LABELS].current_size );
        /* calculate starting index for itarative search at execution time */
        for(i=0xf0,o=6; o<<1 <= mem_block[A_CASE_LABELS].current_size; )
            i++,o<<=1;
        if (block_index == A_CASE_STRINGS) i = ( i << 4 ) | 0xf;
        /* and store it */
        mem_block[A_PROGRAM].block[current_switch-1] &= i;

	mem_block[A_CASE_NUMBERS].current_size = current_case_number_heap;
	mem_block[A_CASE_STRINGS].current_size = current_case_string_heap;

        upd_short(current_switch+2, do_label());
        if(jumpout)
	  upd_jump(jumpout, do_label());
	pop_jump_list(current_break_address,do_label());

	if ( !read_short(current_switch+4 ) )
	    upd_short(current_switch+4,     /* no default given ->  */
	      do_label());  /* create one           */

#ifndef NO_OVERLAP_CHECK
	{
	   struct case_overlap_check *c, *n;
	   for(c = top_case; c; c = n)
	   {
	      n = c->next;
	      free(c);
	   }
	   top_case = (struct case_overlap_check *) pop_address();
	}
#endif

    	current_switch = pop_address();
    	current_break_address = pop_address();
	zero_case_label = pop_address();
    	current_case_string_heap = pop_address();
    	current_case_number_heap = pop_address();

#ifdef DEBUG_SWITCH
    {
       char *ptr = mem_block[A_CASE_LABELS].block;
       int noff = mem_block[A_CASE_LABELS].current_size;
#if 0
       printf("Switch code start: %lu   End: %lu\n",
	      switch_code_start, switch_code_end);
       printf("Heap start: 0x%08x     Size: %d\n", ptr, noff);
       printf("Case heap entries: %d\n", noff / 6);
#endif
       for( ; ptr < mem_block[A_CASE_LABELS].block + noff; ptr += 6)
       {
	  int key;
	  unsigned short addr;
	  ((char *)&key)[0] = ptr[0];
	  ((char *)&key)[1] = ptr[1];
	  ((char *)&key)[2] = ptr[2];
	  ((char *)&key)[3] = ptr[3];
	  ((char *)&addr)[0] = ptr[4];
	  ((char *)&addr)[1] = ptr[5];
#ifdef DEBUG
	  /* The special case is for range */
	  if(addr != 1 &&(addr < switch_code_start || addr >= switch_code_end))
	  {
	     printf("   Codestart: %d   Codeend: %d\n",
		    switch_code_start, switch_code_end);
	     printf("   Key: %d         Addr: %u\n", key, addr);
	     fatal("Trying to generate bad switch table.\n");
	  }
#endif
#if 0
	  printf("   Key: %d         Addr: %u\n", key, addr);
#endif
       }
    }
#endif

    } ;

case: F_CASE case_label ':'
    {
	struct case_heap_entry temp;

	if ( !current_switch ) {
	    yyerror("Case outside switch");
	    break;
	}

#ifndef NO_OVERLAP_CHECK
	{
	   struct case_overlap_check *p, **pp;
	   p = (struct case_overlap_check *) xalloc(sizeof(*p));
	   p->line = current_line;
	   p->lkey = $2.key;
	   p->hkey = $2.key;
	   for(pp = &top_case; *pp; pp = &(*pp)->next)
	   {
	      if((*pp)->hkey < p->lkey)
		 continue;
	      if((*pp)->lkey > p->hkey)
		break;
	      yyerror("Overlapping case entries");
	      break;
	   }
	   p->next = *pp;
	   *pp = p;
	}
#endif

	temp.key = $2.key;
	temp.addr = do_label();
	temp.line = current_line;
	add_to_case_heap($2.block,&temp);
    }
    | F_CASE case_label F_RANGE case_label ':'
    {
	struct case_heap_entry temp;

	if ( !current_switch ) {
	    yyerror("Case outside switch");
	    break;
	}
	if ( $2.block != A_CASE_NUMBERS || $4.block != A_CASE_NUMBERS )
	    yyerror("String case labels not allowed as range bounds");
	if ($2.key > $4.key) break;

#ifndef NO_OVERLAP_CHECK
	{
	   struct case_overlap_check *p, **pp;
	   p = (struct case_overlap_check *) xalloc(sizeof(*p));
	   p->line = current_line;
	   p->lkey = $2.key;
	   p->hkey = $4.key;
	   for(pp = &top_case; *pp; pp = &(*pp)->next)
	   {
	      if((*pp)->hkey < p->lkey)
		 continue;
	      if((*pp)->lkey > p->hkey)
		break;
	      yyerror("Overlapping case entries");
	      break;
	   }
	   p->next = *pp;
	   *pp = p;
	}
#endif

	temp.key = $2.key;
	temp.addr = 1;
	temp.line = current_line;
	add_to_case_heap(A_CASE_NUMBERS,&temp);
	temp.key = $4.key;
	temp.addr = do_label();
	temp.line = 0;
	add_to_case_heap(A_CASE_NUMBERS,&temp);
    } ;
	
case_label: constant
        {
	    if ( !(zero_case_label & NO_STRING_CASE_LABELS) )
		yyerror("Mixed case label list not allowed");
	    if ( $$.key = $1 )
	        zero_case_label |= SOME_NUMERIC_CASE_LABELS;
	    else
		zero_case_label |= do_label();
	    $$.block = A_CASE_NUMBERS;
	}
	  | string_constant
        {
	    if ( zero_case_label & SOME_NUMERIC_CASE_LABELS )
		yyerror("Mixed case label list not allowed");
	    zero_case_label &= ~NO_STRING_CASE_LABELS;
            store_prog_string($1);   /* Store away  /Gwendolyn */
            $$.key = (int)$1;
	    $$.block = A_CASE_STRINGS;
	    free_string($1);   /* We only want one ref  /Gwendolyn */
        }
	  ;

constant: const1
	| constant '|' const1 { $$ = $1 | $3; } ;

const1: const2
      | const1 '^' const2 { $$ = $1 ^ $3; } ;

const2: const3
      | const2 '&' const3 { $$ = $1 & $3; } ;

const3: const4
      | const3 F_EQ const4 { $$ = $1 == $3; }
      | const3 F_NE const4 { $$ = $1 != $3; } ;

const4: const5
      | const4 '>'  const5 { $$ = $1 >  $3; }
      | const4 F_GE const5 { $$ = $1 >= $3; }
      | const4 '<'  const5 { $$ = $1 <  $3; }
      | const4 F_LE const5 { $$ = $1 <= $3; } ;

const5: const6
      | const5 F_LSH const6 { $$ = $1 << $3; }
      | const5 F_RSH const6 { $$ = $1 >> $3; } ;

const6: const7
      | const6 '+' const7 { $$ = $1 + $3; }
      | const6 '-' const7 { $$ = $1 - $3; } ;

const7: const8
      | const7 '*' const8 { $$ = $1 * $3; }
      | const7 '%' const8 
          {
	    if($3 == 0) 
	      yyerror("Mod by zero in constant expresion");
	    else
	      $$ = $1 % $3; 
	  }
      | const7 '/' const8 
          {
	    if($3 == 0) 
	      yyerror("Division by zero in constant expresion");
	    else
	      $$ = $1 / $3; 
	  } ;

const8: const9
      | '(' constant ')' { $$ = $2; } ;

const9: F_NUMBER
      | '-'   F_NUMBER { $$ = -$2; }
      | F_NOT F_NUMBER { $$ = !$2; }
      | '~'   F_NUMBER { $$ = ~$2; } ;

default: F_DEFAULT ':'
    {
	if ( !current_switch )
	{
	    yyerror("Default outside switch");
	    break;
	}
	if ( read_short(current_switch+4 ) )
	    yyerror("Duplicate default");
	upd_short(current_switch+4, do_label());
    } ;


comma_expr: expr0 { $$ = $1; }
          | comma_expr { ins_f_byte(F_POP_VALUE); }
	',' expr0
	{ $$ = $4; } ;

expr0:  expr01
     | lvalue assign expr0
	{
	    if (exact_types && !compatible_types($1, $3) &&
		!($1 == TYPE_STRING && $3 == TYPE_NUMBER && $2 == F_ADD_EQ))
	    {
		type_error("Bad assignment. Rhs", $3);
	    }
	    ins_f_byte($2);
	    $$ = $3;
	}
     | error assign expr01 { yyerror("Illegal LHS");  $$ = TYPE_ANY; };

expr01: expr1 { $$ = $1; }
     | expr1 '?'
	{
	    ins_f_byte(F_JUMP_WHEN_ZERO);
	    push_address();
	    ins_jump(0);
	}
      expr01
	{
	    int i;
	    i = pop_address();
	    ins_f_byte(F_JUMP);
	    push_address();
	    ins_short(0);
	    upd_jump(i, do_label());
	}
      ':' expr01
	{
	    upd_jump(pop_address(), do_label());
	    if (exact_types && !compatible_types($4, $7)) {
		type_error("Different types in ?: expr", $4);
		type_error("                      and ", $7);
	    }
	    if ($4 == TYPE_ANY) $$ = $7;
	    else if ($7 == TYPE_ANY) $$ = $4;
	    else if (TYPE($4, TYPE_MOD_POINTER|TYPE_ANY)) $$ = $7;
	    else if (TYPE($7, TYPE_MOD_POINTER|TYPE_ANY)) $$ = $4;
	    else $$ = $4;
	};

assign: '=' { $$ = F_ASSIGN; }
      | F_AND_EQ { $$ = F_AND_EQ; }
      | F_OR_EQ { $$ = F_OR_EQ; }
      | F_XOR_EQ { $$ = F_XOR_EQ; }
      | F_LSH_EQ { $$ = F_LSH_EQ; }
      | F_RSH_EQ { $$ = F_RSH_EQ; }
      | F_ADD_EQ { $$ = F_ADD_EQ; }
      | F_SUB_EQ { $$ = F_SUB_EQ; }
      | F_MULT_EQ { $$ = F_MULT_EQ; }
      | F_MOD_EQ { $$ = F_MOD_EQ; }
      | F_DIV_EQ { $$ = F_DIV_EQ; };

return: F_RETURN
	{
	    if (exact_types && !TYPE(exact_types, TYPE_VOID))
		type_error("Must return a value for a function declared",
			   exact_types);
	    ins_f_byte(F_RETURN_ZERO);
	}
      | F_RETURN comma_expr
	{
	    if (exact_types && !TYPE($2, exact_types & TYPE_MOD_MASK))
		type_error("Return type not matching", exact_types);
	    ins_f_byte(F_RETURN);
	};

expr_list: /* empty */		{ $$ = 0; }
	 | expr_list2		{ $$ = $1; }
	 | expr_list2 ','	{ $$ = $1; } ; /* Allow a terminating comma */

expr_list2: expr0		{ $$ = 1; add_arg_type($1); }
         | expr_list2 ',' expr0	{ $$ = $1 + 1; add_arg_type($3); } ;


m_expr_list: /* empty */	{ $$ = 0; }
	 | m_expr_list2		{ $$ = $1; }
	 | m_expr_list2 ','	{ $$ = $1; } ; /* Allow a terminating comma */

m_expr_list2: expr0 ':' expr1	{ $$ = 2; add_arg_type($1); add_arg_type($3); }
	 | m_expr_list2 ',' expr0 ':' expr1 { $$ = $1 + 2; add_arg_type($3); add_arg_type($5); }


expr1: expr2 { $$ = $1; }
     | expr2 F_LOR
       {
	 ins_f_byte(F_LOR);
	 push_address();
	 ins_jump(0);
       }
       expr1
	{
	    upd_jump(pop_address(), do_label());
	    if ($1 == $4)
		$$ = $1;
	    else
		$$ = TYPE_ANY;	/* Return type can't be known */
	};

expr2: expr211 { $$ = $1; }
     | expr211 F_LAND
	{
	  ins_f_byte(F_LAND);
	  push_address();
	  ins_jump(0);
	}
       expr2
	{
	    upd_jump(pop_address(), do_label());
	    if ($1 == $4)
		$$ = $1;
	    else
		$$ = TYPE_ANY;	/* Return type can't be known */
	} ;

expr211: expr212
       | expr211 '|' expr212
          {
	      if (exact_types && !TYPE($1,TYPE_NUMBER))
		  type_error("Bad argument 1 to |", $1);
	      if (exact_types && !TYPE($3,TYPE_NUMBER))
		  type_error("Bad argument 2 to |", $3);
	      $$ = TYPE_NUMBER;
	      ins_f_byte(F_OR);
	  };

expr212: expr213
       | expr212 '^' expr213
	  {
	      if (exact_types && !TYPE($1,TYPE_NUMBER))
		  type_error("Bad argument 1 to ^", $1);
	      if (exact_types && !TYPE($3,TYPE_NUMBER))
		  type_error("Bad argument 2 to ^", $3);
	      $$ = TYPE_NUMBER;
	      ins_f_byte(F_XOR);
	  };

expr213: expr22
       | expr213 '&' expr22
	{
#if 0
	      if (!TYPE($1,TYPE_MOD_POINTER | TYPE_ANY) || 
		  !TYPE($3,TYPE_MOD_POINTER | TYPE_ANY) ) {
	          if (exact_types && !TYPE($1,TYPE_NUMBER))
		      type_error("Bad argument 1 to &", $1);
	          if (exact_types && !TYPE($3,TYPE_NUMBER))
		      type_error("Bad argument 2 to &", $3);
	      }
#else
	      $$ = TYPE_ANY;
	      if(exact_types)
	      {
		 if(($1 & TYPE_MOD_POINTER) && ($3 & TYPE_MOD_POINTER))
		 {
		    if(($1 & TYPE_MOD_MASK) == ($3 & TYPE_MOD_MASK))
		       $$ = ($1 & $3);
		    else
		       $$ = TYPE_ANY | TYPE_MOD_POINTER;
		 }
		 else if(!TYPE($1, TYPE_NUMBER))
		    type_error("Bad argument 1 to &", $1);
		 else if(!TYPE($3, TYPE_NUMBER))
		    type_error("Bad argument 2 to &", $3);
		 else
		    $$ = TYPE_NUMBER;
	      }
#endif
	      ins_f_byte(F_AND);
	  };

expr22: expr23
      | expr24 F_EQ expr24
	{
	    int t1 = $1 & TYPE_MOD_MASK, t2 = $3 & TYPE_MOD_MASK;
	    if (exact_types && t1 != t2 && t1 != TYPE_ANY && t2 != TYPE_ANY) {
		type_error("== always false because of different types", $1);
		type_error("                               compared to", $3);
	    }
	    ins_f_byte(F_EQ);
	    $$ = TYPE_NUMBER;
	};
      | expr24 F_NE expr24
	{
	    int t1 = $1 & TYPE_MOD_MASK, t2 = $3 & TYPE_MOD_MASK;
	    if (exact_types && t1 != t2 && t1 != TYPE_ANY && t2 != TYPE_ANY) {
		type_error("!= always true because of different types", $1);
		type_error("                               compared to", $3);
	    }
	    ins_f_byte(F_NE);
	    $$ = TYPE_NUMBER;
	};

expr23: expr24
      | expr24 '>' expr24
	{ $$ = TYPE_NUMBER; ins_f_byte(F_GT); };
      | expr24 F_GE expr24
	{ $$ = TYPE_NUMBER; ins_f_byte(F_GE); };
      | expr24 '<' expr24
	{ $$ = TYPE_NUMBER; ins_f_byte(F_LT); };
      | expr24 F_LE expr24
	{ $$ = TYPE_NUMBER; ins_f_byte(F_LE); };

expr24: expr25
      | string
      | expr25 '+' string
	{
	    ins_f_byte(F_ADD); $$ = TYPE_ANY;
	};
      | expr24 F_LSH expr25
	{
	    ins_f_byte(F_LSH);
	    $$ = TYPE_NUMBER;
	    if (exact_types && !TYPE($1, TYPE_NUMBER))
		type_error("Bad argument number 1 to '<<'", $1);
	    if (exact_types && !TYPE($3, TYPE_NUMBER))
		type_error("Bad argument number 2 to '<<'", $3);
	};
      | expr24 F_RSH expr25
	{
	    ins_f_byte(F_RSH);
	    $$ = TYPE_NUMBER;
	    if (exact_types && !TYPE($1, TYPE_NUMBER))
		type_error("Bad argument number 1 to '>>'", $1);
	    if (exact_types && !TYPE($3, TYPE_NUMBER))
		type_error("Bad argument number 2 to '>>'", $3);
	};

exprs: expr25 '+' string_plus expr27
	{
	    ins_f_byte(F_ADD); $$ = TYPE_ANY;
	};
      | string_plus expr27
      | expr25 '+' expr27

expr25: expr27
      | exprs  	/* Type checks of this case is complicated */
	{
	    ins_f_byte(F_ADD); $$ = TYPE_ANY;
	};
      | expr25 '-' expr27
	{
	    int bad_arg = 0;

	    if (exact_types) {
		if (!TYPE($1, TYPE_NUMBER) && !($1 & TYPE_MOD_POINTER) ) {
                    type_error("Bad argument number 1 to '-'", $1);
		    bad_arg++;
		}
		if (!TYPE($3, TYPE_NUMBER) && !($3 & TYPE_MOD_POINTER) ) {
                    type_error("Bad argument number 2 to '-'", $3);
		    bad_arg++;
		}
	    }
	    $$ = TYPE_ANY;
#if 0
	    if (($1 & $3 & TYPE_MOD_POINTER))
	      $$ = $1;
	    if (!($1 & $3 & TYPE_MOD_POINTER))
	    {
	      if (exact_types && $$ != TYPE_ANY && !bad_arg)
		yyerror("Arguments to '-' don't match");
	      $$ = TYPE_NUMBER;
	    }
#else
	    if(($1 & TYPE_MOD_MASK) == ($3 & TYPE_MOD_MASK))
	       $$ = $1 & $3;
	    else if($1 & $3 & TYPE_MOD_POINTER)
	       $$ = TYPE_ANY | TYPE_MOD_POINTER;
	    else if(($1 & TYPE_MOD_MASK) != TYPE_ANY &&
		    ($3 & TYPE_MOD_MASK) != TYPE_ANY)
	    {
	       if(exact_types && !bad_arg)
		  yyerror("Arguments to '-' don't match");
	    }
#endif
	    ins_f_byte(F_SUBTRACT);
	};

expr27: expr28
      | expr27 '*' expr3
	{
	    if (exact_types && !TYPE($1, TYPE_NUMBER))
		type_error("Bad argument number 1 to '*'", $1);
	    if (exact_types && !TYPE($3, TYPE_NUMBER))
		type_error("Bad argument number 2 to '*'", $3);
	    ins_f_byte(F_MULTIPLY);
	    $$ = TYPE_NUMBER;
	};
      | expr27 '%' expr3
	{
	    if (exact_types && !TYPE($1, TYPE_NUMBER))
		type_error("Bad argument number 1 to '%'", $1);
	    if (exact_types && !TYPE($3, TYPE_NUMBER))
		type_error("Bad argument number 2 to '%'", $3);
	    ins_f_byte(F_MOD);
	    $$ = TYPE_NUMBER;
	};
      | expr27 '/' expr3
	{
	    if (exact_types && !TYPE($1, TYPE_NUMBER))
		type_error("Bad argument number 1 to '/'", $1);
	    if (exact_types && !TYPE($3, TYPE_NUMBER))
		type_error("Bad argument number 2 to '/'", $3);
	    ins_f_byte(F_DIVIDE);
	    $$ = TYPE_NUMBER;
	};

expr28: expr3
	| cast expr3
        {
	  $$ = $1;
	  if (exact_types &&
	      $2 != TYPE_ANY &&
	      $2 != TYPE_UNKNOWN &&
	      $1 != TYPE_VOID &&
	      $1 != ($2 & TYPE_MOD_MASK))
	    type_error("Casts are only legal for type mixed, or when unknown", $2);
	} ;

expr3: expr31
     | F_INC lvalue
        {
	    ins_f_byte(F_INC);
	    if (exact_types && !TYPE($2, TYPE_NUMBER))
		type_error("Bad argument to ++", $2);
	    $$ = TYPE_NUMBER;
	};
     | F_DEC lvalue
        {
	    ins_f_byte(F_DEC);
	    if (exact_types && !TYPE($2, TYPE_NUMBER))
		type_error("Bad argument to --", $2);
	    $$ = TYPE_NUMBER;
	};
     | F_NOT expr3
	{
	    ins_f_byte(F_NOT);	/* Any type is valid here. */
	    $$ = TYPE_NUMBER;
	};
     | '~' expr3
	{
	    ins_f_byte(F_COMPL);
	    if (exact_types && !TYPE($2, TYPE_NUMBER))
		type_error("Bad argument to ~", $2);
	    $$ = TYPE_NUMBER;
	};
     | '-' expr3
	{
	    ins_f_byte(F_NEGATE);
	    if (exact_types && !TYPE($2, TYPE_NUMBER))
		type_error("Bad argument to unary '-'", $2);
	    $$ = TYPE_NUMBER;
	};

expr31: expr4
      | lvalue F_INC
         {
	     ins_f_byte(F_POST_INC);
	     if (exact_types && !TYPE($1, TYPE_NUMBER))
		 type_error("Bad argument to ++", $1);
	     $$ = TYPE_NUMBER;
	 };
      | lvalue F_DEC
         {
	     ins_f_byte(F_POST_DEC);
	     if (exact_types && !TYPE($1, TYPE_NUMBER))
		 type_error("Bad argument to --", $1);
	     $$ = TYPE_NUMBER;
	 };

expr4: lvalue
	{
	  int pos = PC;
	  if(code_generation)
	  {
	    /* Some optimization. Replace the push-lvalue with push-value */
	    if (last_push_identifier == pos-2)
		mem_block[A_PROGRAM].block[last_push_identifier] =
		    F_IDENTIFIER - F_OFFSET;
	    else if (last_push_local == pos-2)
		mem_block[A_PROGRAM].block[last_push_local] =
		    F_LOCAL_NAME - F_OFFSET;
	    else if (last_push_index == pos-1)
		mem_block[A_PROGRAM].block[last_push_index] =
		    F_INDEX - F_OFFSET;
	    else if (last_push_range == pos-1)
		mem_block[A_PROGRAM].block[last_push_range] =
		    F_RANGE - F_OFFSET;
	    else
		fatal("Should be a push at this point !\n");
	    $$ = $1;
	  }
	}
     | expr42;

expr42: function_call
     | number
     | '(' comma_expr ')' { $$ = $2; }
     | catch { $$ = TYPE_ANY; }
     | sscanf { $$ = TYPE_NUMBER; }
     | parse_command { $$ = TYPE_NUMBER; }
     | '(' '{' expr_list '}' ')'
       {
	   pop_arg_stack($3);		/* We don't care about these types */
	   ins_f_byte(F_AGGREGATE);
	   ins_short($3);
	   $$ = TYPE_MOD_POINTER | TYPE_ANY;
       }
     | '(' '[' m_expr_list ']' ')'
       {
	   pop_arg_stack($3);
	   ins_f_byte(F_M_AGGREGATE);
	   ins_short($3);
	   $$ = TYPE_MAPPING;
       };


catch: F_CATCH
     {
       ins_f_byte(F_CATCH);
       push_address();
       ins_jump(0);
     }
     '(' comma_expr ')'
     {
       ins_f_byte(F_POP_VALUE);
#if 1
       ins_f_byte(F_CONST0);
       ins_f_byte(F_THROW);
#else
       ins_f_byte(F_RETURN);
#endif
       upd_jump(pop_address(),do_label());
     };

sscanf: F_SSCANF '(' expr0 ',' expr0 lvalue_list ')'
	{
	    ins_f_byte(F_SSCANF);
            ins_byte($6 + 2);
	}

parse_command: F_PARSE_COMMAND '(' expr0 ',' expr0 ',' expr0 lvalue_list ')'
	{
	    ins_f_byte(F_PARSE_COMMAND);
            ins_byte($8 + 3);
	}

lvalue_list: /* empty */ { $$ = 0; }
	   | ',' lvalue lvalue_list { $$ = 1 + $3; } ;

index_lvalue: lvalue
	    | string
            | expr42;

lvalue: F_IDENTIFIER
	{
	    int i = verify_declared($1);
	    last_push_identifier = PC;
	    ins_f_byte(F_PUSH_IDENTIFIER_LVALUE);
	    ins_byte(i);
	    free($1);
	    if (i == -1)
		$$ = TYPE_ANY;
	    else
		$$ = VARIABLE(i)->type & TYPE_MOD_MASK;
	}
        | F_LOCAL_NAME
	{
	    last_push_local = PC;
	    ins_f_byte(F_PUSH_LOCAL_VARIABLE_LVALUE);
	    ins_byte($1);
	    $$ = type_of_locals[$1];
	}
/*	| expr4 '[' comma_expr F_RANGE comma_expr ']' 
	| expr_index comma_expr F_RANGE comma_expr ']' */
	| index_lvalue '[' comma_expr F_RANGE comma_expr ']'
	  {
	      last_push_range = PC;
	      ins_f_byte(F_PUSH_RANGE_LVALUE);
	      if (exact_types) {
		  if (($1 & TYPE_MOD_POINTER) == 0 && !TYPE($1, TYPE_STRING))
		      type_error("Bad type to indexed value", $1);
		  if (!TYPE($3, TYPE_NUMBER))
		      type_error("Bad type of index", $3);
		  if (!TYPE($5, TYPE_NUMBER))
		      type_error("Bad type of index", $5);
	      }
	      if ($1 == TYPE_ANY)
		  $$ = TYPE_ANY;
	      else if (TYPE($1, TYPE_STRING))
		  $$ = TYPE_STRING;
	      else if ($1 & TYPE_MOD_POINTER)
		  $$ = $1;
	      else if (exact_types)
		  type_error("Bad type of argument used for range", $1);
	  };
/*	| expr4 '[' comma_expr ']' 
	| expr_index comma_expr ']' */
	| index_lvalue '[' comma_expr ']'
	  {
	      last_push_index = PC;
	      ins_f_byte(F_PUSH_INDEXED_LVALUE);
	      if (exact_types  && !($1 & TYPE_MAPPING)) {
		  if (($1 & TYPE_MOD_POINTER) == 0 && !TYPE($1, TYPE_STRING))
		      type_error("Bad type to indexed value", $1);
		  if (!TYPE($3, TYPE_NUMBER))
		      type_error("Bad type of index", $3);
	      }
	      if ($1 == TYPE_ANY)
		  $$ = TYPE_ANY;
	      else if (TYPE($1, TYPE_STRING))
		  $$ = TYPE_NUMBER;
	      else if ($1 == TYPE_MAPPING)
		  $$ = TYPE_ANY;
	      else
		  $$ = $1 & TYPE_MOD_MASK & ~TYPE_MOD_POINTER;
	  };

string: string_lit1
	{
	  ins_string($1);
	  free($1);
	  $$ = TYPE_STRING;
	}
      | string_lit3
	{
	  ins_string($1);
	  free($1);
	  $$ = TYPE_STRING;
	}

string_plus: string_lit2
	{
	  ins_string($1);
	  free($1);
	  $$ = TYPE_STRING;
	}

string_lit1: F_STRING

string_lit2: string_lit1 '+'
           | string_lit3 '+'

string_lit3: string_lit2 F_STRING
	{

	    /* fprintf(stderr, "string_lit3: string_lit2 F_STRING\n    -- $1 = '%s', $2 = '%s'\n", $1, $2); */

	    $$ = xalloc( strlen($1) + strlen($2) + 1 );
	    strcpy($$, $1);
	    strcat($$, $2);

	    /* fprintf(stderr, "    Concatenated to '%s'\n", $$); */

	    free($1);
	    free($2);
	};

string_constant: string_con1
        {
            char *p = make_shared_string($1);

            free($1);
            $$ = p;
        };

string_con1: F_STRING
	   | string_con1 '+' F_STRING
	{

	    /* fprintf(stderr, "string_con1: string_con1 '+' F_STRING\n    -- $1 = '%s', $3 = '%s'\n", $1, $3); */

	    $$ = xalloc( strlen($1) + strlen($3) + 1 );
	    strcpy($$, $1);
	    strcat($$, $3);

	    /* fprintf(stderr, "    Concatenated to '%s'\n", $$); */

	    free($1);
	    free($3);
	};


/*expr_index: lvalue '['
	| expr42 '['
*//*	expr4 '[' *//*
        | string '['*/

expr_arrow: expr4 F_ARROW
        | string F_ARROW


function_call: function_name
    {
	/* This seems to be an ordinary function call. But, if the function
	 * is not defined, then it might be a call to a simul_efun.
	 * If it is, then we make it a call_other(), which requires the
	 * function name as argument.
	 * We have to remember until after parsing the arguments if it was
	 * a simulated efun or not, which means that the pointer has to be
	 * pushed on a stack. Use the internal yacc stack for this purpose.
	 */
	$<funp>$ = 0;
	if (defined_function($1) == -1) {
	    char *p = make_shared_string($1);
	    $<funp>$ = find_simul_efun(p);
	    if ($<funp>$ && !($<funp>$->type & TYPE_MOD_STATIC)) {
	      ins_string(query_simul_efun_file_name());
	      ins_string(p);
	    } else {
		$<funp>$ = 0;
	    }
	    free_string(p);
	}
    }
	'(' expr_list ')'
    { 
	int f;
	int efun_override = strncmp($1, "efun::", 6) == 0;

	if ($<funp>2) {
	    ins_f_byte(F_CALL_OTHER);
	    ins_byte($4 + 2);
	    $$ = $<funp>2->type;
	} else if (!efun_override && (f = defined_function($1)) >= 0) {
	    struct function *funp;
	    ins_f_byte(F_CALL_FUNCTION_BY_ADDRESS);
            ins_short(f);
	    ins_byte($4);	/* Actual number of arguments */
	    funp = FUNCTION(f);
	    if (funp->flags & NAME_UNDEFINED)
		find_inherited(funp);
	    /*
	     * Verify that the function has been defined already.
	     */
	    if ((funp->flags & NAME_UNDEFINED) &&
		!(funp->flags & NAME_PROTOTYPE) && exact_types)
	    {
		char buff[100];
		sprintf(buff, "Function %.50s undefined", funp->name);
		yyerror(buff);
	    }
	    $$ = funp->type & TYPE_MOD_MASK;
	    /*
	     * Check number of arguments.
	     */
	    if (funp->num_arg != $4 && !(funp->type & TYPE_MOD_VARARGS) &&
		(funp->flags & NAME_STRICT_TYPES) && exact_types)
	    {
		char buff[100];
		sprintf(buff, "Wrong number of arguments to %.60s", $1);
		yyerror(buff);
	    }
	    /*
	     * Check the argument types.
	     */
	    if (exact_types && *(unsigned short *)&mem_block[A_ARGUMENT_INDEX].block[f * sizeof (unsigned short)] != INDEX_START_NONE)
	    {
		int i, first;
		unsigned short *arg_types;
		
		arg_types = (unsigned short *)
		    mem_block[A_ARGUMENT_TYPES].block;
		first = *(unsigned short *)&mem_block[A_ARGUMENT_INDEX].block[f * sizeof (unsigned short)];
		for (i=0; i < funp->num_arg && i < $4; i++) {
		    int tmp = get_argument_type(i, $4);
		    if (!TYPE(tmp, arg_types[first + i])) {
			char buff[100];
			sprintf(buff, "Bad type for argument %d to %s %s",
				i+1, $1,
				get_two_types(arg_types[first+i], tmp));
			yyerror(buff);
		    }
		}
	    }
	} else if (efun_override || (f = lookup_predef($1)) != -1) {
	    int min, max, def, *argp;
	    extern int efun_arg_types[];

	    if (efun_override) {
		f = lookup_predef($1+6);
	    }
	    if (f == -1) {	/* Only possible for efun_override */
		char buff[100];
		sprintf(buff, "Unknown efun: %s", $1+6);
		yyerror(buff);
	    } else {
		min = instrs[f-F_OFFSET].min_arg;
		max = instrs[f-F_OFFSET].max_arg;
		def = instrs[f-F_OFFSET].Default;
		$$ = instrs[f-F_OFFSET].ret_type;
		argp = &efun_arg_types[instrs[f-F_OFFSET].arg_index];
		if (def && $4 == min-1) {
		    ins_f_byte(def);
		    max--;
		    min--;
		} else if ($4 < min) {
		    char bff[100];
		    sprintf(bff, "Too few arguments to %s",
			    instrs[f-F_OFFSET].name);
		    yyerror(bff);
		} else if ($4 > max && max != -1) {
		    char bff[100];
		    sprintf(bff, "Too many arguments to %s",
			    instrs[f-F_OFFSET].name);
		    yyerror(bff);
		} else if (max != -1 && exact_types) {
		    /*
		     * Now check all types of the arguments to efuns.
		     */
		    int i, argn;
		    char buff[100];
		    for (argn=0; argn < $4; argn++) {
			int tmp = get_argument_type(argn, $4);
			for(i=0; !TYPE(argp[i], tmp) && argp[i] != 0; i++)
			    ;
			if (argp[i] == 0) {
			    sprintf(buff, "Bad argument %d type to efun %s()",
				    argn+1, instrs[f-F_OFFSET].name);
			    yyerror(buff);
			}
			while(argp[i] != 0)
			    i++;
			argp += i + 1;
		    }
		}
		ins_f_byte(f);
		/* Only store number of arguments for instructions
		 * that allowed a variable number.
		 */
		if (max != min)
		    ins_byte($4);/* Number of actual arguments */
	    }
	} else {
	    struct function *funp;

	    f = define_new_function($1, 0, 0, 0, NAME_UNDEFINED, 0);
	    ins_f_byte(F_CALL_FUNCTION_BY_ADDRESS);
	    ins_short(f);
	    ins_byte($4);	/* Number of actual arguments */
	    funp = FUNCTION(f);
	    if (strchr($1, ':')) {
		/*
		 * A function defined by inheritance. Find
		 * real definition immediately.
		 */
		find_inherited(funp);
	    }
	    /*
	     * Check if this function has been defined.
	     * But, don't complain yet about functions defined
	     * by inheritance.
	     */
	    if (exact_types && (funp->flags & NAME_UNDEFINED)) {
		char buff[100];
		sprintf(buff, "Undefined function %.50s", $1);
		yyerror(buff);
	    }
	    if (!(funp->flags & NAME_UNDEFINED))
		$$ = funp->type;
	    else
		$$ = TYPE_ANY;	/* Just a guess */
	}
	free($1);
	pop_arg_stack($4);	/* Argument types not needed more */
    }
    | expr_arrow function_name
    {
      ins_string($2);
	free($2);
    }
    '(' expr_list ')'
    {
	ins_f_byte(F_CALL_OTHER);
	ins_byte($5 + 2);
	$$ = TYPE_ANY;
	pop_arg_stack($5);	/* No good need of these arguments */
    };

function_name: F_IDENTIFIER
	     | F_COLON_COLON F_IDENTIFIER
		{
		    char *p = xalloc(strlen($2) + 3);
		    strcpy(p, "::"); strcat(p, $2); free($2);
		    $$ = p;
		}
	      | F_IDENTIFIER F_COLON_COLON F_IDENTIFIER
		{
		    char *p = xalloc(strlen($1) + strlen($3) + 3);
		    strcpy(p, $1); strcat(p, "::"); strcat(p, $3);
		    free($1); free($3);
		    $$ = p;
		};

cond: F_IF
    '(' comma_expr ')'
    {
      code_generation=1;
      ins_f_byte(F_JUMP_WHEN_ZERO);
      push_address();
      ins_jump(0);
    } 
    statement
    optional_else_part
    {
      upd_jump(pop_address(), do_label());
    } ;

optional_else_part: /* empty */ { bytes_optimized+=3; }
       | F_ELSE
       {
	 int i;
	 i = pop_address();
	 ins_f_byte(F_JUMP);
	 push_address();
	 ins_jump(0);
	 upd_jump(i, do_label());
       }
       statement
       ;
%%

void yyerror(str)
char *str;
{
    extern int num_parse_error;

    if (num_parse_error > 5)
	return;
    (void)fprintf(stderr, "%s: %s line %d\n", current_file, str,
		  current_line);
    fflush(stderr);
    smart_log(current_file, current_line, str);
    if (num_parse_error == 0)
	save_error(str, current_file, current_line);
    num_parse_error++;
}

static int check_declared(str)
    char *str;
{
    struct variable *vp;
    int offset;
    char * interned;
    extern char* findstring PROT((char*));

    if (interned = findstring(str)) /* Only search if amongst strings */
	for (offset=0;
	     offset < mem_block[A_VARIABLES].current_size;
	     offset += sizeof (struct variable)) {
	    vp = (struct variable *)&mem_block[A_VARIABLES].block[offset];
	    if (vp->flags & NAME_HIDDEN)
		continue;
	    /* Pointer comparison is possible since we use unique strings */
	    if (vp->name == interned)
		return offset / sizeof (struct variable);
	}
    return -1;
}

static int verify_declared(str)
    char *str;
{
    int r;

    r = check_declared(str);
    if (r < 0) {
	char buff[100];
        (void)sprintf(buff, "Variable %s not declared !", str);
        yyerror(buff);
	return -1;
    }
    return r;
}

void free_all_local_names()
{
    int i;

    for (i=0; i<current_number_of_locals; i++) {
	free(local_names[i]);
	local_names[i] = 0;
    }
    current_number_of_locals = 0;
}

void add_local_name(str, type)
    char *str;
    int type;
{
    if (current_number_of_locals == MAX_LOCAL)
	yyerror("Too many local variables");
    else {
	type_of_locals[current_number_of_locals] = type;
	local_names[current_number_of_locals++] = str;
    }
}

/*
 * Copy all function definitions from an inherited object. They are added
 * as undefined, so that they can be redefined by a local definition.
 * If they are not redefined, then they will be updated, so that they
 * point to the inherited definition. See epilog(). Types will be copied
 * at that moment (if available).
 *
 * A call to an inherited function will not be
 * done through this entry (because this entry can be replaced by a new
 * definition). If an function defined by inheritance is called, then one
 * special definition will be made at first call.
 */
static int copy_functions(from, type)
    struct program *from;
    int type;
{
    int i, initializer = -1;
    unsigned short tmp_short;

    for (i=0; i < from->num_functions; i++) {
	/* Do not call define_new_function() from here, as duplicates would
	 * be removed.
	 */
	struct function fun;
	int new_type;

	fun = from->functions[i];	/* Make a copy */
	/* Prepare some data to be used if this function will not be
	 * redefined.
	 */
	if (strchr(fun.name, ':'))
	    fun.flags |= NAME_HIDDEN;	/* Not to be used again ! */
	fun.name = make_shared_string(fun.name);	/* Incr ref count */
	fun.inherit_offset = mem_block[A_INHERITS].current_size /
	    sizeof (struct inherit) - 1;
	fun.offset = i;
	if (fun.type & TYPE_MOD_NO_MASK) {
	    int n;
	    if ((n = defined_function(fun.name)) != -1 &&
		!(((struct function *)mem_block[A_FUNCTIONS].block)[n].flags &
		  NAME_UNDEFINED))
	    {
		char *p = (char *)alloca(80 + strlen(fun.name));
		sprintf(p, "Illegal to redefine 'nomask' function \"%s\"",
			fun.name);
		yyerror(p);
	    }
	    fun.flags |= NAME_INHERITED;
	} else if (!(fun.flags & NAME_HIDDEN)) {
	    fun.flags |= NAME_UNDEFINED;
	}
	/*
	 * public functions should not become private when inherited
	 * 'private'
	 */
	new_type = type;
	if (fun.type & TYPE_MOD_PUBLIC)
	    new_type &= ~TYPE_MOD_PRIVATE;
	fun.type |= new_type;
	/* marion
	 * this should make possible to inherit a heart beat function, and
	 * thus to mask it if wanted.
	 */
	if (heart_beat == -1 && fun.name[0] == 'h' &&
	    strcmp(fun.name, "heart_beat") == 0)
	{
	    heart_beat = mem_block[A_FUNCTIONS].current_size /
		sizeof (struct function);
	} else if (initializer == -1 && /* Profezzorn */
		   fun.name[0] == '_' && strcmp(fun.name, "__INIT") == 0) {
	    initializer = i;
	    fun.flags |= NAME_INHERITED;
	}
	add_to_mem_block(A_FUNCTIONS, (char *)&fun, sizeof fun);
	/*
	 * Copy information about the types of the arguments, if it is
	 * available.
	 */
	tmp_short = INDEX_START_NONE;	/* Presume not available. */
	if (from->type_start != 0 && from->type_start[i] != INDEX_START_NONE)
	{
	    int arg;
	    /*
	     * They are available for function number 'i'. Copy types of
	     * all arguments, and remember where they started.
	     */
	    tmp_short = mem_block[A_ARGUMENT_TYPES].current_size /
		sizeof from->argument_types[0];
	    for (arg = 0; arg < fun.num_arg; arg++) {
		add_to_mem_block(A_ARGUMENT_TYPES,
				 &from->argument_types[from->type_start[i] + arg],
				 sizeof (unsigned short));
	    }
	}
	/*
	 * Save the index where they started. Every function will have an
	 * index where the type info of arguments starts.
	 */
	add_to_mem_block(A_ARGUMENT_INDEX, &tmp_short, sizeof tmp_short);
    }
    return initializer;
}

/*
 * Copy all variabel names from the object that is inherited from.
 * It is very important that they are stored in the same order with the
 * same index.
 */
static void copy_variables(from, type)
    struct program *from;
    int type;
{
    int i;

    for (i=0; i<from->num_variables; i++) {
	int new_type = type;
	int n = check_declared(from->variable_names[i].name);

	if (n != -1 && (VARIABLE(n)->type & TYPE_MOD_NO_MASK)) {
	    char *p = (char *)alloca(80 +
				     strlen(from->variable_names[i].name));
	    sprintf(p, "Illegal to redefine 'nomask' variable \"%s\"",
		    VARIABLE(n)->name);
	    yyerror(p);
	}
	/*
	 * 'public' variables should not become private when inherited
	 * 'private'.
	 */
	if (from->variable_names[i].type & TYPE_MOD_PUBLIC)
	    new_type &= ~TYPE_MOD_PRIVATE;
	define_variable(from->variable_names[i].name,
			from->variable_names[i].type | new_type,
			from->variable_names[i].type & TYPE_MOD_PRIVATE ?
			    NAME_HIDDEN : 0);
    }
}

/*
 * This function is called from lex.c for every new line read from the
 * "top" file (means not included files). Some new lines are missed,
 * as with #include statements, so it is compensated for.
 */
#ifdef SHORT_LINE_NUMBERS
static int old_current_line;
static int old_offset;
#endif

void store_line_number_info()
{
  int offset = PC;

#ifdef SHORT_LINE_NUMBERS
  while(old_current_line <current_line)
  {
    int delta;
    unsigned char p;
    delta=offset-old_offset;
    if(delta<0) delta=0;
    if(delta < 255)
    {
      p=delta;
      add_to_mem_block(A_LINENUMBERS, (char *)&p,1);
    }else{
      p=255;
      add_to_mem_block(A_LINENUMBERS, (char *)&p,1);
    }
    old_offset+=delta;
    old_current_line++;
  }
#else
    while(mem_block[A_LINENUMBERS].current_size / sizeof (short) <
	  current_line)
    {
	add_to_mem_block(A_LINENUMBERS, (char *)&offset, sizeof offset);
    }
#endif
}

static char *get_type_name(type)
    int type;
{
    static char buff[100];
    static char *type_name[] = { "unknown", "int", "string", "void",
				   "object", "mixed", "mapping", "float" };
    int pointer = 0;

    buff[0] = 0;
    if (type & TYPE_MOD_STATIC)
	strcat(buff, "static ");
    if (type & TYPE_MOD_NO_MASK)
	strcat(buff, "nomask ");
    if (type & TYPE_MOD_PRIVATE)
	strcat(buff, "private ");
    if (type & TYPE_MOD_PROTECTED)
	strcat(buff, "protected ");
    if (type & TYPE_MOD_PUBLIC)
	strcat(buff, "public ");
    if (type & TYPE_MOD_VARARGS)
	strcat(buff, "varargs ");
    type &= TYPE_MOD_MASK;
    if (type & TYPE_MOD_POINTER) {
	pointer = 1;
	type &= ~TYPE_MOD_POINTER;
    }
    if (type >= sizeof type_name / sizeof type_name[0])
	fatal("Bad type: %d\n", type);
    strcat(buff, type_name[type]);
    strcat(buff," ");
    if (pointer)
	strcat(buff, "* ");
    return buff;
}

void type_error(str, type)
    char *str;
    int type;
{
    static char buff[100];
    char *p;
    p = get_type_name(type);
    if (strlen(str) + strlen(p) + 5 >= sizeof buff) {
	yyerror(str);
    } else {
	strcpy(buff, str);
	strcat(buff, ": \"");
	strcat(buff, p);
	strcat(buff, "\"");
	yyerror(buff);
    }
}

/*
 * Compile an LPC file.
 */
void compile_file() {
    int yyparse();

    prolog();
    yyparse();
    epilog();
}

static char *get_two_types(type1, type2)
    int type1, type2;
{
    static char buff[100];

    strcpy(buff, "( ");
    strcat(buff, get_type_name(type1));
    strcat(buff, "vs ");
    strcat(buff, get_type_name(type2));
    strcat(buff, ")");
    return buff;
}

/*
 * The program has been compiled. Prepare a 'struct program' to be returned.
 */
void epilog() {
    int size, i;
    char *p;
    struct function *funp;
    static int current_id_number = 1;

#ifdef DEBUG
    if (num_parse_error == 0 && type_of_arguments.current_size != 0)
	fatal("Failed to deallocate argument type stack\n");
#endif

#ifndef NO_OVERLAP_CHECK
    if(top_case != NULL)
    {
       struct case_overlap_check *c, *n;
       for(c = top_case; c; c = n)
       {
	  n = c->next;
	  free(c);
       }
       top_case = NULL;
    }
#endif

    /*
     * Define the __INIT function, but only if there was any code
     * to initialize.
     */
    if (first_last_initializer_end != last_initializer_end) {
	define_new_function("__INIT", 0, 0, 0, 0, 0);
	/*
	 * Change the last jump after the last initializer into a
	 * return(1) statement.
	 */
	mem_block[A_PROGRAM].block[last_initializer_end-1] =
	    F_RETURN_ONE - F_OFFSET;
    }

    /*
     * If functions are undefined, replace them by definitions done
     * by inheritance. All explicit "name::func" are already resolved.
     */
    for (i = 0; i < mem_block[A_FUNCTIONS].current_size; i += sizeof *funp) {
	funp = (struct function *)(mem_block[A_FUNCTIONS].block + i);
	if (!(funp->flags & NAME_UNDEFINED))
	    continue;
	find_inherited(funp);
    }

    if (num_parse_error > 0)
    {
#if 1
      int grogs;
      char **grog;
      struct function *grfun;
      struct variable *grvar; 

      while(saved_code_stack)
	pop_code_block();
      
      /* we gotta free some references here as well */
      grog=(char **)(mem_block[A_STRINGS].block);
      grogs=mem_block[A_STRINGS].current_size/sizeof(char *);
      for(i=0;i<grogs;i++) free_string(grog[i]);

      grfun=(struct function *)(mem_block[A_FUNCTIONS].block);
      grogs=mem_block[A_FUNCTIONS].current_size/sizeof(struct function);
      for(i=0;i<grogs;i++) free_string(grfun[i].name);

      grvar=(struct variable *)(mem_block[A_VARIABLES].block);
      grogs=mem_block[A_VARIABLES].current_size/sizeof(struct variable);
      for(i=0;i<grogs;i++) free_string(grvar[i].name);
#endif
      prog = 0;

      for (i=0; i<NUMAREAS; i++)
      {
	free(mem_block[i].block);
	mem_block[i].current_size=0;
      }
      return;
    }
    size = align(sizeof(struct program));
#ifdef DO_SAVE_ARG_TYPES
    if(pragma_save_types)
    {
       for(i = 0; i < NUMPAREAS; i++)
	  size += align(mem_block[i].current_size);
    }
    else
    {
       for(i = 0; i < NUMSAVEAREAS; i++)
	  size += align(mem_block[i].current_size);
    }
#else
#if 0
    for(i = 0; i < NUMPAREAS; i++)
       size += align(mem_block[i].current_size);
#else
    for(i = 0; i < NUMSAVEAREAS; i++)
       size += align(mem_block[i].current_size);
#endif
#endif
    p = (char *)xalloc(size);
    prog = (struct program *)p;
    *prog = NULL_program;
    prog->total_size = size;
    prog->ref = 0;
    prog->heart_beat = heart_beat;
    prog->name = make_shared_string(current_file);
    prog->id_number = current_id_number++;
    total_prog_block_size += prog->total_size;
    total_num_prog_blocks += 1;

    p += align(sizeof (struct program));
    prog->program = p;
    if (mem_block[A_PROGRAM].current_size)
	memcpy(p, mem_block[A_PROGRAM].block,
	       mem_block[A_PROGRAM].current_size);
    prog->program_size = mem_block[A_PROGRAM].current_size;

    p += align(mem_block[A_PROGRAM].current_size);
#ifdef SHORT_LINE_NUMBERS
    prog->line_numbers = (unsigned char *)p;
#else
    prog->line_numbers = (unsigned short *)p;
#endif
    if (mem_block[A_LINENUMBERS].current_size)
	memcpy(p, mem_block[A_LINENUMBERS].block,
	       mem_block[A_LINENUMBERS].current_size);

    p += align(mem_block[A_LINENUMBERS].current_size);
    prog->functions = (struct function *)p;
    prog->num_functions = mem_block[A_FUNCTIONS].current_size /
	sizeof (struct function);
    if (mem_block[A_FUNCTIONS].current_size)
	memcpy(p, mem_block[A_FUNCTIONS].block,
	       mem_block[A_FUNCTIONS].current_size);

    p += align(mem_block[A_FUNCTIONS].current_size);
    prog->strings = (char **)p;
    prog->num_strings = mem_block[A_STRINGS].current_size /
	sizeof (char *);
    if (mem_block[A_STRINGS].current_size)
	memcpy(p, mem_block[A_STRINGS].block,
	       mem_block[A_STRINGS].current_size);

    p += align(mem_block[A_STRINGS].current_size);
    prog->variable_names = (struct variable *)p;
    prog->num_variables = mem_block[A_VARIABLES].current_size /
	sizeof (struct variable);
    if (mem_block[A_VARIABLES].current_size)
	memcpy(p, mem_block[A_VARIABLES].block,
	       mem_block[A_VARIABLES].current_size);

    p += align(mem_block[A_VARIABLES].current_size);
    prog->num_inherited = mem_block[A_INHERITS].current_size /
	sizeof (struct inherit);
    if (prog->num_inherited) {
	prog->inherit = (struct inherit *)p;
	memcpy(p, mem_block[A_INHERITS].block,
	       mem_block[A_INHERITS].current_size);
	p += align(mem_block[A_INHERITS].current_size);
    } else
	prog->inherit = 0;

#ifdef DO_SAVE_ARG_TYPES  /* This could really blow up :) */
    if(pragma_save_types &&
       mem_block[A_ARGUMENT_TYPES].current_size / sizeof(unsigned short) &&
       mem_block[A_ARGUMENT_INDEX].current_size / sizeof(unsigned short))
    {
       prog->argument_types = (unsigned short *) p;
       memcpy(p, mem_block[A_ARGUMENT_TYPES].block,
	      mem_block[A_ARGUMENT_TYPES].current_size);
       p += align(mem_block[A_ARGUMENT_TYPES].current_size);
       prog->type_start = (unsigned short *) p;
       memcpy(p, mem_block[A_ARGUMENT_INDEX].block,
	      mem_block[A_ARGUMENT_INDEX].current_size);
       p += align(mem_block[A_ARGUMENT_INDEX].current_size);
    }
    else
    {
       prog->argument_types = 0;
       prog->type_start = 0;
    }
#else
    prog->argument_types = 0;	/* For now. Will be fixed someday */
    prog->type_start = 0;
#endif

#ifndef NO_PROGRAM_STATUS
    progstruct_bytes += sizeof(struct program);
    byte_code_bytes += prog->program_size;
    function_bytes += prog->num_functions * sizeof(struct function);
    strings_bytes += prog->num_strings * sizeof(char *);
    variables_bytes += prog->num_variables * sizeof(struct variable);
    line_number_bytes += align(mem_block[A_LINENUMBERS].current_size);
    inherits_bytes += prog->num_inherited * sizeof(struct inherit);
#ifdef DO_SAVE_ARG_TYPES
    if(pragma_save_types)
    {
       arg_types_bytes += align(mem_block[A_ARGUMENT_TYPES].current_size);
       arg_index_bytes += align(mem_block[A_ARGUMENT_INDEX].current_size);
    }
    else
    {
       arg_bytes_won += align(mem_block[A_ARGUMENT_TYPES].current_size) +
	  align(mem_block[A_ARGUMENT_INDEX].current_size);
    }
#else
    arg_bytes_won += align(mem_block[A_ARGUMENT_TYPES].current_size) +
       align(mem_block[A_ARGUMENT_INDEX].current_size);
#endif
#endif

    for(i = 0; i < NUMAREAS; i++)
       free((char *) mem_block[i].block);

    /*  marion
	Do referencing here - avoid multiple referencing when an object
	inherits more than one object and one of the inherited is already
	loaded and not the last inherited
    */
    reference_prog (prog, "epilog");
    for (i = 0; i < prog->num_inherited; i++) {
	reference_prog (prog->inherit[i].prog, "inheritance");
    }
}

/*
 * Initialize the environment that the compiler needs.
 */
static void prolog() {
    int i;

#ifdef SHORT_LINE_NUMBERS
    old_offset=0;
    old_current_line=1;
#endif

    if (type_of_arguments.block == 0) {
	type_of_arguments.max_size = 100;
	type_of_arguments.block = xalloc(type_of_arguments.max_size);
    }
    type_of_arguments.current_size = 0;
    approved_object = 0;
    last_push_local = -1;
    last_push_identifier = -1;
    prog = 0;		/* 0 means fail to load. */
    heart_beat = -1;
    comp_stackp = 0;	/* Local temp stack used by compiler */
    current_continue_address = 0;
    current_break_address = 0;
    current_switch=0;
    num_parse_error = 0;
    free_all_local_names();	/* In case of earlier error */
    /* Initialize memory blocks where the result of the compilation
     * will be stored.
     */
    for (i=0; i < NUMAREAS; i++) {
	mem_block[i].block = xalloc(START_BLOCK_SIZE);
	mem_block[i].current_size = 0;
	mem_block[i].max_size = START_BLOCK_SIZE;
    }
    do_label();
    add_new_init_jump();
    first_last_initializer_end = last_initializer_end;
}

/*
 * Add a trailing jump after the last initialization code.
 */
void add_new_init_jump() {
    /*
     * Add a new jump.
     */
    ins_f_byte(F_JUMP);
    last_initializer_end = PC;
    ins_jump(0);
}
