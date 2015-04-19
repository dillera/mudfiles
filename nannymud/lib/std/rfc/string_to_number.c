/* Written by Profezzorn (probably not finished) */

string skipwhite(string s)
{
  int e;
  for(e=0;e<strlen(s);e++)
  {
    switch(s[e])
    {
    case ' ':
    case '\t':
    case '\n':
    case '-':
      break;

    case 'a':
      if(s[e..e+2]=="and")
      {
	e+=2;
	break;
      }
      
    default:
      return s[e..0x7fffffff];
    }
  }
  return "";
}

mixed *identify_next_number(string num,int base)
{
  switch(strlen(num=skipwhite(num)))
  {
  default:
    switch(num[0..4])
    {
    case "three": return ({base+3,num[5..1000]});
    case "seven": return ({base+7,num[5..1000]});
    case "eight": return ({base+8,num[5..1000]});
    }
  case 4:
    switch(num[0..3])
    {
    case "four": return ({base+4,num[4..1000]});
    case "nine": return ({base+9,num[4..1000]});
    case "five": return ({base+5,num[4..1000]});
    }
  case 3:
    switch(num[0..2])
    {
    case "one": return ({base+1,num[3..1000]});
    case "two": return ({base+2,num[3..1000]});
    case "six": return ({base+6,num[3..1000]});
    }
  case 2:
  case 1:
  case 0:
  }
  return ({base,num});
}

mixed *identify_one_number(string num)
{
  mixed res;
  num=skipwhite(num);
  if(sscanf(num,"%d%s",num,res)==2)
    return ({num,res});

  switch(strlen(num))
  {
  default:
    switch(num[0..8])
    {
    case "seventeen": return ({17,num[9..1000]});
    case "eightteen": return ({18,num[9..1000]});
    }
  case 8:
    switch(num[0..7])
    {
    case "thirteen": return ({13,num[8..1000]});
    case "fourteen": return ({14,num[8..1000]});
    case "nineteen": return ({19,num[8..1000]});
    } 
  case 7:
    switch(num[0..6])
    {
    case "fifteen": return ({15,num[7..1000]});
    case "sixteen": return ({16,num[7..1000]});
    case "seventy": return identify_next_number(num[7..1000],70);
    }
  case 6:
    switch(num[0..5])
    {
    case "eleven": return ({11,num[6..1000]});
    case "twelve": return ({12,num[6..1000]});
    case "twenty": return identify_next_number(num[6..1000],20);
    case "eighty": return identify_next_number(num[6..1000],80);
    case "ninety": return identify_next_number(num[6..1000],90);
    case "thirty": return identify_next_number(num[6..1000],30);
    case "fourty": return identify_next_number(num[6..1000],40);
    }
  case 5:
    switch(num[0..4])
    {
    case "three": return ({3,num[5..1000]});
    case "seven": return ({7,num[5..1000]});
    case "eight": return ({8,num[5..1000]});
    case "fifty": return identify_next_number(num[5..1000],50);
    case "sixty": return identify_next_number(num[5..1000],60);
    }
  case 4:
    switch(num[0..3])
    {
    case "zero": return ({0,num[4..1000]});
    case "four": return ({4,num[4..1000]});
    case "five": return ({5,num[4..1000]});
    case "nine": return ({9,num[4..1000]});
    }
  case 3:
    switch(num[0..2])
    {
    case "one": return ({1,num[3..1000]});
    case "two": return ({2,num[3..1000]});
    case "ten": return ({10,num[3..1000]});
    case "six": return ({6,num[3..1000]});
    }
  case 2:
  case 1:
  case 0:
  }
  return 0;
}

mixed *identify_number_below_thousand(string num)
{
  string a,b;
  mixed *ret;
  int result;

  if(sscanf(num,"%shundred%s",a,b))
  {
    sscanf(b,"s%s",b);
    ret=identify_next_number(a,0);
    if(strlen(ret[1])) return 0;
    result=100*ret[0];
    ret=identify_one_number(b);
    if(!ret) return ({result,b});
    ret[0]+=result;
    return ret;
  }
  return identify_one_number(num);
}

mixed *string_to_number(string foo)
{
  int result;
  mixed *ret;
  string a;

  foo=skipwhite(lower_case(foo));
  if(sscanf(foo,"a %s",foo) ||
     sscanf(foo,"an %s",foo) ||
     sscanf(foo,"the %s",foo))
    return ({1, foo});
    
  result=0;

  while(1)
  {
    ret=identify_number_below_thousand(foo);
    if(!ret) return 0;
    foo=skipwhite(ret[1]);
    if(sscanf(foo,"million%s",foo))
    {
      sscanf(foo,"s%s",foo);
      result+=ret[0]*1000000;
    }
    else if(sscanf(foo,"thousand%s",foo))
    {
      sscanf(foo,"s%s",foo);
      result+=ret[0]*1000;
    }
    else
    {
      result+=ret[0];
      foo=skipwhite(foo);
      return ({result,foo});
    }
  }
}

