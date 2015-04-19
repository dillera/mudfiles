inherit "/std/rfc/complex_room";

void reset(int arg)
{
  object o;
  ::reset(arg);

  if(!arg)
  {
    set_light(1);
    set_short("A testroom");
    set_long("You are in the testroom.\n");
    
    add_exit("south",this_object());    
    add_exit("west",this_object());

#if 0
    add_object("a dwarf");
    add_object("a dwarf");
    add_object("a dwarf");

    add_object("a sword");
    add_object("a sword");

#endif
    add_object("a dagger");

    add_object("/std/rfc/demo/testmonster");

#if 0
    o=clone_object("/std/rfc/simple_monster");
    o->set_name("gober");
    o->set_short("Gober the simple monster");
    o->add_alias(({"simple monster","monster"}));
    o->set_level(20);
    o->set_gender(1);
    o->set_long("Gober has a stupid look on his face.\n");
    o->add_item("face","It is a particularly stupid-looking face.\n");
    o->add_chat("Gober says: I'm a simple monster!\n");
    o->add_chat("Gober sings: My Bonny is over the ocean,\n"+
		"Gober sings: My Bonny is over the sea,\n"+
		"Gober sings: My Bonny is over the ocean,\n"+
		"Gober sings: So bring back my Bonny to me,\n"+
		"Gober signs: Bring, Back, Bring, Back\n"+
		"Gober sings: So bring back my Bonny to me, to me,\n"+
		"Gober signs: Bring, Back, Bring, Back\n"+
		"Gober sings: So bring back my Bonny to me!\n");
    o->add_chat("Gober says: Slap me silly!\n");
    o->set_chat_chance(10);

    o->add_attack("\bPRON slap\b$ \b2OBJ silly.\n",90,5,0,0);
    o->add_attack("\bPRON spank\b$ \b2OBJ silly.\n",90,5,0,0);
    o->add_attack("\bPRON flerp\b$ \b2OBJ silly.\n",90,5,0,0);
    o->start_walking();
    add_object(o);
#endif
  }
}
