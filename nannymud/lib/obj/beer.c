/* /obj/beer.c - A bottle of beer */

inherit "obj/drink";

reset(arg)
{
  ::reset(arg);
  
  if (arg)
    return;

  name       = "beer";
  short_desc = "a bottle of beer";
  message    = "That feels good";
  heal       = 0;
  value      = 12;
  strength   = 2;
}
