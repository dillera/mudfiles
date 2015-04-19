inherit "room/room";

/* 
 * This file may be used to (through inheritance) create a room that is 
 * under water or is a water surface.
 *
 * It may also be used as a normal room. Whether it is waterfilled or not is
 * controlled by using properties: If the property "waterfilled" is set, the
 * room's special behaviour will be activated. This makes it possible to 
 * simulate tides, airlocks, etc... SET THIS PROPERTY USING add_property()
 * in reset().
 *
 * See /room/room.c for a description of what room properties is...
 *
 * Created Dec 1991 by Caution (alias matoh@sssab.se)
 */

string surface;
string bottom;
int drown_time;

/*
 * If the room is entirely under water, give it the name of the surface room
 * with surface(). If the room has no bottom, and the bottom is in a room 
 * below, give the name of that room with set_bottom().
 *
 * If set_surface() is not called, the room is considered a water surface, and
 * there will be no risk of drowning.
 *
 * If set_bottom() is not called, the room is supposed to have its own bottom,
 * and dropped items will not sink.
 *
 * set_drown_time() sets the time after which the player is forced to the
 * surface, and thus rescued from drowning.
 */

set_surface(string str) { surface = str;}
set_bottom(string str)  { bottom  = str;}
set_drown_time(int i)   { drown_time = i;}
query_surface() { return surface;}
query_bottom()  { return bottom;}
query_drown_time() { return drown_time;}

/*
 * Init() checks whether a rescue is needed, and adds a patch for "drop"
 * to make dropped things sink.
 */

init()
{
  if (query_property("waterfilled") && 
      interactive(this_player()) &&    /* Don't force our monsters to leave */
      !check_air(this_player())) {
    call_out("rescue", drown_time, this_player());
  } 

  ::init();			       /* Call /room/room's init */

  add_action("do_drop", "drop");
  add_action("drink_water", "drink");
  sink_search(this_object());
}

drink_water(str)
{
  if (!str || str != "water")
    return 0;

  if (this_player() && this_player()->drink_soft(3))
  {
    write("You drink some water. It doesn't taste that good though.\n");
    say(capitalize(this_player()->query_name()) + " drinks some water.\n");
  }
  return 1;
}

/*
 * Reset() sets a default drown time.
 * Don't forget to call this reset() when you inherit this file!!
 */

reset(int arg)
{
  if (!arg)
    drown_time = 10;
}

/*
 * This patches the player's drop. 
 */

do_drop(string str)
{
  int result;

  result = this_player()->drop_thing(str);
  sink_search(this_object());

  return result;
}

/*
 * Force a player to the surface...
 * The checks from init() are repeated, as circumstances may have changed.
 * Wizards may go where they want.
 */

rescue(object victim)
{

  if (surface &&
      present(victim) &&
      living(victim) &&
      interactive(victim) &&
      victim->query_level() < 20 &&     
      !check_air(victim)) {
    
    tell_object(victim,
		"You are forced to the surface due to a pressing " +
		"lack of breathable air.\n");

    victim->move_player("upwards#" + surface);

    tell_object(victim,
		"You find that you can breathe again.\n");
    say(victim->query_name() + " is gasping for air.\n", victim);
  }
}

/*
 * This function checks whether a player has anything that can help him to
 * breathe. I don't check for containers, as everybody seems to be running
 * around with sacks nowadays...
 *
 * And besides that, I think that a sack or a cup helping you to breathe 
 * is a silly idea. I even have a fishing net that acts as a container.
 */

check_air(object person)
{
  object *all;
  
  person->set_wet(0);  /* Make person wet */

  all = all_inventory(person);
  return sizeof((all -> query_air()) - ({0}));
}

/*
 * Search for things that should sink. Since there is no density defined
 * for objects, we use the draconian method of only defining living objects,
 * invisible objects, and zero weight objects as floating.
 */

sink_search(object loc)
{
  object *inv;
  int i;
  object bot_obj;

  if (!bottom)
    return;

  bot_obj = find_object(bottom);
  if (!bot_obj) {
    call_other(bottom, "short");         /* Make it load if not loaded... */
    bot_obj = find_object(bottom);
    if (!bot_obj)
      return;
  }
  
  inv = all_inventory(loc);

  for (i = 0; i < sizeof(inv); i++) {
    if ((inv[i]->id("money")) || (!living(inv[i]) && 
				  inv[i]->short() &&
				  inv[i]->query_weight() > 0)) {
      tell_room(loc,
		"The "+ inv[i]->query_name() +
		" sinks out of view.\n");
      transfer(inv[i], bottom);
    }
  }
}

/*
 * Call this monster_died() through set_dead_ob() in your monsters to make 
 * the corpse sink when the monster has been killed.
 * 
 * We must use call_out since the monster is not *really* dead yet...
 */

monster_died(object monster)
{
  call_out("sink_search", 1, this_object());
  return 0;			/* Let the monster die... */
}
