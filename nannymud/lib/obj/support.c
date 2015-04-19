#pragma save_types

string number_to_string(int i);

status _is_living(object o) { return !!o->query_living(); }

void display_short(string s)
{
  tell_object(this_player(),capitalize(s)+".\n");
}

display_look(object ob, int foo) 
{
  string desc;
  if(!ob)
    ob=environment();

  if(!this_player())
     return;

  if (!foo && this_player()->query_toggles("brief") && 
      stringp(desc=ob->short()))
  {
    tell_object(this_player(), capitalize(desc) + ".\n");
  } else {
    if (desc = ob->query_long_desc())
      tell_object(this_player(), desc);  
    else  
      ob->long();
  }
  if (desc = ob->query_contents())      /* Mats 960223 */
    tell_object(this_player(), desc);
  else
  map_array((( all_inventory(ob)-({this_player()}) ) -> short()) - ({0}),
	    "display_short", this_object());
}

/* This function decides weather next word should begin with 'a' or 'an' */
status vocp(string a)
{
  switch(a[0])
  {
   case 'a': case 'e': case 'i': case 'o': case 'u':
  /* case 'y':   wrong, y pronounces j in the beginning of words */
    return 1;
  }
}

varargs string _name(object o,status f)
{
  string tmp;
  if(!objectp(o))
    return 0;
  if(o==this_player())
    return f?"!YOU":"YOU";

  if(o==environment(this_player()))
  {
    if(f) return "";

    switch(o->query_inorout())
    {
    default:
    case 2:
      return (f?"!the ":"")+"ground";

    case 1:
      return (f?"!the ":"")+"floor";
    }
  }

  if(tmp=(string)o->query_name())
    if(f)
      if(_is_living(o))
	return "!"+tmp;
      else
	return "!"+(vocp(tmp)?"an ":"a ")+tmp;
    else 
      return tmp;

  if(!(tmp=(string)o->short()))
  {
    if(_is_living(o))
      tmp="someone";
    else
      tmp="something";
  }else{
    sscanf(tmp,"a %s",tmp);
    sscanf(tmp,"A %s",tmp);
    sscanf(tmp,"an %s",tmp);
    sscanf(tmp,"An %s",tmp);
    sscanf(tmp,"the %s",tmp);
    sscanf(tmp,"The %s",tmp);
  }

  if(f)
    if(_is_living(o))
      return "!"+tmp;
    else
      return "!"+(vocp(tmp)?"an ":"a ")+tmp;
  else
    return tmp;
}

string a_name(object o) { return _name(o,1)[1..1000]; }

