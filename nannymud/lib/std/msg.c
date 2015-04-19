/* Written by Profezzorn */
/* string fix_msg(string s,mixed *persons,object active)
 *
 * This function does the actuall replacing of tokes agains real names.
 * Input:
 *  s:       The string to do the replacing on.
 *  persons: Contains the persons around ordered as follows:
 *           ({
 *             ({ 1st person(s) }),
 *             ({ 2nd person(s) }),
 *             ({ 3rd person(s) }),
 *           })

 *           No limitations are imposed, you can have any amount of groups
 *           of people, and every group can contain any amount of people.
 *           Having one person in several groups will not make much sense though.
 *  active:  The person who will hear this message. Can be zero to indicate
 *           any person outside the groups.
 */

#pragma strict_types
#pragma save_types

#define DEBUG 0

/* Return 1 if who can see what active is doing,
 * When this function returns zero, Someone is echoed instead of 
 * the name of who
 */
int see_names(object who, object active)
{
  return 1;
}

string fix_msg(string s,mixed *persons,object active)
{
  string *t,*t2,tmp;
  int e,d,p,i;
  object who;
  mixed tmp2,foo;

#if DEBUG>3
  write("fix_msg '"+s+"' "+show_value(persons)+".\n");
#endif

  t=explode("-"+s+"\b","\b");
  t[0]=t[0][1..100000000];
  for(e=1;e<sizeof(t);e++)
  {
    string a,b,c;

    a=t[e];

    if(sscanf(a,"%d%s",p,a)!=2)
      p=0;
    else
      p--;

    switch(a[0])
    {
    case 'c':
      if(sscanf(a,"cap%s",tmp))
      {
	t[e]=capitalize(fix_msg("\b"+(p+1)+tmp,persons,active));
      }
      break;

    case 'p':
      if(member_array(active,persons[p])<0)
      {
	if(sscanf(a,"pron%s",tmp))
	{
	  if(sizeof(persons[p]))
	  {
	    t[e]=persons[p][0]->query_pronoun()+tmp;
	  }
	  else
	  {
	    t[e]="they"+tmp;
	  }
	  break;
	}

	if(sscanf(a,"poss%s",tmp))
	{
	  if(sizeof(persons[p])==1)
	  {
	    t[e]=persons[p][0]->query_possessive()+tmp;
	  }
	  else
	  {
	    t[e]="their"+tmp;
	  }
	  break;
	}
      }

      /* fall through */
    case 'P':
      if(sscanf(a,"PRON%s",tmp) || sscanf(a,"pron%s",tmp))
      {
	tmp2=persons[p]+({});

	for(i=0;i<sizeof(tmp2);i++)
	{
	  who=tmp2[i];

	  if(who==active)
	  {
	    tmp2[i]="you";
	  }else{
	    if(see_names(who,active))
	    {
	      tmp2[i]=(string)who->query_name();
	    }else{
	      tmp2[i]="Someone";
	    }
	  }
	}

	t[e]=implode_nicely(tmp2)+tmp;
	break;
      }

      if(sscanf(a,"POSS%s",tmp) || sscanf(a,"poss%s",tmp))
      {
	tmp2=persons[p]+({});
	
	for(i=0;i<sizeof(tmp2);i++)
	{
	  who=tmp2[i];

	  if(who==active)
	  {
	    if(p && -1!=member_array(who,persons[0]))
	    {
	      tmp2[i]="your own";
	    }else{
	      tmp2[i]="your";
	    }
	  }else{
	    if(see_names(who,active))
	    {
	      if(p && -1!=member_array(who,persons[0]))
	      {
		tmp2[i]=(string)who->query_possessive()+" own";
	      }else{
		foo=(string)who->query_name();
		if(foo[-1]=='s') return foo+"'";
		tmp2[i]=foo+"'s";
	      }
	    }else{
	      tmp2[i]="Someone's";
	    }
	  }
	}
	  
	t[e]=implode_nicely(tmp2)+tmp;
	break;
      }
      break;

    case 'o':
      if(sscanf(a,"obj%s",tmp) && member_array(active,persons[p])<0)
      {
	if(sizeof(persons[p])==1)
	{
	  if(persons[p][0]==this_player())
	    t[e]=this_player()->query_objective()+"self"+tmp;
	  else
	    t[e]=persons[p][0]->query_objective()+tmp;
	}else{
	  t[e]="them"+tmp;
	}
	break;
      }

    case 'O':
      if(sscanf(a,"OBJ%s",tmp) || sscanf(a,"obj%s",tmp))
      {
	tmp2=persons[p]+({});
	
	for(i=0;i<sizeof(tmp2);i++)
	{
	  who=tmp2[i];
	  if(who==active)
	  {
	    if(p && -1!=member_array(who,persons[0]))
	    {
	      tmp2[i]="yourself";
	    }else{
	      tmp2[i]="you";
	    }
	  }else{
	    if(see_names(who,active))
	    {
	      if(p && -1!=member_array(who,persons[0]))
	      {
		tmp2[i]=(string)who->query_objective()+"self";
	      }else{
		tmp2[i]=(string)who->query_name();
	      }
	    }else{
	      tmp2[i]="Someone";
	    }
	  }
	}

	t[e]=implode_nicely(tmp2)+tmp;
	break;
      }
      break;

    case 'i':
    case 'a':
      if(sscanf(a,"is%s",tmp) || sscanf(a,"are%s",tmp))
      {
	if(member_array(active,persons[p])==-1 && sizeof(persons[p])==1)
	  t[e]="is"+tmp;
	else
	  t[e]="are"+tmp;
      }
      break;

    case '$':
      sscanf(a,"$%s",tmp);
      if(member_array(active,persons[p])>=0)
	t[e]=tmp;
      else
	t[e]="s"+tmp;
      break;

    case '{':
      if(sscanf(a,"{%s,%s}%s",b,c,tmp))
      {
	if(member_array(active,persons[p])>=0)
	  t[e]=b+tmp;
	else
	  t[e]=c+tmp;
	break;
      }
      break;
    }
  }
#if DEBUG>3
  write("fix_msg returned '"+implode(t,"")+"'.\n");
#endif
  return implode(t,"");
}

