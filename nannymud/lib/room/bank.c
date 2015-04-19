inherit "/std/room";

int door_is_open, door_is_locked;
object guard;

reset(arg) {
  door_is_open = 0; 
  door_is_locked = 1;
  ::reset(arg);
  if (arg) return;
  set_light(1);
  add_property("indoors");
  add_exit("west","/room/narr_alley");
  add_exit("north","/room/bank/bank");
  set_short("The lobby");
  set_long("You are in the lobby of Aurora's first National Bank.\n" +
	   "The floor is made of white marble and the walls are painted\n"+ 
	   "in a soft blue colour. There is a door leading east.\n");
  add_item(({"floor","marble floor"}), 
	   "A beautiful marble floor. It is made out\n"+ 
	   "of expensive-looking white marble. It must have cost a fortune.\n");
  add_item(({"wall","walls"}),
	   "The walls are made of wood and are painted in a soft blue colour.\n");
  add_item("door","@exa_door()");
  add_item("sign","The sign is made of gold and says: President.\n");
  add_item("wood","You can't make out what kind of wood it is beacuse of all the paint.\n");
  add_item("marble","It is white and expensive.\n");
  add_object("@make_guard()");
  add_item_cmd(({"take","get"}),"sign","The sign is thorougly fastened to the door.\n");
} /* reset */

exa_door() {
  string str;
  str="The looks like it is made of silver. An ornamented sign is attached to it.\n";
  if (door_is_open) str+="The door is open.\n";
  else str+="The door is closed.\n";
  return str;
} /* exa_door */

make_guard() {
  object key, weapon;
  guard = clone_object("std/basic_monster");
  guard->set_name("guard");
  guard->set_level(11);
  guard->set_hp(200);
  guard->set_al(100);
  guard->set_short("A guard");
  guard->set_long("A big and sturdy guard.");
  move_object(guard, this_object());
  weapon = clone_object("std/simple_weapon");
  weapon->set_name("shortsword");
  weapon->set_short("A shortsword");
  weapon->set_alias("sword");
  weapon->set_long("It is proffesional looking short sword, used by warriors and guards");
  weapon->set_class(15);
  weapon->set_value(700);
  weapon->set_weight(3);
  transfer(weapon, guard);
  guard->init_command("wield shortsword");
  key = clone_object("/obj/key");
  key->set_type("bronze");
  key->set_code("bank key");
  transfer(key, guard);
  return guard;
}

	

init() {
  ::init();
  add_action("open", "open");
  add_action("unlock", "unlock");
  add_action("east", "east");
}


open(str) {
  if (str && str != "door")
    return 0;
  if (door_is_open)
    return 0;
  if (door_is_locked) {
    write("The door is locked.\n");
    return 1;
  }
  door_is_open = 1;
  write("Ok.\n");
  say(this_player()->query_name() + " opened the door.\n");
  return 1;
}

unlock(str) {
  object key;
  if (str && str != "door")
    return 0;
  if (door_is_open || !door_is_locked)
    return 0;
  key=present("bronze key",this_player());
  if (!key) {
    write("You haven't got a key!\n");
    return 1;
  }
  if (key->query_code()!="bank key") {
    write("The key doesn't fit!\n");
    return 1;
  }
  door_is_locked = 0;
  write("Ok.\n");
  say(this_player()->query_name() + " unlocked the door.\n");
  return 1;
}

east() {
  if (!door_is_open) {
    write("The door is closed.\n");
    return 1;
  }
  if (guard && present(guard, this_object())) {
    write("The guard bars the way.\n");
    return 1;
  }
  this_player()->move_player("east#room/bankroom");
  return 1;
}

query_door() {
  return !door_is_open;
}

open_door_inside() {
  door_is_locked = 0;
  door_is_open = 1;
}