string pluralize(string a)
{
  string s, b;
  b = s = "";
  if(sscanf(a, "%s!%s", a, s) == 2)
    b = "! ";
  else if(sscanf(a, "%s!%s", a, s) == 2)
    b = "!";

  switch(a)
  {
  case "moose":
  case "fish":
  case "deer":
  case "sheep": return a+b+s;
  case "child": return "children"+b+s;
  case "tooth": return "teeth"+b+s;
  case "ox": return "oxen"+b+s;
  case "vax": return "vaxen"+b+s;
  case "chef": return "chefs"+b+s;
  case "knife": return "knives"+b+s;
  case "foot": return "feet"+b+s;
  case "thief": return "thieves"+b+s;
  case "mouse": return "mice"+b+s;
  case "staff": return "staves"+b+s;
  case "stuff": return "stuff"+b+s;
  case "platypus": return "platypuses"+b+s;

  case "alumnus": return "alumni"+b+s;
  case "apparatus": return "apparati"+b+s;
  case "bacillus": return "bacilli"+b+s;
  case "bronchus": return "bronchi"+b+s;
  case "cactus": return "cacti"+b+s;
  case "focus": return "foci "+b+s;
  case "fungus": return "fungi"+b+s;
  case "hamus": return "hami"+b+s;
  case "locus": return "loci"+b+s;
  case "modulus": return "moduli"+b+s;
  case "modus": return "modi"+b+s;
  case "nucleus": return "nuclei"+b+s;
  case "octopus": return "octopi "+b+s;
  case "radius": return "radii"+b+s;
  case "stimulus": return "stimuli"+b+s;

  case "calculus": return "calculuses"+b+s;
  case "terminus": return "terminuses"+b+s;

  case "asparagus": return " asparaguses"+b+s;
  case "bonus":  return "bonuses"+b+s;
  case "bones": return "bones"+b+s;
  case "bus":   return "buses"+b+s;
  case "campus": return "campuses"+b+s;
  case "census": return "censuses"+b+s;
  case "chorus": return "choruses"+b+s;
  case "circus": return "circuses"+b+s;
  case "consensus": return "consensuses"+b+s;
  case "genius": return "geniuses"+b+s;
  case "impetus": return "impetuses"+b+s;
  case "isthmus": return "isthmuses"+b+s;
  case "minus": return "minuses"+b+s;
  case "plus": return "plusses"+b+s;
  case "prospectus": return "prospectuses"+b+s;
  case "status": return "statuses"+b+s;
  case "surplus": return "surpluses"+b+s;
  case "torus": return "toruses"+b+s;
  case "virus": return "viruses"+b+s;
  case "walrus": return "walruses"+b+s;

  case "genus": return "genera"+b+s;
  case "corpus": return "corpora"+b+s;
  }

  if(a[-1]=='x' || a[-1]=='s' || (a[-1]=='h' && (a[-2]=='c' || a[-2]=='s')))
    return a+"es";

  a+="@";
/* if(sscanf(a,"%sff@",a) || sscanf(a,"%sfe@",a)) return a+"ves"+b+s; */

/*  if(sscanf(a,"%sef@",a)) return a+"efs"; */
  if(sscanf(a,"%sfe@",a)) return a+"ves"+b+s;
  if(sscanf(a,"%sff@",a)) return a+"ffs"+b+s;
  if(sscanf(a,"%sf@",a)) return a+"ves"+b+s;
  if(sscanf(a,"%sey@",a)) return a+"eys"+b+s;
  if(sscanf(a,"%sy@",a)) return a+"ies"+b+s;
  if(sscanf(a,"%sus@",a)) return a+"i"+b+s;
  if(sscanf(a,"%sman@",a)) return a+"men"+b+s;
  if(sscanf(a,"%sis@",a)) return a+"es"+b+s;
  if(sscanf(a,"%so@",a)) return a+"s"+b+s;
  sscanf(a,"%s@",a);
  return a+"s"+b+s;

/*
  switch(a[-1])
  {
   case 'h':
   case 'x':
    return a[0..strlen(a)-1]+"es"+b+s;
   case 'y':
    return a[0..strlen(a)-2]+"ies"+b+s;
   case 'e':
    if(a[-2]=='f')
      return a[0..strlen(a)-3]+"ves"+b+s;
   default:
    return a+"s"+b+s;
  }
*/
}

status _do_tell(object o, mixed *w)
{
  object tp;
  string what;
  string name;
  tp=w[1];
  if(interactive(o))
  {
    what=replace(w[0],"$",(o==(tp=w[1]))?"":"s");
    what=replace(what,"YOU",(o==tp)?"you":(string)tp->query_objective()+
		 "self");
    what=replace(what,(name=(string)o->query_name()||"")+", ","you, ");
    what=replace(what,name+".", "you.");
    what=replace(what,name+" ", "you ");
    tell_object(o,line_break(capitalize(what+".\n")));
  } else {
    int i;
    mixed *q;
    i=sizeof(q=w[2]);
    while(i--) tell_object(o,q[i]+".\n");
  }
}

int _tell_room(object room,
	       object tp,
	       mapping what,
	       string foo,
	       status f,
	       string *others,
	       object *ob)
{
  object *who;
  string *o,*a;
  mixed *b;
  int i;
  string fonk;

  switch(query_verb())
  {
   case "drop":
   case "put":
    fonk=" in ";
    break;
   case "take":
   case "get":
    fonk=" from ";
    break;
   default:
    fonk=" to ";
  }

