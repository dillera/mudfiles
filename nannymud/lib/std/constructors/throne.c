/* A Throne for /std/lib.c, made by Bive 950716 */

object make() { 

  object throne;
  throne=clone_object("/std/basic_thing");
  throne->set_name("throne");
  throne->set_value(1);
  throne->set_short("a throne");
  throne->set_long("The throne is made of gold.\n");
  throne->set_weight(100);
  throne->set_property("gold");
  return throne;
 
}

