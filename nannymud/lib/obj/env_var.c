

#define ERROR  -1
#define FALSE   0
#define TRUE    1
#define NULL    0

#define OBJECT(XXX)  (XXX[0])
#define FUNC(XXX)    (XXX[1])



private        mapping variables  = ([ /* empty */ ]);
private static mapping call_backs = ([ /* empty */ ]);


/*
 * Function name: getenv.
 * Description:   Finds the value for a variable.
 * Arguments:     A variable name.
 * Returns:       The value of the variable. 0 if the variable didn't exist.
 */

string getenv(string var)  { return variables[var]; }

/*
 * Function name: setenv.
 * Description:   Bind a value to a variable.
 * Arguments:     A variable and a value.
 * Returns:       1 if it the value was changed 0 if it was a new variable.
 *                -1 on error.
 */

int setenv(string var, string value) 
{
  string  old_value;
  mixed  *call_funcs;
  int     i;

  if(!var)
    return ERROR;

  old_value = variables[var];

  variables[var] = value;
  
  if(call_funcs = call_backs[var])
  {
    for(i = 0; i < sizeof(call_funcs); i++)
    {
      if(OBJECT(call_funcs[i]) == NULL)
	continue;
      call_other(OBJECT(call_funcs[i]), FUNC(call_funcs[i]), var, value);
    }
  }

  return (old_value ? 1 : 0);
}
      
/*
 * Function name: unsetenv.
 * Description:   Removes a variable from the variable array.
 * Arguments:     A variable
 * Returns:       1 of sucessful.
 */

int unsetenv(string var) 
{
  mixed  *call_funcs;
  int     i;

  if(!var || variables[var] == 0)
    return 0;

  variables = m_delete(variables, var);

  if(call_funcs = call_backs[var])
  {
    for(i = 0; i < sizeof(call_funcs); i++)
    {
      if(OBJECT(call_funcs[i]) == NULL)
	continue;
      call_other(OBJECT(call_funcs[i]), FUNC(call_funcs[i]), var, 0);
    }
  }
  
  return 1;
}

    
/*
 * Function name: printenv
 * Description:   Prints all variables and their values.
 * Returns:       1 if there is any variables 0 otherwise.
 */

int printenv() 
{
  string  *vars;
  int      i;

  if(m_sizeof(variables) == 0)
    return 0;
  
  vars = m_indices(variables);

  for(i = 0; i < sizeof(vars); i++) 
    if(stringp(variables[vars[i]]) && stringp(vars[i]))
      write(sprintf("%s = %s\n",vars[i] , variables[vars[i]]));
  return 1;
}
      
string *envv() { return m_indices(variables); }
  
/*
 * Function name: add_call_back
 * Description:   Add a call back function for a variable. This function will 
 *                be called if the value of the variable is  changed.
 * Arguments:     A variable, an object and a function.
 * Returns:       1 on success otherwise 0.
 */

int add_call_back(string var, mixed ob, string func)
{
  mixed  *old_call_funcs;
  
  if(!var || !ob || !func || stringp(var) == FALSE  || 
     (stringp(ob) == FALSE && objectp(ob) == FALSE) ||
     stringp(func) == FALSE)
    return 0;

  old_call_funcs = call_backs[var];

  if(old_call_funcs)
    call_backs[var] = old_call_funcs + ({ ({ ob, func }) });
  else
    call_backs[var] = ({ ({ ob, func }) });

  return 1;
}

  
  
  