  who=filter_array(all_inventory(room),"_is_living", this_object());
  o=allocate(i=m_sizeof(what));
  a=m_indices(what);
  b=m_values(what);
  while(i--)
    o[i]=implode_nicely(b[i])+
      (a[i]?fonk+a[i]:"");
  map_array(who,"_do_tell",this_object(),
	    ({sprintf("%s %s%s %s",tp->query_name(),foo,
		      f?"":"$",implode_nicely(o)), tp, others,ob}));
  return sizeof(who);
}


status pluralp(string s)
{
  string *q;
  q=explode(s," ");
  if(!q) return 0;
  if(!sizeof(q)) return 0;
  switch(q[-1])
  {
  case "money": return 1;
  }

  if(s[-1]=='s') return 1;
  if(sscanf(s,"%*d")) return 1;
  return 0;
}


varargs mixed
describe_objects(mixed *o, /* array of objects (or so) manipulated */
		 string s, /* manipulation command */
		 int ignore,
		 status sp) /* spell with s at end? */
{
  mapping map,zonk;
  mixed tmp;
  int i,foo;
  string a,b,*qwerk;  
  mixed *c,*d;
  string fonk,name;
  object *the_objects;

  name=(string)this_player()->query_name();

  switch(query_verb())
  {
  case "drop":
  case "put":
    fonk="in";
    break;
  case "take":
  case "get":
    fonk="from";
    break;
  default:
    fonk="to";
  }

  qwerk=({});
  map=([]);
  zonk=([]);

  qwerk=allocate(i=sizeof(o));
  the_objects=allocate(i);
  
  /* Foo put(s) */
  tmp=name+" "+s+(sp?"":"s");

  /* loop over all manipulated objects */
  while(i--)
  {
    string shortobj;
      
    /* do we have the name of the object and the object in this index? */
    if(pointerp(o[i]))
    {
      the_objects[i]=o[i][1];
      o[i]=o[i][0];
    }

    /* Does this, new object o[i] really, REALLY exist?
     * Brom 950831 
     */
    if (!o[i]) continue;

    /* Did the object have an environment? */
    if(!sscanf(o[i],"%s!%s",a,b))
    {
      a=o[i];
      b=0;
    }
      
    /* Do we have a pointer to the object? */
    if(the_objects[i])
    {
      if(!the_objects[i]->id(a))
      {
	/* emulate query_name() */
	shortobj=lower_case(a);
	sscanf(shortobj, "a %s", shortobj) ||
	  sscanf(shortobj, "the %s", shortobj) ||
	    sscanf(shortobj, "an %s", shortobj);

	while(!the_objects[i]->id(shortobj) && sscanf(shortobj,"%*s %s",shortobj));

	if(the_objects[i]->id(shortobj))
	  a=shortobj;
      }
    }

    /* Foo put/get/take(s) <object> in/to/from b */
    qwerk[i]=tmp+" "+a+(b?" "+fonk+" "+b:"");
    map[o[i]]++;
  }

  i=m_sizeof(map);
  c=m_values(map);
  d=m_indices(map);

  while(i--)
  {
    if(!sscanf(a=d[i],"%s!%s",a,b)) b=0;

    if(c[i] > 1)
    {
      a=number_to_string(c[i]) + " " + pluralize(a);
    }else{
      if(!pluralp(a))
      {
	if(vocp(a))
	{
	  a="an "+a;
	}else{
	  a="a "+a;
	}
      }
    }
    if(zonk[b])
      zonk[b]+=({a});
    else
      zonk[b]=({a});
  }

  if(!(intp(ignore) && ignore))
    _tell_room(environment(this_player()), this_player(), zonk, s, sp, qwerk,the_objects);
  else
    return zonk;
}

status valid_name(string str)
{
  int length;
  if (str == "logon") 
    return write("Invalid name"),0;
  length = strlen(str);
  if (length > 11) 
    return write("Too long name.\n"),0;

  while(length--) 
  {
    if (str[length] < 'a' || str[length] > 'z') 
    {
      write("Invalid characters in name:" + str + "\n");
      write("The character was :" + str[length..length] + ".\n");
      return 0;
    }
  }
  return 1;
}

