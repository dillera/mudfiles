/* A Cat for /std/lib.c, made by Bive 950716 */

object make() {

   object cat;
   cat=clone_object("/obj/smartmonster");
   cat->set_name("cat");
   cat->set_race("cat");
   cat->set_short("a cat");
   cat->set_long("A cute cat.\n");
   cat->set_level(random(2) +2);
   cat->set_gender(1+random(2));
   cat->set_wimpyness(20);
   return cat;
}




