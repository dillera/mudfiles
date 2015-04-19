inherit "/std/rfc/pluralize";
inherit "/std/rfc/string_to_number";

#define array mixed *
#define arrayp(x) pointerp(x)
#define _explode(x,y) (explode(x,y)||({""}))

void error(string err)
{
  if(this_player() && this_player()->query_level()>19) write(err);
  throw(err);
}

/*
 * Parser, find objects from a 'natural language' string.
 * The types in the comments in this file will be 'uLPC-stylish', this
 * allows for more precise description of the actual types.
 *
 * An array (x|y) foo is an array with eiter 'x's or 'y's in it.
 * Thus, array (string|object) is the same as mixed *, but it indicates that 
 * there are either strings or objects in the array.
 *
 * See parser.ref for more info.
 * In genera|, simply try giving it a string in natural english language, like
 * 'all the bottles present in the bags milamber have'
 */

#define CHECK_ARGS


#define DESC_SHORT 1
#define DESC_LONG  2


#define DESC_THE   4
#define DESC_A     8
#define DESC_PLURAL 16

/* Public functions */
string describe(mixed o, int form);
string describe_composite(object *o, int form);
string find(string what, mixed in);

/* Private functions, don't touch.. */
private static string short_name(object o, string id);
private static string long_name(object o, string id);
private static string living_the(string name);
private static string a_an(string name);
private static string is_plural(string name);
varargs static private array  low_find(string what, mixed in);

string number_to_string(int number)
{
  return ("/obj/support"->number_to_string(number));
}


/* ----------------------------------------------------------------
 | Public functions 
 *-----------------------------------------------------------------*/

/* Might be useful.. */

object split_money(object original, int to)
{
  object new;
  if(to > original->query_value())
    error("Cannot have money with negative value, can we?\n");
  new = clone_object("/obj/money");
  new->set_value(to);
  original->set_value(original->query_value()-to);
  if(environment(original))
    move_object(new, environment(original));
  new->set_volatile();
  return new;
}

object is_living(string s)
{
  s=lower_case(s);
  sscanf(s, "%s'", s);
  return find_living(s);
}


int is_ordered(string what)
{
  if(atoi(what))
    return atoi(what);
  switch(lower_case(what))
  {
   case "first":
    return 1;
   case "second":
    return 2;
   case "third":
    return 3;
   case "fourth":
    return 4;
   case "fifth":
    return 5;
   case "sixth":
    return 6;
   case "seventh":
    return 7;
   case "eighth":
    return 8;
   case "ninth":
    return 9;
   case "tenth":
    return 10;
   case "eleventh":
    return 11;
   case "twelfth":
    return 12;
  }
  return 0;
}

/*
 * array (object) find(string what, array|object|void in, int|void howmany);
 *
 *  'a and c from d' is parsed like a and (c from d), not
 *  (a and c) from d.
 */

private static mixed find2(string what, mixed in, int howmany, int from_flag);

array unique(array from)
{
  int i;
  mapping res;
  res = ([]);
  for(i=sizeof(from)-1; i>=0; i--)
    if(!res[from[i]])
      res[from[i]]=1;
  return m_indices(res);
}

varargs mixed find(string what, mixed in, int howmany)
{
  mixed tmp;
  tmp = find2(what, in , howmany, 0);
  if(tmp)
    return unique(tmp);
  return 0;
}

/*
 * (a from b) and c
 * a from (b and from c)
 * (a from b) except (c from d)
 */