void display_item(object ob, string item)
{
  string desc;
  if(stringp(desc = (string) ob -> query_long_desc(item)))
    return write(capitalize(desc));
  else if(stringp(desc = (string) ob->long(item)))
    return write(capitalize(desc));
}


string *singularize(string s)
{
  string *ret,a;
  switch(s)
  {
  case "moose":
  case "fish":
  case "deer":
  case "sheep": return ({s});
  case "children": return ({"child"});
  case "teeth": return ({"tooth"});
  case "oxen": return ({"ox"});
  case "vaxen": return ({"vax"});
  case "busses": return ({"bus"});
  case "chefs": return ({"chef"});
  case "knives": return ({"knife"});
  case "feet": return ({"foot"});
  case "thieves": return ({"thief"});
  case "mice": return ({"mouse"});
  /* more to come */

  default:
    ret=({});
    s+="@";
    if(sscanf(s,"%ses@",a)) ret+=({a,a+"s",a+"x",a+"ch",a+"sh"});
    if(sscanf(s,"%sves@",a)) ret+=({a+"f",a+"fe",a+"ff"});
    if(sscanf(s,"%sies@",a)) ret+=({a+"y"});
    if(sscanf(s,"%si@",a)) ret+=({a+"us"});
    if(sscanf(s,"%smen@",a)) ret+=({a+"man"});
    if(sscanf(s,"%ses@",a)) ret+=({a+"is"});
    if(sscanf(s,"%ss@",a)) ret+=({a,a+"o"});
    return ret;
  }
}

static mapping _singularize_cache=(mapping)"/obj/support"->get_global_cache();
mapping get_global_cache() { return _singularize_cache || ([]); }

smart_singularize(string s,object *envs)
{
  string *poss,*words,tmp;
  int x,y;

  if(tmp=_singularize_cache[s]) return tmp;
  words=(explode(s," ")||({s}))-({""});
  poss=singularize(words[-1]);

  if(sizeof(words)>1)
  {
    tmp=implode(words[0..sizeof(words)-2]," ")+" ";
    for(x=0;x<sizeof(poss);x++)  poss[x]=tmp+poss[x];
  }

  for(x=0;x<sizeof(envs);x++)
  {
    y=sizeof(poss);
    while(y--)
      if(present(poss[y],envs[x]))
	return _singularize_cache[s]=poss[y];
  }
  return 0;
}

string get_object_error;
string get_get_object_error() {}

