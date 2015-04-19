/* monsters2.c
 *
 * A slightly more complex example than monsters1.c
 *
 */

inherit "std/room";

reset(arg)
{
  ::reset(arg); /* You have to have this in order to make the objects return
                   at the next reset. */
  if (!arg)
  {
    add_property("indoors");
    set_light(1);
    set_short("example room");
    set_long(
	     "This is the second room with monsters. Here is a little more\n"+ 
	     "advanced monster. He has a club to fight with and he becomes\n"+ 
	     "aggressive after a while.\n");
  
    add_exit("south","monsters1");
  
    add_item( ({ "walls", "stone walls", "wall", "stone wall" }),
	     "The walls are made of stone and are bare.");
    add_item( ({ "floor", "wooden floor" }),
	     "The floor is made of rough wooden planks.");
    add_item( "ceiling",
	     "There are some spiderwebs in the ceiling.");
    add_item( ({ "spiderweb", "spiderwebs", "web", "webs" }),
	     "The spiderwebs in the ceiling are too high up for you to reach.");

    add_object("@make_orc()");
  }
} /* reset */


make_orc()
{
  object orc, club;

  orc = clone_object("/std/basic_monster");
  orc -> set_name("orc");
  orc -> set_level(8);
  orc -> set_race("orc");
  orc -> set_short("An orc");
  orc -> set_long("This is a foul smelling orc.\n");
  orc -> set_male();

  /* Orcs are agressive by nature. But he is so stupid that it takes 10 seconds 
   * to notice that you are here.
   */
  orc -> set_aggressive(10); 


  /* The orc needs a weapon to fight with. */
  club = clone_object("/std/simple_weapon");
  club -> set_name("club");
  club -> set_value(80);
  club -> set_weight(2);
  club -> set_class(10);
  club -> set_short("A club");
  club -> set_long("It is a medium sized wooden club.\n");
  club -> set_type("crush"); 
  club -> add_property("wooden");
  transfer(club, orc);

  /* We want the orc to wield the club. */
  command("wield club", orc);

  return orc;
} /* make_orc */