varargs private static mixed find2(string what, mixed in, int howmany, int from_flag)
{
  int i, sum, hmm;  
  int the_flag;
  object who;
  int a_flag, all_flag;
  string obj, from;
  mixed tmp;

  if(objectp(in))
    in = ({ in });
  else if(!in)
    in = ({ environment(this_player()), this_player() });
  else if(!arrayp(in))
    error("Wrong type of argument to " +
	  "find(string what, array(object)|object|void in)\n");
      
  what = lower_case(what);
  if(sscanf(what, "%s except %s", obj, from)
     || sscanf(what, "%s but %s", obj, from)
     || sscanf(what, "not %s but %s", from, obj))
  {
    tmp = find2(obj, in, 0, 0);
    if(!tmp)
      return 0;
    in = find2(from, in, 0, 0);
    if(in) tmp -= in;
    return tmp;
  }

  what = replace(what, " present in ", " in ");
  what = replace(what, " from ", " in ");

  if(sscanf(replace(what, ", and", ",and "), "%s ,and %s", obj, from) == 2)
  {
    if(sscanf(from, "in %s", tmp) && sscanf(obj, "%s in %s", obj, from))
    {
      mixed res, tmp2;
      /*  "a in b, and in c" */
      tmp = find2(from + " and " + tmp, in, 1, 0);
      if(!tmp)
      {
	notify_fail("In what?\n");
	return 0;
      }
      res = ({ });
      for(i=0; i<sizeof(tmp); i++)
      {
	if(tmp2 = find2(obj, ({ tmp[i] }), 0, 0))
	  res += tmp2;
      }
      if(sizeof(res))
	return res;
    }
    tmp = find2(obj, in, 0, 0);
    if(!tmp)
      return 0;
    tmp += find2(from, in, 0, 0)||({});
    return tmp;
  }

  tmp = _explode(replace(what, "\t", " "), " ") - ({ "" });
  what = implode(tmp, " ");

  if(this_player())
  {
    if(sscanf(lower_case(what), "my own %s", what))
      what = this_player()->query_real_name()+"'s" + " " + what;

    tmp = _explode(what, " ");
    for(i=0; i<sizeof(tmp); i++)
    {
      switch(lower_case(tmp[i]))
      {
       case "i":
       case "me":
	tmp[i] = this_player()->query_real_name();
	break;

       case "my":
       case "mine":
	tmp[i] = this_player()->query_real_name()+"'s";
      }
    }
    what = implode(tmp, " ");
  }
  
  if(sscanf(what, "%s that's %s", obj, from) == 2
     || sscanf(what, "%s that is %s", obj, from) == 2
     || sscanf(what, "%s which are %s", obj, from) == 2)
  {
    mixed a, b;
    a=find2(obj, in, 0, 0);
    if(!a) return 0;
    b=find2(from, in, 0, 0);

    sscanf(from, "also %s", from);

    if(b) 
      a=a&b;
    else 
      a=({});

    if(!sizeof(a))
    {
      if(is_plural(obj))
	obj = is_plural(obj);
      notify_fail("There are no " + pluralize(obj) + " which also are " 
		  + from + ".\n");
      return 0;
    }
    return a;
  }

  if(sscanf(what, "%s in %s", obj, from) == 2
     || sscanf(what, "%s %s own", obj, from)==2 
     || sscanf(what, "%s %s has", obj, from)==2
     || sscanf(what, "%s %s have", obj, from)==2)
  {
    mixed f;
    if(!(f = find2(from, in, 0, 0)))
    {
      notify_fail("From what?\n");
      return 0;
    }
    return find2(obj, f, 0, 0);
  }

  what = replace(what, "everything", "objects");
  if(sscanf(what, "all %s", what) || what=="all")  all_flag = 1;

  /* "milamber's money"  */
  if(sscanf(what, "%s %s", from, obj))
  {
    if(who = is_living(from))
    {
      if(member_array(environment(who), in) == -1)
      {
	notify_fail(capitalize(who->query_real_name()) + " is not here.\n");
	return 0;
      }
      return find2(obj, ({ who }), 0, 0);
    }
  }


  if(sscanf(what, "a %s", what) 
     || sscanf(what, "an %s", what)
     || sscanf(what, "any of the %s", what) 
     || sscanf(what, "any one of the %s", what)
     || sscanf(what, "one of the %s", what)) a_flag = 1; 

  what = lower_case(what);

  if(sscanf(what, "the %s", what) ||
     sscanf(what, "this %s", what))
  {
    string order;
    the_flag = 1;

    if(sscanf(what, "%s %s", order, obj) == 2 && (howmany = is_ordered(order)))
    {
      sscanf(obj, "of %s", obj);
      sscanf(obj, "the %s", obj);

      if(what = is_plural(obj))
	obj = what; /* the second of the bottles */
    
      if(tmp = find2(obj + " " + howmany, in, 0, 0)) /* 'bottle 2' etc. */
	return ({ tmp[0] });
      else
      {
	tmp = find2(obj, in, 0, 0);
	if(tmp)
	  notify_fail("You only have "+number_to_string(sizeof(tmp)) 
		      + " " + obj + "\n");
	else
	  notify_fail("You have no "+obj+" at all.\n");
	return 0;
      }
    }
  }

  if(!a_flag)  
  {
    if(obj = is_plural(what)) 
    {
      all_flag = 1;
      what = obj;
    }
  }
    
  if(tmp = string_to_number(what))
  {
    howmany = tmp[0];
    all_flag = 0;
    what = tmp[1];
  }
  
  if(sscanf(what, "%*sorth of %s",  from, obj) == 2)
  {
    tmp = find2(obj, in, 0, 0);
    if(!tmp) return 0;
    for(i=0; i<sizeof(tmp); i++)
    {
      if(sum + (intp(tmp[i])?tmp[i]:tmp[i]->query_value()) <= howmany)
	sum +=(intp(tmp[i])?tmp[i]:tmp[i]->query_value());
      else
	tmp[i] = 0;
    }
    return tmp - ({ 0 });
  }

  switch(what)
  {
   case "coin":
   case "gold coin":
    if(!all_flag && !howmany)
      howmany = 1;

   case "money":
   case "coins":
   case "gold coins":
    if(tmp = low_find("money", in))
    {
      if((a_flag && !howmany))
	howmany = 1;

      if(!howmany || all_flag)
	return tmp;

      for(i=0; i<sizeof(tmp); i++)
      {
	hmm += intp(tmp[i])?tmp[i]:tmp[i]->query_value();
	if(hmm >= howmany) break;
      }

      for(i++;i<sizeof(tmp); i++)	tmp[i]=0;
      tmp -= ({ 0 });

      if(hmm == howmany)
	return tmp;
      
      if(hmm < howmany)
      {
	notify_fail("You cannot find that kind of money.\n");
	return 0;
      }

      if(intp(tmp[-1]))
	tmp[-1] -= hmm - howmany;
      else
	tmp[-1] = split_money(tmp[-1], hmm - howmany);
      return tmp;
    }
    return 0;

   default:
    tmp = low_find(what, in);
    if(!tmp)
    {
      if(all_flag)
	notify_fail("You cannot find any "+pluralize(what)+".\n");
      else
	if(!a_flag)
	  if(!the_flag)
	    notify_fail("You cannot find any "+pluralize(what)+".\n");
	  else
	    notify_fail("You cannot find the "+what+".\n");
	else
	  notify_fail("You cannot find " + a_an(what) + " " + what + ".\n");
      return 0;
    }

    if(howmany && sizeof(tmp) < howmany)
    {
      notify_fail("You only found " + number_to_string(sizeof(tmp)) + " "
		  + (sizeof(tmp)==1?what:pluralize(what)) + ".\n");
      return 0;
    }

    if(the_flag && !all_flag && sizeof(tmp)>1)
    {
      string s;

      if(from_flag) 
	return tmp[0];

      s = line_break(describe_composite(tmp, DESC_LONG|DESC_A)+".");
      tmp = "You found "+number_to_string(sizeof(tmp))+": " 
	+ replace(s[0..strlen(s)-2], "\n", "\n   ") + "\n";
      notify_fail("Which "+what+"?\n"+tmp);
      return 0;
    }

    if(all_flag) return tmp;
    if(a_flag) return ({ tmp[ random(sizeof(tmp)) ] });
    if(howmany) return tmp[0 .. howmany-1];
    return ({ tmp[0] });
  }
  error("Not reached (tm).\n");
}