get_objects(string s,object *in)
{
  int all,num,e,d,c,plural,everything,ins;
  object *ret,o;
  string a,b;

  object *tmp;
  int tmps;


  if(objectp(in)) in = ({ in });
  if(sscanf(s,"%s but %s",a,b)==2 || sscanf(s,"%s except %s",a,b)==2)
    return get_objects(a,in) - get_objects(b,in);

  ret=({});
  s = replace(s, ",", " and ");

  /* remove all double whitespaces */
  s=implode((explode(s," ")||({}))-({""})," ");
  if(s == "me" || s == "my" || s == "mine" || s=="myself")
    s=this_player()->query_real_name();
  
  if(!in) 
  {
    in=({ environment(this_player()), this_player() }); /* Same as present */
  }
  in-=({0});
  if(!(ins=sizeof(in)))
  {
    get_object_error = "From what?";
    return ({});
  }
  if((sscanf(s,"%s in %s",a,b) && !sscanf(a,"%*s from ")) ||
     sscanf(s,"%s from %s",a,b))
  {
    in = get_objects(b,in);
    /* Please fix this crude code I just wrote someone. /Profezzorn */
    for(e=0;e<sizeof(in);e++)
    {
      if(in[e] != this_player() &&
	 in[e] != environment(this_player()) &&
	 (!environment(in[e]) ||
	  (
	   environment(in[e]) != this_player() &&
	   environment(in[e]) != environment(this_player())
	   )
	  )
	 )
      {
	write("You can't reach there.\n");
	return ({});
      }

      if((in[e]->query_npc() || interactive(in[e]) && in[e]!=this_player()))
      {
        get_object_error="That's not yours.";
	return ({});
      }
    }
    if(!(ins=sizeof(in)))
    {
      get_object_error = "From what?";
      return ({});
    }
    s = a;
  }
  sscanf(s,"the %s",s);
  sscanf(s,"a %s",s);
  sscanf(s,"an %s",s);
  for(e=0;e<ins;e++)
    if(o = present(s, in[e])) 
      return ({ o });

 if(sscanf(s,"%s and %s",s,a)) 
    ret+=get_objects(a,in);
  

  s+="    ";
  if(atoi(s) && (sscanf(s,"%d coi%*s",e) == 2
		 || sscanf(s,"%d mone%*s",e) == 2
		 || sscanf(s,"%d gol%*s",e) == 2)
     || (sscanf(s,"%d%s",e,a) && !explode(a," ")))
  {
    object o2;
    if(e < 0)
      return write("You don't have negative coins.\n"), ({});
    for(d=0; d < ins; d++) 
    {
      if(_is_living(in[d]) && (in[d] != this_player()))
	continue;
      
      if(o2=present("money",in[d]) )
      {
	if((int)o2->query_value()==e) return ({o2});
	if((int)o2->query_value()<e) return ({});
	o2->add_value(-e);
	o=clone_object("/obj/money");
	o->set_volatile();
	o->set_money(e);
	transfer(o,in[d]);
	return ({o});
      } else if(in[d]->query_money() >= e && 
		(in[d]!=this_player() || (query_verb()!="take" 
					  && query_verb()!="get"))) {
	o=clone_object("obj/money");
	o->set_volatile();
	o->set_money(e);
	in[d]->add_money(-e);
	return ({ o });
      } 
    }
    get_object_error="You can't find that kind of money here.";
    return ({});
  }

  if(sscanf(s,"all %s",s) ||
     sscanf(s,"everything %s",s) || 
     sscanf(s,"every %s",s))
  {
    plural=1;
    all=1000;
    if(a=(string)smart_singularize(s,in)) s=a;
  }
  
  if(sscanf(s,"%s %s",a,s))
  {
    if(explode(s," "))
    {
      switch(a)
      {
      case "first"  :  num=1; break;
      case "second" :  num=2; break;
      case "third"  :  num=3; break;
      case "fourth" :  num=4; break;
      case "fifth"  :  num=5; break;
      case "sixth"  :  num=6; break;
      case "seventh":  num=7; break;
      case "eighth" :  num=8; break;
      case "ninth"  :  num=9; break;
      case "tenth"  :  num=10; break;
      case "eleventh": num=11; break;
      case "twelfth":  num=12; break;
      default: s=a+" "+s;
      }
    }else{
      s=a+" "+s;
    }
  }

  if(num)
  {
    string foo;
    /* remove double space */
    foo=implode((explode(s+" "+num," ")||({}))-({""})," ");
    for(e=0;e<ins;e++)
      if(o=present(foo,in[e]))
	return ({ o });
    return ({});
  }
  
  if(sizeof(ret))
    return ret;
  
  if(sscanf(s,"%s %s",a,s))
  {
    if(explode(s," "))
    {
      switch(a)
      {
      case "one":    all=0;  break;
      case "two":    all=1;  break;
      case "three":  all=2;  break;
      case "four":   all=3;  break;
      case "five":   all=4;  break;
      case "six":    all=5;  break;
      case "seven":  all=6;  break;
      case "eight":  all=7;  break;
      case "nine":   all=8;  break;
      case "ten":    all=9;  break;
      case "eleven": all=10; break;
      case "twelve": all=11; break;
      default: 
	if(atoi(a))
	  all=atoi(a)-1;
	else
	  s=a+" "+s;
      }
      if(all)
      {
	plural=1;
	if(a=smart_singularize(s,in)) s=a;
      }
    }else{
      s=a+" "+s;
    }
  }

  /* remove double space, again */
  s=implode((explode(s," ")||({}))-({""})," ");
  
  while(s[-1]==' ') s=s[0..strlen(s)-2];
  
  if(s=="" || s=="things" || s==" " || s=="everything")
    everything=1;

  for(e=0;e<ins;e++)
  {
    if(_is_living(in[e]) && in[e] != this_player())
      continue;
    tmps=sizeof(tmp=all_inventory(in[e]));
    d=0;
    while(d<tmps)
    {
      if(everything || tmp[d]->id(s))
      {
	ret+=tmp[d..d];
	if(--all<0) return ret;
      }
      d++;
    }
  }

  if(!sizeof(ret) && !plural && !everything)
  {
    if(s=smart_singularize(s,in))
    {
      for(e=0;e<ins;e++)
      {
	if(_is_living(in[e]) && in[e] != this_player())
	  continue;
	tmps=sizeof(tmp=all_inventory(in[e]));
	for(d=0;d<tmps;d++)
	  if(tmp[d]->id(s))
	    ret+=tmp[d..d];
      }
    }
  }

  return ret;
}

