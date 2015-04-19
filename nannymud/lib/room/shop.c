inherit "room/room";
inherit "obj/support";

reset(int arg)
{
  object bin;

  if(!arg)
  {
    set_light(1);
    set_short("The shop");
    set_long("You are in a shop. You can buy or sell things here.\n" +
	     "On the wall, there is a large sign and to the north you \n"+
	     "see an opening, shimmering with blue light.\n");
    property = ({"no_fight"});
    set_indoors();
    add_item(({"sign", "plaque"}), 
	     "It's a pretty large, wooden sign. Perhaps reading it would be a\n"+
	     "a good idea");
    add_item(({"light","blue light"}),"Looks like the effect of some strong magic");
    add_item("doorway","Hmm, seems to lead to a room _full_ with stuff.");
    add_item("shop","It's a magical, fully automated, self-service shop. Very impressive.");
    add_exit("room/vill_road2","south");
  }
  if (!present("bin"))
  {
    bin = clone_object("obj/bin");
    move_object(bin, this_object());
  }
} /* reset */

void init()
{
  ::init();
  add_action("sell", "sell");
  add_action("value", "value");
  add_action("buy", "buy");
  add_action("north", "north");
  add_action("list", "list");
  add_action("read", "read");
}

int read(string s)
{
  if(!s)
    return notify_fail("Read what?\n"), 0;
  if(s == "sign" || s == "plaque")
  {
    say(this_player() -> query_name() + " reads a sign.\n");
    return write(read_file("room/shop_text.txt")), 1;
  }
  return notify_fail("You see nothing like that here.\n"), 0;

}

void test_ill(object ob,int foo);

string do_sell(object ob, mapping s)
{
  int value;
  object e;
  string __name;
  
  test_ill(ob, 0);		/* added by Mats 901011 */

  __name = _name(ob);
  value = (int)ob->query_value();
  if (environment(ob) != this_object() && ob->drop(1))
    return 0;
      /* write("I can't take the "+__name+" from you!\n")
	 removed by Ppannion is this really neccecary ?
	 just a lot of irritating text for all autoloaders
      */

  environment(ob)->add_weight(-((int)ob->query_weight()));
  if (ob->query_wet())
  {
    write("The "+ __name +" is wet! You only get 90% the money for it.\n");
    value -= value/10;
  }  
  if (value>1000)
  {
    write("The shop is low on money and you only get "+
	  (value = 950 + random(101))+" coins for the "+ __name +".\n");
  }
  this_player()->add_money(value);
  add_worth(value, ob);
  s && (s[0] += value);
  "room/store"->store(ob);
  return __name;
}

int id_check(object o,string desc)
{
  if(o->id(desc)) return 1;
  if(desc[-1]=='s' && o->id(desc[0..strlen(desc)-2])) return 1;
  return 0;
}

int sell(string item)
{
  object ob,magobj,env,*obs;
  string cont, where, *mess;
  mapping s;
  int sell, e, no_force;
    
  s=([ 0:0 ]);
  magobj = present("magobj", this_player());
  if (!item) 
    return notify_fail("Sell what?\n"),0;
  if(item != "ALL")
    no_force = 1; 
  mess = allocate(sizeof(obs = (object *)get_objects(lower_case(item), 
						   ({this_player()}))));

  for(sell=e=0;e<sizeof(obs);e++)
  {
    ob = obs[e];
    if (ob -> query_money()) {
      write("You don't sell your money.\n");
      return 1;
    }
    if(no_force && (ob -> query_wielded() || ob -> query_worn()))
       write("You don't sell your "+_name(ob)+".\n");
    else 
      /* Added a check for ob after ob->drop() since sometimes the item
	 is destructed in drop(). Banshee 951018 */
      if (!ob->drop() && ob &&
	  ob->query_value() &&
	  ob->short() &&
	  (!magobj || !magobj->check_bound(ob)))
      {
	sell++;
	mess[e] = do_sell(ob,s);
      }
  }
  if(!sell)
  {
    write("Nothing to sell.\n");
    return 1;
  }
  describe_objects(mess-({0}), "sold", 0 , 1);
  write("You got "+s[0]+" coins in total.\n");
  return 1;
}