/* 
 * string describe(object|array|int identifier, int flags)
 * Describe an object.
 * 
 * If 'o' is an integer number, it is assumed to be some money from the character
 * this_player().
 *
 * If 'o' is an object, use that as the identifier, otherwise, if 
 * o is an array, use the {short|query_name} function in that object 
 * with the second index as an identifier.
 *
 *  The second argument is an bitwise or of these flags:
 *
 * DESC_SHORT       -- Use the short form (one word)
 * DESC_LONG        -- Use a somewhat longer form (from the short() function)
 *
 * Optional:
 * DESC_THE         -- Use determinative form. (the sword, Milamber)
 * DESC_A           -- Use generalized from. (a sword, but names are not
 *                     generalized, that is, not 'a Milamber', but 'Milamber') 
 * DESC_PLURALIZE   -- Pluralize (swords, but still 'Milamber')
 */

string describe(mixed o, int form)
{
  string name_of;

  if(intp(o))
  {
    /* This is the easiest one. */
    if(!this_player())
      return number_to_string(o) + " coin"+(o==1?"":"s");
    if(o!=this_player()->query_money())
      return number_to_string(o) + " of your coins";
    return "all your money";
  }

  if(objectp(o))
    o = ({ o, 0 });

  if(form & DESC_SHORT)
  {
    name_of = short_name(o[0], o[1]);
    if(!name_of) return 0;
    if(form & DESC_PLURAL)
    {
      /* If this is a name, you cannot really pluralize it.. */
      if(o[0]->query_living() && living_the(name_of) == name_of)
	return name_of;
      return pluralize(name_of);
    }
    if(form & DESC_THE)
    {
      if(o[0]->query_living())
	return living_the(name_of);
      return "the " + name_of;
    }
    if(form & DESC_A)
    {
      /* If this is a name, don't add an 'a' or 'an' to it */
      if(o[0]->query_living() && living_the(name_of) == name_of)
	return name_of;
      return a_an(name_of) + " " + name_of;
    }
    return name_of;
  } else if(form & DESC_LONG) {
    name_of = long_name(o[0], o[1]);
    if(!name_of) return 0;
    if(form & DESC_PLURAL)
    {
      string name;

      /* If this is a name, you cannot really pluralize it.. */
      if(o[0]->query_living() && living_the(name_of) == name_of)
	return name_of;
      name = short_name(o[0], o[1]);
      if(!name) return 0;
      return replace(name_of, name, pluralize(name));
    }
    if(form & DESC_THE)
    {
      if(o[0]->query_living())
	return living_the(name_of);
      return "the " + name_of;
    } 
    if(form & DESC_A)
    {
      /* If this is a name, don't add an 'a' or 'an' to it */
      if(o[0]->query_living() && living_the(name_of) == name_of)
	return name_of;
      return a_an(short_name(o[0], o[1])) + " " + name_of;
    }
    return name_of;
  } else {
    error("Eh? Neither 'DESC_SHORT' or 'DESC_LONG' ored to the bitfield.\n");
  }
}