status __ok_to_get(object o)
{
  if(!environment(o)) return 0;
  if(environment(o)==this_player() || !o->short()) return 0;
  if(interactive(o)) return 0;
  return 1;
}

status __ok_to_drop(object o, object mag)
{
  return !(!o -> short() || mag->check_bound(o));
}

varargs object *get_getable_objects(string s, object *env, object *exclude)
{
  int e,d;
  if(!exclude) exclude=({});
  return filter_array((object *)get_objects(s,env), "__ok_to_get", 
		     this_object())-exclude;  
}

varargs object *get_dropable_objects(string s, object *env, object *exclude)
{
  object mag;
  if(!exclude) exclude=({});
  if(!(mag=present("magobj",this_player())))
    mag=load_object("room/room");
  return filter_array((object *)get_objects(s,env), "__ok_to_drop", 
		      this_object(),mag)-exclude;
}

/* -Exaggerating, me? /Profezzorn */
string number_to_string(int x)
{
  switch(x)
  {
    case 0: return "zero";
    case 1: return "one";
    case 2: return "two";
    case 3: return "three";
    case 4: return "four";
    case 5: return "five";
    case 6: return "six";
    case 7: return "seven";
    case 8: return "eight";
    case 9: return "nine";
    case 10: return "ten";
    case 11: return "eleven";
    case 12: return "twelve";
    case 13: return "thirteen";
    case 14: return "fourteen";
    case 15: return "fifteen";
    case 16: return "sixteen";
    case 17: return "seventeen";
    case 18: return "eighteen";
    case 19: return "nineteen";

    case 20: return "twenty";
    case 30: return "thirty";
    case 40: return "forty";
    case 50: return "fifty";
    case 60: return "sixty";
    case 70: return "seventy";
    case 80: return "eighty";
    case 90: return "ninety";

    case 21..29: case 31..39: case 41..49: case 51..59:
    case 61..69: case 71..79: case 81..89: case 91..99:
      return number_to_string((x/10)*10)+number_to_string(x%10);

    case 100: case 200: case 300: case 400: case 500:
    case 600: case 700: case 800: case 900:
      return number_to_string(x/100)+" hundred";

    case 101..199: case 201..299: case 301..399: case 401..499:
    case 501..599: case 601..699: case 701..799: case 801..899:
    case 901..999:
      return number_to_string((x/100)*100)+" and "+number_to_string(x%100);

    case 1000..999999:
      if(x%1000)
      {
	return number_to_string(x/1000)+" thousand "+number_to_string(x%1000);
      }else{
	return number_to_string(x/1000)+" thousand";
      }

    case 1000000: return "one million";
    case 1000001..1999999:
      return "one million "+number_to_string(x%1000000);

    case 2000000..999999999:
      if(x%1000000)
      {
	return number_to_string(x/1000000)+" millions "+number_to_string(x%1000000);
      }else{
	return number_to_string(x/1000000)+" millions";
      }

    case -0x7fffffff..-1: return "minus "+number_to_string(-x);

    default:
      return "many";
  }
}


