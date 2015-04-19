/* Originally made by Beldin */
object make()
{
  object o;
  string color;
  o=clone_object("/obj/monster");
  o->set_name("small bat");
  switch(random(3))
  {
   case 0: color = "grey"; break;
   case 1: color = "brown"; break;
   case 2: color = "black"; break;
  }
  o->set_short("a small " + color + " bat");
  o->set_long("This is a small " + color + " bat with leathery wings.\n");
  o->set_level(1);
  o->set_race("bat");
  o->set_hp(10 + random(20) + random(20));
  o->set_wc(2 + random(3));
  o->set_ac(random(3));
  o->set_whimpy();
  o->set_al(-20 + random(41));
  return o;
}