string describe_composite(mixed *objects, int how, int nl)
{
  int i;
  string s;
  mapping o;
  o = ([ ]);

  for(i=0; i < sizeof(objects); i++)
  {
    if(s=describe(objects[i], how))
    {
      if(o[s]) o[s] += ({ objects[i] });
      else o[s] = ({ objects[i] });
    }
  }
  objects = m_indices(o);
  for(i=0; i<sizeof(objects); i++)
    if(sizeof(o[objects[i]]) > 1)
      objects[i] = number_to_string(sizeof(o[objects[i]])) + 
              	   " " + describe(o[objects[i]][0], how|DESC_PLURAL);

  if(nl) return implode(objects, "\n")+"\n";
  return implode_nicely(objects);
}


/* This most definately needs more work.. */

/* Return singularised string if 'what' is pluralized, otherwise 0. */

private string is_plural(string what)
{
  string pre;
  mixed tmp;
  tmp = _explode(what, " ") - ({ "" });
  what = tmp[-1];

  if(sizeof(tmp)>1)
    pre = implode(tmp[0..sizeof(tmp)-2], " ") + " ";
  else
    pre="";

  switch(what)
  {
   case "foxen": return pre+"fox";
   case "knives": return pre+"knife";
  }

  switch(strlen(what))
  {
   default:
    if(pluralize(what[0..strlen(what)-4]) == what)
      return pre+what[0 .. strlen(what) - 4];
   case 4:
    if(pluralize(what[0..strlen(what)-3]) == what)
      return pre+what[0 .. strlen(what) - 3];
   case 3:
    if(pluralize(what[0..strlen(what)-2]) == what)
      return pre+what[0 .. strlen(what) - 2];
   case 2:
   case 1:
   case 0:
    return 0;
  }
  return 0;
}

/* 'a' or 'an'? */
private static string a_an(string a)
{
  if(!a) return 0;
  switch(a[0])
  {
   case 'a': case 'e': case 'i': case 'o': case 'u':
    return "an";
  }
  return "a";
}



/* Try to find out if this 'name' is really a name.
 * Otherwise, we have to add a 'the ' to the string.
 */

