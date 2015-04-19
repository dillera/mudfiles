/* /obj/sack.c - a bigger bag. */

inherit "/obj/container";

void reset( int arg )
{
  if (arg)
    return;
  ::reset();
  set_wet_desc( "A wet sack.\n" );
  set_name( "sack" );
  set_short( "a sack" );
  set_weight(5);
  set_max_weight( 20 );
  set_value( 18 );
}
void prevent_program_throw() {}
