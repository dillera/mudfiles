inherit "obj/support";

#define MAX_LIST 30

int value;
string name_of_item;

string short()
{
  return "store room for the shop";
}

void init()
{
  if( this_player() && interactive(this_player()) && this_player()->query_level() < 20 ) {
    write("You feel your body yanked away.\n");
    this_player()->move_player("south, in a hurry", "/room/shop");
    return;
  }
  add_action("south", "south");
}

void list(object ob, int i)
{
  int value;
  
  if ((value = (int)ob->query_value()) && ob->short())
  {
    string sh, f;
    f=itoa(value*2);
    sh=capitalize((string)ob->short())[0..40];
    write(sprintf("| %3d: %s %'.'*s %s |##\n",i, sh,55-strlen(sh)-strlen(f),
		  "",f));
  }
}

void inventory(string str)
{
  object *oo;
  object ob;
  int max, i;
  oo=({});
  if (!str) str = "all";
  max = MAX_LIST;

  switch(str)
  {
  case "armours":
  case "armors":
  case "containers":
  case "weapons":
    for(ob=first_inventory(this_object());ob && max;ob=next_inventory(ob))
    {
      switch(str)
      {
       case "weapons":
	if(!ob->weapon_class()) break;
        oo+=({ob});
	max--;
	break;
	
       case "armours":
	if(!ob->armour_class()) 
	  break;
        oo+=({ob});
        max--;
	break;

       case "containers":
	if(!ob->can_put_and_get()) break;
        oo+=({ob});
	max--;
	break;

       case "treasures":
	if(ob->can_put_and_get()||!ob->armour_class()||ob->weapon_class()) break;
        oo+=({ob});
	max--;
	break;
      }
    }
    oo=oo[0..29];
    break;

  case "ALL":
    oo=all_inventory(this_object());
    break;

  case "all":
    oo=all_inventory(this_object())[0..29];
    break;

  default:
    str=lower_case(str);
    str=implode((explode(str," ")||({}))," ");
    if(str[0..3]!="all ") str="all "+str;
    oo=(object *)get_objects(lower_case(str), ({this_object() }) );
    oo=oo[0..29];
  }


  max=sizeof(oo);
  if(max)
  {

    write("+----------------------------------------------------------------+\n");
    write("| Item                                                     Price |##\n");
    write("|----------------------------------------------------------------|##\n");
    for(i=0;i<max;i++) list(oo[i], i+1);
    write("|_______________________________________________________________/#"+
	  "#\n  ############################################################"+
	  "####\n");
  }else{
    if(str=="all")
      write("The store is empty.\n");
    else
      write("No such object in store.\n");
  }
  
}

int value(string item)
{
  object o, *ob;
  int i;
  ob = (object *)get_objects(item, ({this_object()}));
  
  i = sizeof(ob);
  if (!i) return 0;
  
  while(i--)
  {
    o = ob[i];
    if(o -> short())
      if(value = (int)o -> query_value())
   	write("The "+_name(o) +" would cost you "+ 
	      (value*2) +" gold coins.\n");
  }
  return 1;
}

void buy(string item)
{
  object o, *ob, *bought;
  int i,tot;
  o = present(item,this_object());
  ob = (object *)get_objects(item, ({this_object()}));
  bought = ({});
  i = sizeof(ob);
  if (!i) 
    return write("No such item in the store.\n");
  
  while(i--)
  {
    o = ob[i];
    if(o -> short())
      if(value = 2 * ((int)o -> query_value()))
      {
	string __name;
	__name = _name(o);
	if (this_player()->query_money() < value)
	{
	  write("The "+ __name +" would cost you "+
		value+" gold coins, which you don't have.\n");
	  continue;
	}
	if(transfer(o, this_player()))
	{
	  write("You cannot carry the "+ __name +".\n");
	  continue;
	}
	this_player() -> add_money(-value);
	bought += ({__name});
	tot+=value;
      }
  }
  if(sizeof(bought))
  {
    describe_objects(bought, "bought", 0, 1);
    write("It cost you "+tot+" coins in total.\n");
  }
}

int south()
{
  this_player()->move_player("south#room/shop");
  return 1;
}

heart_beat()
{
  object ob, next_ob;
  ob = first_inventory(this_object());
  while(ob) {
    next_ob = next_inventory(ob);
    destruct(ob);
    ob = next_ob;
  }
}

reset(arg)
{
  object *ob;
  int i, n;

  if (!arg)
    set_light(1);

  /* This will possibly remove some objects from the game.
   * With huge amounts of mem (56M++), this is needed to save CPU.
   * Brom 960131
   */
  ob = all_inventory(this_object());
  for (i=1000; i<sizeof(ob); i++)
    destruct(ob[i]);

  /* Also dest some shitty equipment, and equipment that is extremely
   * expensive. 
   */
  for (i=0; i<sizeof(ob); i++)
  {
    if (living(ob[i])) continue;
    if (!random(3) ||
	((ob[i] -> is_weapon()) && (ob[i] -> weapon_class() < 12)) ||
	((ob[i] -> is_armour()) && (ob[i] -> query_class() < 1))   ||
	(ob[i] -> query_value() > 3000))
      destruct(ob[i]);
  }  
  if (!present("torch"))
  {
    object torch;
    torch = clone_object("obj/torch");
    torch->set_name("torch");
    torch->set_fuel(2000);
    torch->set_weight(1);
    move_object(torch, this_object());
  }
}

long()
{
  write("All things from the shop are stored here.\n");
}

store(item)
{
  string short_desc;
  object ob;

  if (!objectp(item)) return;
  if(short_desc=(string)item->query_property("special_item"))
  {
    if(stringp(short_desc))
    {
      write(short_desc);
    }
    else{
      if ((item -> query_value()) > 100)
	write("The valuable item is hidden away.\n");
      else
	write("The item is thrown away by the shop keeper.\n");
    }
    move_object(item, this_object());
    if (objectp(item)) destruct(item);
    return;
  }
  short_desc = call_other(item, "short");
  ob = first_inventory(this_object());
  while(ob)
  {
    if (ob->short() == short_desc)
    {
      /* Move it before destruct, because the weight
	 has already been compensated for. */
      move_object(item, this_object());
      destruct(item);
      return;
    }
    ob = next_inventory(ob);
  }
  move_object(item, this_object());
}

query_prevent_shadow() { return "Profezzorn was here."; }
query_inorout() { return 1; }