private static string living_the(string name)
{
  if(!name) return 0;
  if(lower_case(name) == name)
    return "the "+name;
  
  /* Here we should try to find some other common cases... */
  return name;
}

private static string trim_long_name(string from)
{
  sscanf(from, "to the %s", from);
  sscanf(from, "is the %s", from);
  sscanf(from, "%*s is the %s", from);
  sscanf(from, "%*s's %s", from);
  sscanf(from, "there is %s", from);
  sscanf(from, "a %s", from);
  sscanf(from, "an %s", from);
  sscanf(from, "the %s", from);
  return from;
}

status is_monster(object o) { return o->query_living() && o->query_npc(); }
status is_creature(object o) { return o->query_living(); }
status is_player(object o) {  return interactive(o); }
status is_object(object o) { return !o->query_living(); }
status is_container(object o) {  return !!(o->can_put_and_get()); }
status is_money(object o) { return !!(o->query_money() && o->id("money")); }
status is_weapon(object o) { return o->short() && o->weapon_class(); }
status is_armour(object o) { return (o->armour_class() || o->query_ac()); }
status is_edible(object o) 
{ 
  return (o->query_heal() || o->query_strength()) && o->query_full(); 
}

status is_equipment(object o) 
{ 
  return is_weapon(o) || is_armour(o) || is_container(o) || is_edible(o); 
}
status is_treasure(object o) 
{ 
  return o->query_value() && !is_equipment(o) && !is_creature(o); 
}

status has_id(object o, string i) 
{ 
  return (o->short() && (o->id(i) || sscanf(o->short(), "%*s "+i+" %*s"))); 
}
status has_inventory(object o) 
{ 
  return !environment(o) || is_container(o) || is_creature(o);
}

static private array low_find(string what, array in)
{
  int order, i;
  mixed tmp;
  array res;

  res = ({ });

  in = unique(filter_array(in, "has_inventory", this_object()));

  if(is_plural(what)) what=is_plural(what); 

  write("Searching for ("+what+").\n");

  if(sscanf(what, "%s %d", tmp, order) == 2)
  {
    what = tmp;
    res = allocate(sizeof(in));
    /* Tricky.. */
    for(i=0; i<sizeof(in); i++)
      if(sizeof(tmp = low_find(what, ({in[i]}))||({})) >= order)
	res[i] = tmp[order-1];
    res -= ({ 0 });
    return sizeof(res)?res:0;
  }

  sscanf(what, "%s things", what);
  sscanf(what, "%s objects", what);
  sscanf(what, "%s items", what);

  switch(what)
  {
   case "something":
    for(i=0; i<sizeof(in); i++)
    {
      if(tmp = filter_array(all_inventory( in[i] ),"is_object", this_object()))
	res += ({ tmp[ random(sizeof(tmp)) ] });
    }
    break;

   case "somebody":
    for(i=0; i<sizeof(in); i++)
    {
      if(sizeof(tmp = filter_array(all_inventory( in[i] ),
				   "is_creature", this_object())))
	res += ({ tmp[ random(sizeof(tmp)) ] });
    }
    break;
    

   case "all":
    for(i=0; i<sizeof(in); i++)
      res += all_inventory( in[i] );
    break;

   case "money":
    for(i=0; i<sizeof(in); i++)
    {
      if(in[i] == this_player())
	res += ({ this_player()->query_money() });
      else
	res += filter_array(all_inventory( in[i] ), "is_money", this_object());
    }
    break;

   case "everyone":
   case "everybody":
    for(i=0; i<sizeof(in); i++)
    {
      res += filter_array(all_inventory( in[i] ), 
			  "is_creature", this_object());
    }

   case "container":
    for(i=0; i<sizeof(in); i++)
      res += filter_array(all_inventory( in[i] ), "is_container", this_object());
    break;

   case "person":
   case "player":
    for(i=0; i<sizeof(in); i++)
      res += filter_array(all_inventory( in[i] ), "is_player", this_object());
    break;

   case "monster":
   case "living":
    for(i=0; i<sizeof(in); i++)
      res += filter_array(all_inventory( in[i] ), "is_monster", this_object());
    break;

   case "environment":
   case "room":
   case "ground":
   case "floor":
   case "wall":
    for(i=0; i<sizeof(in); i++)
      if(environment(in[i]))
	res += ({ environment(in[i]) });
    break;

   case "object":
   case "thing":
    for(i=0; i<sizeof(in); i++)
      res += filter_array(all_inventory( in[i] ), "is_object", this_object());
    break;

   case "food":
   case "edible":
   case "heal":
    for(i=0; i<sizeof(in); i++)
      res += filter_array(all_inventory( in[i] ), "is_edible",this_object());
    break;

   case "treasure":
    for(i=0; i<sizeof(in); i++)
      res += filter_array(all_inventory( in[i] ), "is_treasure",this_object());
    break;

   case "equipment":
    for(i=0; i<sizeof(in); i++)
      res += filter_array(all_inventory( in[i] ), "is_equipment", this_object());
    break;

   case "weapon":
    for(i=0; i<sizeof(in); i++)
      res += filter_array(all_inventory( in[i] ), "is_weapon", this_object());
    break;

   case "armour":
    for(i=0; i<sizeof(in); i++)
      res += filter_array(all_inventory( in[i] ), "is_armour", this_object());
    break;

   case "body armour":
    what = "armour";
    
   default:
    for(i=0; i<sizeof(in); i++)
    {
      res += filter_array(all_inventory(in[i]), "has_id", this_object(), what);
      if(in[i]->id(what)) res += ({ ({ in[i], what }) });
    }
    break;
  }
  res -= ({ 0 });

  if(!sizeof(res))
  {
    mixed foo, bar;
    if(sscanf(replace(what, ",", " and "), "%s and %s", foo, bar))
    {
      foo = low_find(foo, in);
      bar = low_find(bar, in);
      if(foo && !bar) return foo;
      if(!foo && bar) return bar;
      if(foo && bar) return foo + bar;
    }

    if(sizeof(foo = _explode(what, " ")) > 1)
    {
      bar = low_find(foo[0], in);
      if(!bar) return 0;
      foo = find2(implode(foo[1..100000], " "), in, 0, 0);
      if(!foo) return 0;
      bar = bar & foo;
      if(sizeof(bar))
	return bar;
    }
    return 0;
  }
  return res;
}

