
#include <stdio.h>

#include "config.h"
#include "lint.h"
#include "lang.h"
#include "exec.h"
#include "interpret.h"
#include "object.h"
#include "instrs.h"


int func_arr[1000];
int func_pek;
FILE *fd;


char *get_variable_name( offset, prog)
     unsigned short offset;
     struct program *prog;
{
  if(offset >= prog->num_variables)
    return "----";
  return prog->variable_names[offset].name;
}

dump_functions(funcs, num)
     struct function *funcs;
     int              num;
{
  int i, j, k;

  func_pek = 0;

  fprintf(fd, "%25s %-5s %-5s %-5s %-5s %-5s %-5s\n",
	 "Function", "Offset", "Flag", "Var", "Arg", "Ind", "Type");

  for(i = 0; i < num; i++)
  {

    if((funcs[i].flags & (NAME_INHERITED | NAME_UNDEFINED)) == 0)
    {
      func_arr[func_pek] = i;
      for(j = func_pek; j >= 0; j--)
      {
	if(j > 0 && funcs[func_arr[j]].offset < funcs[func_arr[j-1]].offset)
	{
	  k = func_arr[j];
	  func_arr[j] = func_arr[j-1];
	  func_arr[j-1] = k;
	}
	else
	  break;
      }
      func_pek++;
    }
    
    fprintf(fd, "%-2d %-25s %-5hu %-5hu %-5hu %-5hu %-5hu %-5hu\n", i,
	   funcs[i].name, funcs[i].offset, funcs[i].flags, 
	   funcs[i].num_local, funcs[i].num_arg, 
	   funcs[i].inherit_offset, funcs[i].type);
  }
}

int dump_object(ob, file)
     struct object *ob;
     char *file;
{
  
  char *prog;
  char *pc;
  char *str;
  int   size, j = 0;

  if((fd = fopen("syslog/dump", "w")) == NULL)
    return 0;
  
  pc   = ob->prog->program;
  size = ob->prog->program_size;

  fprintf(fd, "\nProgram size: %d\n\n", size);
  fprintf(fd, "Function Table:\n");
  dump_functions(ob->prog->functions, ob->prog->num_functions);
  fprintf(fd, "\n");

 
  while(pc < ob->prog->program + size)
  {
    int  i;
    unsigned short instr;
    unsigned short offset;

    if(j < func_pek &&
       ob->prog->functions[func_arr[j]].offset == pc-ob->prog->program)
    {
      fprintf(fd, "Function: %s\n", ob->prog->functions[func_arr[j]].name);
      j++;
    }
    
    instr = EXTRACT_UCHAR(pc) + F_OFFSET;

    fprintf(fd, "%d\t%s\t", pc-ob->prog->program, get_f_name((int) instr));

    pc += 1;
    if (instrs[instr -F_OFFSET].min_arg != instrs[instr-F_OFFSET].max_arg) 
      pc += 1;
    

    switch(instr)
    {
    case F_IDENTIFIER:
    case F_PUSH_IDENTIFIER_LVALUE:
      fprintf(fd, "%s", get_variable_name(EXTRACT_UCHAR(pc), ob->prog));
      pc +=1;
      break;

    case F_LOCAL_NAME:
    case F_PUSH_LOCAL_VARIABLE_LVALUE:
      fprintf(fd, "%hu", EXTRACT_UCHAR(pc));
      pc +=1;
      break;
      
    case F_JUMP_WHEN_NON_ZERO:
    case F_JUMP_WHEN_ZERO:
    case F_JUMP:
    case F_AGGREGATE:
    case F_M_AGGREGATE:
    case F_CATCH:
    case F_STRING:
      ((char *)&offset)[0] = pc[0];
      ((char *)&offset)[1] = pc[1];
      pc += 2;
      fprintf(fd, "%hu", offset);
      break;    
      
    case F_CALL_FUNCTION_BY_ADDRESS:
      ((char *)&offset)[0] = pc[0];
      ((char *)&offset)[1] = pc[1];
      pc += 3;
      fprintf(fd, "%s", ob->prog->functions[offset].name);
      break;
      
    case F_NUMBER:
      ((char *)&i)[0] = pc[0];
      ((char *)&i)[1] = pc[1];
      ((char *)&i)[2] = pc[2];
      ((char *)&i)[3] = pc[3];
      pc += 4;
      fprintf(fd, "%d", i);
      break;
      
    case F_PARSE_COMMAND:
    case F_SSCANF:
      pc++;
      break;
    }
    fprintf(fd, "\n");
  }
  
  fprintf(fd, "\nString Table:\n");
  for(j = 0; j < ob->prog->num_strings; j++)
    fprintf(fd, "%d\t%s\n", j, ob->prog->strings[j]);

  fclose(fd);
  return 1;
}



