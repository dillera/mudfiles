inherit "std/room";

int diamond;
reset(arg) {
  if (arg) return;

  add_property("indoors");
  set_light(1);
  add_exit("west","room6");
  add_exit("north","forest");
  add_exit("south","mappings");
  add_exit("east","room8");
  set_short("Example room 7");
  set_long(
    "This is the seventh room in a series of examples of LPC code.\n"+
    "It this room remove_item and remove_item_cmd is demonstrated.\n"+
    "On the wall there is a painting. Try to move it.\n");

  add_item("painting","It is a beautiful painting.");
  add_item_cmd("move","painting","@move_painting()");
} /* reset */

move_painting() {
  write("You move the painting. Behind the painting is a compartment.\n");

  /* First remove some old stuff. */
  remove_item("painting");
  remove_item_cmd("painting");

  /* Then add some new items. */
  add_item("painting",
    "The painting is moved aside, showing a little compartment.");
  add_item("compartment","The compartment contains a diamond.");
  add_item("diamond",
    "There is a little sparkling diamond inside the compartment.\n");

  /* Add some things to do with the new items. */
  add_item_cmd("empty","compartment","@get_diamond()");
  add_item_cmd(({"get","take"}),"diamond","@get_diamond()");

  return 1;
} /* move_painting */


get_diamond() {
object treasure;
  treasure=make_diamond();

  /* If we cant carry the diamond we still want it to be in this room. */
  move_object(treasure,this_object()); 

  write("You take the diamond out of the compartment.\n");

  /* Use transfer to move the diamond to the player, otherwise he will 
     not get the weight from it. 
     transfer returns 0 if there was no problem */
  if (transfer(treasure,this_player())) 
    write("Ooops, you dropped the diamond on the ground.\n");
  
  /* Now, remove some old stuff. */
  remove_item("diamond");
  remove_item("compartment");
  remove_item_cmd("empty","compartment");
  remove_item_cmd(({"get","take"}),"diamond");
  
  /* And add some noew descriptions */
  add_item("compartment","The compartment is empty.");

  return 1;
} /* get_diamond */


make_diamond() {
object thing;
  thing=clone_object("/std/basic_thing");
  thing->set_name("diamond");
  thing->add_alias("jewel");
  thing->set_short("A diamond");
  thing->set_long("A little sparkling diamond.\n");
  thing->set_weight(1);
  thing->set_value(100+random(150));
  return thing;
} /* make_diamond */
