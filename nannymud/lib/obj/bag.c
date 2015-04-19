/* /obj/bag.c - a generic bag object */

inherit "/obj/container"; /* Now you can look at it without puking.
			     Wing 920821*/

void reset(int arg)
{
  ::reset(arg);
  if (arg) return;
  set_wet_desc("A wet bag.\n");
  set_name( "bag" );
  set_short( "a bag" );
  set_weight(1);
  set_max_weight( 6 );
  set_value( 12 );
}

void prevent_program_throw() {}
