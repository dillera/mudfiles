/* A thief, thanks to Bive */

object make()
{
   object thief;

   thief=clone_object("/obj/monster");
   thief->set_name("thief");
   thief->set_race("gnome");
   thief->set_short("a little ugly thief");
   thief->set_long("A small gnome with long fingers.\n");
   thief->set_level(random(3) + 3);
   thief->set_gender(1+random(2));

   switch(random(5))
   {
     case 0:
       transfer("/std/lib"->make("box"), thief);
       break;
     
     case 1:
       transfer("/std/lib"->make("chalk"), thief);
       break;

     case 2:
       transfer("/std/lib"->make("diamond"), thief);
       break;
    
     case 3:
       transfer("/std/lib"->make("emerald"), thief);
       break;
 
     case 4:
       transfer("/std/lib"->make("ruby"), thief);
       break;
   }

   return thief;
}