int value(string item)
{
  int value, i, found;
  object o, *ob;
  
  if(!item) return notify_fail("Value what?\n"),0;
  ob = (object *)get_objects(item, ({this_player()}));
  
  i = sizeof(ob);
  while(i--)
  {
    o = ob[i];
    if(o -> short())
    {
      value = (int)o->query_value(0);
      if (!value)
	write("The "+_name(o)+" has no value.\n");
      else 
      {
	found = 1;
	if (o->query_wet())  value -= value/10;
	write("You would get "+ value +" gold coin"+
	      (value != 1 ? "s" : "") +" for the "+_name(o)+".\n");
      }
    }
  }
  if(!found)
  {
    if(call_other("room/store", "value", item))
      return 1;
    return notify_fail("No such item ("+item+") here or in the store.\n"), 0;
  }
  return 1;
}

int buy(string item)
{
  if (!item) return notify_fail("Buy what?\n"),0;
  "room/store" -> buy(item);
  return 1;
}

int north()
{
  if (this_player()->query_level()< 20)
  {
    write("A strong magic force stops you.\n");
    say(this_player()->query_name()+
	" tries to go through the field, but fails.\n");
    return 1;
  }
  write("You wriggle through the force field...\n");
  this_player()->move_player("north#room/store");
  return 1;
}

int list(string obj)
{
  "room/store"->inventory(obj);
  return 1;
}

test_ill(x, i) {    /* written by chrisp */
/* Some changes of values by Brom 940403. */
int wc, value, weight, cl;
string sh, cr, fn, at, out, rep;

  cl = x -> armour_class();
  wc = x -> weapon_class();
  value = x -> query_value();
  weight = x -> query_weight();

  at = x -> query_type();
  cr = x -> query_real_creator();
  if (!cr) cr = creator(x);
  if (!cr) cr = "unknown";
  fn = file_name(x);
  sh = x -> short();
  out = "";

  if (! sh)
    out = out + "Short description missing!\n";

  if (at == "armour" && (cl < 0 || cl > 15 || (cl > 12 && weight < 9))) {
    out = out + "class: " + cl + "\n";
    rep = rep + "class: " + cl + " (recommendation: max 15)\n";
    if (cl > 12 && weight < 9) {
      out = out + "weight: " + weight + "\n";
      rep = rep + "weight: " + weight + " (recommendation: min 12)\n";
    }
  }
 
  if (at && at != "armour" && cl>0) {
    if (cl > 3) {
      out = out + "class: " + cl + "\n";
      rep = rep + "class: " + cl + " (recommendation: max 3)\n";
    }
    if (at != "shield" && at != "helmet" && at != "boot" && at != "glove" &&
        at != "cloak" && at != "ring" && at != "amulet") {
      out = out + "type: " + at + "\n";
      rep = rep + 
        "type: " + at + 
        " (Ok: shield, helmet, boot, glove, cloak, ring, amulet)\n";
    }
  }

  if (wc > 19) {
    out = out + "wc : " + wc + "\n";
    rep = rep + "wc : " + wc + " (recommendation: max 20)\n";
  }

  if (value > 5000 || value < 0) {
    out = out + "value: " + value + "\n";
    if ((cl || wc) && value <= 0)
      rep = rep + "value " +
        value + " on combat item. (recommendation in the rules)\n";
    else
      rep = rep + "value: " + value + " (recommendation: max 5000)\n";
  }

  if (weight < 0) {
    out = out + "weight: " + weight + "\n";
    if (wc || cl || at || weight < 0)
      rep = rep + "weight: " + weight + "\n";
  }
  if (out!="") {
    rep = "*** Possible bug in '" + sh + "' on file '" + fn + "'\n" + rep;
    out = "*** " + sh + "; " + fn + " (" + cr + ")\n" + out;
    write_file("/log/SECURE/ILL_OBJ", ctime(time())+" "+out);
    if (i == 1 && creator(x) == cr) log_file(cr + ".rep", rep);
    return 1;
  }
  return 0;
}

query_prevent_shadow() { return "Profezzorn was here."; }

