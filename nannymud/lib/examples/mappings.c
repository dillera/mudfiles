/* The forest */

inherit "/std/room";
mapping persons;
reset(arg) {
  ::reset(arg);
  persons=([]);  /* Empty the mapping of persons every reset so you can get one every reset. */
  if (arg) return;

  add_property("indoors");
  set_light(1);
  add_exit("north","room7");

  set_short("Forest");
  set_long(
    "This is a simple example of using mappings.\n" +
    "Every reset every person can get one candy.\n"+
    "To keep track of which ones who has gotten a candy we store them in a mapping.\n"+
    "There is a big lever here and beside it on the wall is a sign.\n");


  add_item("lever",
    "There is a big lever in the wall and beside it is a sign.\n");
  add_item("sign",
    "The sign is nailed to the wall just beside the lever.\n");
  add_item_cmd("pull","lever","@pull_lever()");
  add_item_cmd("read","sign",
	       "The sign says: Pull the lever to get a candy.\n");
} /* reset */


pull_lever() {
  string name;
  name=this_player()->query_real_name();
  if(persons[name]) { 
    /* This person is already in the mapping */
    write("You have already gotten a candy.\n");
    return 1;
  }
  make_candy();
  write("As you pull the lever a candy appears on the floor.\n");
  persons[name]=1; /* Add this person to the mapping. */
  return 1;
}

make_candy() {
  object candy;
  candy=clone_object("obj/food");
  candy->set_name("candy");
  candy->set_short("A candy");
  candy->set_long("A piece of tasty looking candy.\n");
  candy->set_value(3);
  candy->set_weight(1);
  candy->set_strength(2);
  candy->set_heal(1); 
  candy->set_eater_mess("Mmmm. This tasted sweet.\n");
  candy->set_eating_mess(" eats a piece of candy.\n");
  move_object(candy,this_object());
}
