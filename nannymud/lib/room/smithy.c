inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

mapping inventory;

/* number field is the number of items sold per reset */
void refresh() {
  inventory = ([
    "dagger" : ([ "number" : 5, "name" : "dagger",
                  "short" : "a dagger",
                  "long" : "A short but sharp-bladed weapon.\n",
                  "weight" : 2,
                  "class" : 6,
                  "value" : 12,
                  "file" : "/obj/weapon",
               ]),
    "shortsword" : ([ "number" : 3, "name" : "shortsword",
                  "short" : "a shortsword",
                  "alt_name" : "sword",
                  "alias" : "short sword",
                  "long" : "A plain shortsword.\n",
                  "value" : 250,
                  "class" : 11,
                  "weight" : 4,
                  "file" : "/obj/weapon",
               ]),
    "longsword" : ([ "number" : 2, "name" : "longsword",
                  "short" : "a longsword",  
                  "alias" : "sword",
                  "alt_name" : "long sword",
                  "long" : "An unadorned longsword.\n",
                  "class" : 14,
                  "value" : 600,
                  "weight" : 5,
                  "file" : "/obj/weapon",
               ]),
    "chainmail" : ([ "number" : 4, "name" : "chainmail",
                  "alias" : "mail", "type" : "armour",
                  "weight" : 6,
                  "short" : "a suit of chainmail", 
                  "long" : "A suit of finely-woven metal links.\n",
                  "class" : 9,
                  "value" : 500,
                  "file" : "/obj/armour",
                ]),
    "platemail" : ([ "number" : 2, "name" : "platemail",
                  "alias" : "suit", "type" : "armour",
                  "class" : 12, "weight" : 9,
                  "value" : 1400,
                  "short" : "a suit of platemail",
                  "long" : "A full-body suit of armour.\n",
                  "file" : "/obj/armour",
                ]),
    "shield" : ([ "number" : 5, "name" : "medium shield",
                  "alias" : "shield", "type" : "shield",
                  "class" : 2, "weight" : 2,
                  "value" : 150,
                  "short" : "a medium shield",
                  "long" : "A round metal shield.\n",
                  "file" : "/obj/armour",
               ]),
    "helmet" : ([ "number" : 5, "name" : "metal helmet",
                  "alias" : "helmet", "type" : "helmet",
                  "weight" : 2, "class" : 2,
                  "value" : 135,
                  "short" : "a metal helmet",
                  "long" : "A plain metal helmet, to protect your noggin'.\n",
                  "file" : "/obj/armour",
                ]),
  ]);
}

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("inside");
    
    add_exit("east", "/room/narr_alley");

    set_short("The Village Smithy");
    set_long(LB("This is the workplace of the village blacksmith. "+
		"The heat from the furnace in the rear of the shop makes you "+
		"perspire profusely. In addition to normal metal work, "+
		"this shop serves as an armoury, where you can buy weapons "+
		"and armour to aid you in your adventures. A sign on one wall "+
		"lists what you can buy here. "));
    add_item("sign", "You can read it.\n");
    add_item("furnace", "Enormous amounts of heat radiate from it.\n");
  }
  refresh();
}

void init() {
  ::init();
  add_action("read", "read");
  add_action("list", "list");
  add_action("buy", "buy");
  add_action("buy", "purchase");
}

string list_item(string n) {
  return sprintf("%-20s  %4d coins\n", capitalize(n), inventory[n]["value"]*2);
}

int get_stock(mapping m) {
  return m["number"];
}

int sort_items_by_price(string a, string b) {
  return inventory[a]["value"] > inventory[b]["value"];
}


string read_sign() {
  mapping what_is_here;
  string *my_items, txt;
  int i;

  what_is_here = filter_mapping(inventory, "get_stock", this_object());
  if( !sizeof(my_items = m_indices(what_is_here)) )
    return "The smithy is currently sold out of everything.";
  my_items = sort_array(my_items, "sort_items_by_price", this_object());
  for( txt = sprintf("%-20s %s\n", "Item", "Price"); i<sizeof(my_items); i++ )
    txt += list_item(my_items[i]);
  return txt + 
    "Simply, \"buy [item]\" to purchase something. \n";
    
}
  
int buy(string str) {
  mapping what;
  int i;
  string *func;
  object o;

  if( !str ) {
    notify_fail("Buy what?\n");
    return 0;
  }

  str = lower_case(str);

  if( !(what=inventory[str]) ) 
    return write("But we don't sell "+str+"s here!\n"), 1;

  if( !what["number"] )
    return write("But we are sold out of "+str+"s!\n"), 1;

  if( (int)this_player()->query_money() < what["value"] * 2 )
    return
	write("You can't afford it!  It costs "+what["value"]*2+" coins!\n"), 1;

  inventory[str]["number"]--;

  write("You hand "+what["value"]*2+" coins over, and buy the "+str+".\n");
  say(this_player()->query_name()+" buys "+str+".\n");

  this_player()->add_money(-2*what["value"]);

  for( 	o=clone_object(what["file"]), func=m_indices(what); 
	i<sizeof(func); i++ )
    if( func[i] != "file" && func[i] != "number" )
      call_other(o, "set_"+func[i], what[func[i]]);
  
  if( transfer(o, this_player()) ) {
    write("You cannot carry any more. The "+what["name"]+" is laid\n"+
	  "on the ground in front of you.\n");
    move_object(o, this_object());
  }

  return 1;
}

int list() {
  write(read_sign());
  return 1;
}

int read(string str) {
  if( str=="sign" || str=="prices" || str=="price" ) 
	return write(read_sign()), 1;
  notify_fail("Read what?\n");
}

