object make()
{
  object o;

  o=clone_object("/obj/monster");
  o->set_race("snail");
  o->set_name("snail");
  o->set_alt_name(0);
  o->set_level(2);
  o->set_wc(0);
  o->set_al(0);
  o->set_aggressive(0);
  o->set_short("a snail (slippery)");
  o->set_long("It is very white, and looks blind, just as\nif it had lived in caves all its life.\n");
  o->set_gender(0);
  return o;
}
