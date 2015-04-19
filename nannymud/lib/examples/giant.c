/* This monster is a giant who likes killing people with his bare fists,
   and who therefore has an unusual type of damage messages. */

inherit "/std/simple_monster"; 

reset(arg) {
  ::reset(arg);                       /* If you forget this, strange things
					 might happen when a monster resets */
  if(arg) return;                     /* Everything below here is just done
					 when the monster is first created,
					 not at a normal reset. */

  set_name("Skullsmasher the giant"); /* Notice that there is no ending '.' */
  add_alias("giant");                 /* Alternative names for the monster */
  add_alias("skullsmasher");
  set_level(14);
  set_hp(200+random(50));             /* We want the hp to be a bit random */
  set_al(-200);                       /* He's an evil giant */
  set_short("Skullsmasher of the great fists"); /* No ending '.' here either */

  
  /* You don't need to put in \n to break lines in the set_long and add_item
     unless you explicitly want the breaks at certain places. If you don't put
     them in, the description will automatically be made to fit onto the
     screen. */
  
  set_long("A giant surely over twelve feet tall. He is clad in raggy "+
	   "furs, and his hands are large even for a giant.");
  add_item("fists",
	   "The giant's fists look like they could smash human skulls like "+
	   "eggs.");
  set_male();

  
  /* This makes the giant use the 'hand-to-hand' damage type messages instead
     of the default 'You hit, you tickled' ones. */
  
  add_property("damage_type","hand-to-hand");
  make_furs();                        /* We create the giant's furs */
  set_wc(18);                         /* Always put these last, so the wearing
					 or wielding of things don't mess the
					 numbers up */
  set_ac(8);
}

/* This is a standard armour. */
make_furs() {
  object furs;
  furs = clone_object("/std/simple_armour");
  furs->set_name("Raggy fur jacket");
  furs->add_alias("jacket");
  furs->add_alias("furs");
  furs->add_alias("fur jacket");
  furs->add_alias("fur");
  furs->set_class(6);
  furs->set_weight(5);
  furs->set_type("armour");
  furs->set_value(200);
  furs->set_short("A huge fur jacket");
  furs->set_long("Several raggy furs, sewn together into a huge jacket.");
  furs->add_property("leather");
  furs->add_property("burnable");
  transfer(furs,this_object());
  command("wear furs",this_object()); /* Make him wear the furs, too */
}

  