/*
 * msg(message, who, target, rest);
 * message: a string to output
 * who, first person, person doing things, default is this_player()
 * target, who you do it to, default is nobody
 * rest, other people who will hear the message, default is everybody in the
 *   room
 *
 * The message can contain tags, generally all tags have this form:
 *  "\b<who><identifer>"
 * <who> is a number specifying 1 for who, 2 for target, 3 for rest,
 *       if no number is given, 1 (who) is assumed.
 * <idenifier> is a word specifying what this tag should be replaced with:
 * 'OBJ' will be replaced with the objective for that person,
 *       ie. you, yourself, himself, or a list of names
 * 'obj' will be replaced with your, him/her/it or them
 * 'POSS' will be replaced with your own, your or <name>'s
 * 'poss' will be replaced with his/her/its/their
 * 'PRON' will be replaced with you or a list of names
 * 'pron' will be replaced with the he/she/it/they
 * 'is' will be replaced with is/are
 * '{string1,string2}' will be replaced with string1 for the person
 *  given after \b and replaced with string2 for everybody else.
 * '$' is the same as {,$}
 *
 * You can capitalize any identifier by preceding it with 'cap', examples:
 *   '\b2capis', '\bcappron', '\bcapOBJ'
 *
 * some examples:
 * msg("\bPRON smile\b$ happily.\n");
 * msg("\bPRON \b{kiss,kisses} \b2OBJ.\n",this_player(),target);
 */
varargs void msg(string message,mixed first,mixed second,mixed rest)
{
  string st_msg,nd_msg,rd_msg;
  mixed *tmp;
  object o,env;
  int e;

  if(!first)
    first=({this_player()});
  else
    if(!pointerp(first))
      first=({first});
  
  if(!sscanf(message,"%*s\b"))
  {
    for(e=0;e<sizeof(first);e++) tell_object(first[e],message);
    return;
  }

  st_msg=message;
  if(sscanf(nd_msg=st_msg,"%s\b|%s",st_msg,nd_msg))
  {
    sscanf(rd_msg=nd_msg,"%s\b|%s",nd_msg,rd_msg);
  }else{
    rd_msg=nd_msg;
  }

  if(!second || !rest)
    env=environment(first[0]);

  if(!second)
    second=({});
  else
    if(!pointerp(second))
      second=({second});
  
  if(!rest)
    rest=all_inventory(env)-(mixed *)(first+second);
  else
    if(!pointerp(rest))
      rest=({rest});

  tmp=({first,second,rest});

  second-=first;
  rest-=second + first;

  for(e=0;e<sizeof(first);e++)
    tell_object(first[e],capitalize(fix_msg(st_msg,tmp,first[e])));

  for(e=0;e<sizeof(second);e++)
    tell_object(second[e],capitalize(fix_msg(nd_msg,tmp,second[e])));

  if(sizeof(rest))
  {
    message=capitalize(fix_msg(rd_msg,tmp,0));

    for(e=0;e<sizeof(rest);e++)
      tell_object(rest[e],message);
  }
}


