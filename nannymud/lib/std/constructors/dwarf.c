object make()
{
  object o;
  mixed arg;

  o=clone_object("/obj/monster");
  o->set_name("dwarf");
  o->set_alt_name(20);
  o->set_level(random(8)+4);
  o->set_aggressive(0);
  o->set_short("a dwarf (small)");
  arg=random(2);
  o->set_gender(arg?1:2);
  o->set_long((arg?"He":"She")+" looks like any dwarf. Small.\n");
  o->set_spell_mess1("The dwarf hit you in the head with "+(arg?"his":"her")+" hammer\n");
  o->load_chat(4,({"Dwarf sings: Hi-ho hi-ho hi-ho hi-ho hi-ho hi-ho, hi-ho hi-ho\nhi-ho hi-ho hi-ho hi-ho hi-ho, hi-ho hi-ho hi-ho hi-ho hi-ho hi-ho\n",
		   "Dwarf sings: gold gold, gold gold gold, gold gold gold gold gold gold, gold\n"}));
  o->set_spell_mess2("");
  o->set_spell_dam(10);
  o->set_spell_chance(10);
  o->set_money(random(10));
  transfer("/std/lib"->make("hammer"),o);
  transfer("/std/lib"->make("helmet"),o);
  transfer("/std/lib"->make("shield"),o);
  command("wield hammer",o);
  command("wear all",o);
  return o;
}
