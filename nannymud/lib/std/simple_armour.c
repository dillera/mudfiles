/* Written by Profezzorn */
#pragma strict_types
#pragma save_types

inherit "/std/basic_thing";

/*** variables ***/
private string type = "armour";
private int class = 0;
private static int worn;

/*** Setup functions ***/
void set_type(string s) { type = s; }
void set_class(int c) { class=c; }

/*** Functions called by mudlib/driver ***/

int query_ac() { return class / 3; }
int armour_class() { return class; }

string short()
{
  string tmp,*xtra;
  tmp=::short();
  if(!tmp) return 0;
  xtra=({});

  if(worn) xtra+=({"worn"});
  if(query_property("cursed")) xtra+=({"cursed"});
  if(sizeof(xtra)) tmp+=" ("+implode_nicely(xtra)+")";

  return tmp;
}

string extra_look()
{
  if(worn)
    return ::extra_look();
  return query_property("__default_extra_look");
}

int is_armour() { return 1; }
object query_worn() { return worn && environment(); }
string query_type() { return type; }

int drop(int silently)
{
  string str;
  if(worn)
  {
    if((str=query_property("cursed")) && query_verb()!="quit")
    {
      if(!silently)
	write(stringp(str)?str:"You can't drop it, it seems to be cursed.\n");
      return 1;
    }

    if(environment())
    {
      object ob;
      if(ob=query_property("__remote"))
	if(ob->remote_do_remove(this_object(),0))
	  return 1;

      environment()->stop_wearing(type);
      worn=0;

      if(!silently)
	tell_object(environment(),"You drop your worn armour.\n");
    }
  }
  return ::drop(silently);
}

void do_remove()
{
  if(worn)
    environment()->stop_wearing(type);
  worn = 0;
}

int id(string s)
{
  return ::id(s) || s==type;
}

/*** commands ***/

int do_cmd(string what)
{
  object *weapons,ob;
  string str;
  
  if(::do_cmd(what)) return 1;
  
  switch(query_verb())
  {
  case "remove":
    if(!what || !(id(what) || what=="all"))
    {
      notify_fail("Remove what?\n");
      return 0;
    }
    
    if(!worn)
    {
      notify_fail("You are not wearing any "+what+".\n");
      return 0;
    }
    
    if(str=query_property("cursed"))
    {
      if(query_verb() != "quit" && !environment()->query_npc())
      {
	write(stringp(str)?str:short()+" seems to be stuck.\n");
	return 1;
      }
    }
    
    if(ob=query_property("__remote"))
      if(ob->remote_do_remove(this_object(),what=="all"))
	return 1;
    
    environment()->stop_wearing(type);
    worn=0;
    
    if(what=="all")
    {
      notify_fail("Ok.\n");
      return 0;
    }
    
    write("Ok.\n");
    return 1;
    

  case "wear":
    if(what == "all")
    {
      if(environment()!=this_player()) return 0;
      if(worn)
      {
	notify_fail("");
	return 0;
      }
    }else{
      if(!what || this_object()!=present(what,environment()))
      {
	notify_fail("Wear what?\n");
	return 0;
      }
      
      if(environment() !=this_player())
      {
	write("You don't have it.\n");
	return 1;
      }
      
      if(worn)
      {
	write("You are already wearing it.\n");
	return 1;
      }
    }
    
    
    if(type=="shield")
    {
      if(weapons = (object *) this_player()->query_wielded())
      {
	int e;
	for(e=0;e<sizeof(weapons);e++)
	{
	  if(weapons[e]->query_property("two_handed"))
	  {
	    write("You can't wear a shield while wielding a two handed weapon;.\n");
	    if(what=="all")
	    {
	      notify_fail("");
	      return 0;
	    }
	    return 1;
	  }
	}
      }
    }
    
    if(ob=query_property("__remote"))
      if(ob->remote_prevent_wear(this_object()))
	return 1;
    
    ob=(object)this_player()->wear(this_object());
    
    if(ob)
    {
      if(objectp(ob))
      {
	str=(string)ob->query_armour_block();
	if(str)
	{
	  write(str);
	} else {
	  write("You already wear a '"+type+"'\n");
	  if(str=(string)ob->short())
	    write("Worn armour: "+str+".\n");
	}
      }
    } else {
      worn=1;
      if(ob=query_property("__remote"))
	ob->remote_do_wear(this_object());
      
    }
    
    if(what == "all")
    {
      notify_fail("");
      return 0;
    }
    return 1;
  }


  return 0;
}


