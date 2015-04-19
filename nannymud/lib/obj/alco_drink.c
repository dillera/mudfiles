/*
  
   This object is a standard alco_drink object and works
   like /obj/food.c or /obj/armour.c
  
   To use this you can do:
     inherit "obj/alco_drink";
   ......
   or,
   object ob;
   ob = clone_object("obj/alco_drink");
   ob->set_name("Apple Cider");
  
*  For more documentation look at /doc/build/drinks


   These functions are defined:

	   set_name(string)	To set the name of the item. For use in id().

   Two alternative names can be set with the calls:

	   set_alias(string) and set_alt_name(string)

	   set_short(string)	To set the short description.

	   set_long(string)	To set the long description.

	   set_value(int)	To set the value of the item.

	   set_weight(int)	To set the weight of the item.

	   set_strength(int)	To set the intoxination power of the item. 
                                If you don't wish the item to have intoxination
				powers just set this value to 0.

	   set_heal(int)	To set the instant healing power of the item. 
                                If you don't wish the item to have instant
				healing powers just set this value to 0.

	   set_drinker_mess(string)
				To set the message that is written to the 
				player when he drinks the item.

	   set_drinking_mess(string)
				To set the message given to the surrounding
				players when this object is drunk.

	   set_empty_container(string)
				The container of the liquid inside. For
				example "bottle" or "jug".


	For an example of the use of this object, please read:
*	/doc/examples/apple_cider.c

*/

#define TP this_player()
#define NAME TP -> query_name()

inherit "obj/soft_drink";

soft_drink(str) { /* Added by Qqqq */
  return ::drink(str);
  }

drink(str)
{
  if (!(full && str && id(str)))
    return 0;

  if (strength / 2 >= 12 && TP -> query_level() < 10) 
  {
    write("You sputter liquid all over the room.\n");
    say(NAME +" tries a "+ name +" but coughs and sputters\nall over you.\n");
    full = 0;
    return 1;
  }
  
  if (strength / 2 >= 8 && TP -> query_level() < 5) 
  {
    write("You throw it all up.\n");
    say(NAME + " tries to drink a " + name + " but throws up.\n");
    full = 0;
    return 1;
  }

  if(!TP -> drink_alco(strength) && !TP -> query_npc()) 
    /*drink_alco divides the strength/2 in player*/
    return 1;
  
  full = 0;
  TP -> heal_self(heal);
  remove_property("special_item");
  say(NAME +(drinking_mess ? drinking_mess : " drinks "+ short_desc +".\n"));
  return write(drinker_mess), 1;
}
