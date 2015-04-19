inherit "room/room";
object the_monster;

reset(arg) {
  object the_hammer;

  if (!present("hammer", this_object())) {
    the_hammer = clone_object("obj/weapon");
    the_hammer->set_name("hammer");
    the_hammer->set_alias("heavy hammer");
    the_hammer->set_short("a heavy hammer");
    the_hammer->set_long("This is a heavy hammer made of iron.\n" +
                         "It is old and rusty, but it can probably\n" +
                         "be moderately useful as a weapon.\n");
    the_hammer->set_class(6);
    the_hammer->set_value(2);
    the_hammer->set_weight(3);
    move_object(the_hammer, this_object());
  }

  if (!the_monster) {
    the_monster = clone_object("obj/monster");
    move_object(the_monster, this_object());
    the_monster->set_name("monster");
    the_monster->set_alias("small monster");
    the_monster->set_short("a small monster");
    the_monster->set_long(
      "This is a disgusting little monster with red, bulging eyes,\n" +
      "slimy green skin, and a wide, ugly mouth full of pointed teeth.\n" +
      "Monsters like this have no business in this nice example room.\n" +
      "It looks dangerous, so you'd better smash it to pieces\n" +
      "before things turn violent!\n");
    the_monster->set_level(2);
    the_monster->set_al(-50);
    /* Thick skin: higher armour class than usual for level 2! */
    the_monster->set_ac(2);
    the_monster->add_money(5);
    /* Give the disgusting luttle creep a weapon so it can defend itself: */
    transfer(clone_object("standard/weapons/knife"), the_monster);
    the_monster->init_command("wield knife");
    /* Strong arms: higher weapon class than usual for the knife! */
    the_monster->set_wc(10);
  }

  if (arg)
    return;

  set_indoors();
  set_light(1);
  add_exit("players/anna/examples/room7", "west");
  add_exit("players/anna/examples/room9", "east");
  set_short("Example room 8");
  set_long(
    "In this example room, number 8, there is a hammer\n" +
    "and a small monster that you can fight if you want to.\n");

} /* reset */




