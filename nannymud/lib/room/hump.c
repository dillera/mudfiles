/* inherit "/std/room"; */
inherit "/room/extra/time_room";

void reset(int arg) {  
  object purse, money, stick;

  if( !arg ) {
    time_item_setup();
    set_light(1);
    add_property("realm", "village");
    add_property("outdoors");
  
    add_exit("east", "/room/vill_green");
    add_exit("west", "/room/wild1");
  
    set_short("Hump-backed bridge");
    set_long("You are on an old stone bridge which arches over a calm, "+
      "slow river.  This marks the western edge of the town; beyond is "+
      "wilderness, danger, and adventure.  The village church "+
      "lies directly east and north, a safe haven for all.\n");
    add_item(({ "bridge", "stone bridge", "old bridge", "hump" }),
        "The bridge arches to a height of about 15 feet over the river in "+
        "the center.  It is about 40 feet long, and is made of stone which "+
        "is well-worn from travel across it, but still quite solid.\n");
    add_item(({ "river", "calm river", "slow river", "water" }),
        "The river is about 25 feet wide at this point.  It is flowing "+
        "southward, burbling gently.  The water is very dark and probably "+
        "pretty cold.\n");
  }
  if( !present("purse") ) {
    purse = clone_object("/std/simple_container");
    purse->set_name("leather purse");
    purse->add_alias("purse");
    purse->set_short("a leather purse");
    purse->set_long("A leather purse that someone must have lost.\n");
    purse->set_weight(1);
    purse->set_max_weight(4);
    purse->set_value(8);
    tell_room(this_object(), "A man walking buy drops a small leather "+
	"purse by the bridge.\n");
    move_object(purse, this_object());

    money = clone_object("/obj/money");
    money->set_money(15);
    move_object(money, purse);
  }
  if( !present("stick") ) {
    stick = clone_object("/obj/torch");
    stick->set_name("stick");
    stick->set_weight(1);
    stick->set_fuel(500);
    move_object(stick, this_object());
  }
  if( !present("purse") ) {
    purse = clone_object("/std/container");
    purse->set_name("leather purse");
    purse->add_alias("purse");
    purse->set_short("a leather purse");
    purse->set_long("A leather purse that someone must have lost.\n");
    purse->set_weight(1);
    purse->set_max_weight(4);
    purse->set_value(8);
    tell_room(this_object(), "A man walking buy drops a small leather "+
	"purse by the bridge.\n");
    move_object(purse, this_object());

    money = clone_object("/obj/money");
    money->set_value(15);
    move_object(money, purse);
  }
  if( !present("stick") ) {
    stick = clone_object("/obj/torch");
    stick->set_name("stick");
    stick->set_weight(1);
    stick->set_fuel(500);
    move_object(stick, this_object());
  }
}

