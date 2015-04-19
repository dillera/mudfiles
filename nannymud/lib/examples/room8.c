inherit "std/room";

int moved_painting, gotten_diamond;
reset(arg) {
  if (!arg) { /* First time */
    set_up_painting();
  }
  if (moved_painting && !gotten_diamond) {
    /* The painting is moved but the diamond is still there. */
    remove_diamond();
    remove_item("painting");
    set_up_painting();
  }
  if (gotten_diamond) {
    /* They got the diamond. */
    remove_item("compartment");
    remove_item("painting");
    set_up_painting();
  }

  if (arg) return;

  add_property("indoors");
  set_light(1);
  add_exit("west","room7");
  set_short("Example room 8");
  set_long(
    "This is the 8th room in a series of examples of LPC code.\n"+
    "In room seven we had a painting that could be moved and a diamond\n"+
    "that could be taken, in this room we show how to make it reappear\n"+
    "every reset.\n"+
    "On the wall there is a painting. Try to move it.\n");

} /* reset */

set_up_painting() {
  moved_painting=0;
  gotten_diamond=0;
  add_item("painting","It is a beautiful painting.");
  add_item_cmd("move","painting","@move_painting()");
} /* set_up_painting */

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

  moved_painting=1;
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
  remove_diamond();
  
  /* And add some noew descriptions */
  add_item("compartment","The compartment is empty.");

  gotten_diamond=1;
  return 1;
} /* get_diamond */

remove_diamond() {
  remove_item("diamond");
  remove_item("compartment");
  remove_item_cmd("empty","compartment");
  remove_item_cmd(({"get","take"}),"diamond");
} /* remove_diamond */



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