private static string trim_name(string from, object o)
{
  int i;
  string probably, not_really;
  mixed foo;

  switch(sscanf(from, "%s of %s", probably, not_really))
  {
   default:
    probably = from;
   case 1:
    not_really = "";
   case 2:
  }

  foo = _explode(probably, " ") - ({ "" });
  for(i=sizeof(foo)-1; i>=0; i--)
  {
    if(o->id(foo[i]))
      return foo[i];
  }

  foo = _explode(not_really, " ") - ({ "" });
  for(i=sizeof(foo)-1; i>=0; i--)
  {
    if(o->id(foo[i]))
      return foo[i];
  }

  /* Fallback.. */
  sscanf(from, "%s of", from);
  return trim_long_name(from);
}

private static string short_name(object o, string id)
{
  string res;
  if(o->query_living())
    return o->query_name();

  if(id)
  {
    if(res = o->query_long_desc(id))  return trim_name(lower_case(res), o);
    if(res = o->query_long(id))   return trim_name(lower_case(res), o);
    if(res = o->short(id))  return trim_name(lower_case(res), o);
    if(res = o->query_name(id))   return trim_name(lower_case(res), o);
  }  else {
    if(res = o->query_name())   return trim_name(lower_case(res), o);
    if(res = o->short())  return trim_name(lower_case(res), o);
    if(res = o->query_long_desc())   return trim_name(lower_case(res), o);
    if(res = o->query_long())  return trim_name(lower_case(res), o);
  }
  return 0;
}

private static string long_name(object o, string id)
{
  string res;

  if(o->query_living()) if(res = o->short(id)) return trim_long_name(res);
  
  if(id)
  {
    if(res = o->query_long_desc(id)) return trim_long_name(lower_case(res));
    if(res = o->query_long(id)) return trim_long_name(lower_case(res));
    if(res = o->short(id)) return trim_long_name(lower_case(res));
    if(res = o->query_name(id)) return trim_long_name(lower_case(res));
  } else {
    if(res = o->short(id)) return trim_long_name(lower_case(res));
    if(res = o->query_name(id)) return trim_long_name(lower_case(res));
    if(res = o->query_long_desc(id)) return trim_long_name(lower_case(res));
    if(res = o->query_long(id)) return trim_long_name(lower_case(res));
  }
  return 0;
}

