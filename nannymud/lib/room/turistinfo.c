#include <turistinfo.h>

void my_say( mixed a, mixed b )
{ tell_room( this_object(), LINE_WRAP(a), b ); }

inherit "/room/room";

object clerk;

#define CLERK if (!(clerk && present( clerk, this_object() ))) \
		if (!(clerk = present( CLERK_ID, this_object() ))) \
		  move_object( clerk = clone_object( CLERK_OBJ ), \
			       this_object() )

create()
{
  dest_dir = ({ "/room/new/narr_alley2", "west" });
  short_desc = "Tourist information";
  long_desc = "This is Nannymud's tourist information office. Behind a \n"+
              "counter the room is filled with shelves containing booklets\n"+
	      "describing everybody's areas in this wonderful place.\n" +
	      "  On a wall there is a large map over Nannymud.\n";
  items = ({ "counter", "An counter, behind which the clerk is standing",
	     ({ "booklets", "booklet" }),
	       "You can't reach them from this side of the counter, \n"+
	       "but the clerk seems eager to show them to you.\n"+
	       "You just have to mention which wizard you want \n"+
	       "information about",
	     "map","A map over Nannymud"});
  CLERK;
  set_light(1);
}

void reset( int flag )
{
  if (!flag)
    create();
}

void init()
{
  CLERK;
  if (this_player() == clerk)
    return;
  add_action( "read_map", "read" );
  add_action( "climb_counter", "climb" );
  ::init();
}

status climb_counter( string str )
{
  if (!stringp( str ))
    return 0;
  if (lower_case( str ) == "counter")
  {
    CLERK;
    SAY( this_player()->query_name() + " tries to climb the counter but " +
	 "the clerk stops him.", ({ clerk, this_player() }) );
    TELL( this_player(), "The clerk stops you." );
    SAY( "The clerk says: " + this_player()->query_name() + " you ought to " +
	 "be ashamed.", ({ clerk }) );
    return 1;
  }
  return 0;
}

status read_map( string str )
{
  if (!stringp( str ) || lower_case( str ) != "map")
    return 0;
  TELL( this_player(), "You look at the map and try to comprehend it, but " +
	"fail miserably." );
  CLERK;
  SAY( this_player()->query_name() + " looks confused.",
       ({ clerk, this_player() }) );
  return 1;
}
/* sigh this is causing me endless grief 
string query_long( string str )
{
  if (stringp( str ) && lower_case(str) == "map")
    read_map( str );
  else
    ::query_long( str );
}
*/
void long( string str )
{
  if (stringp( str ) && lower_case(str) == "map")
    read_map( str );
  else
    ::long( str );
}

void exit_dangerous()
{
  object booklet, b2;
  mixed inv;
  int c;
  string tmp;
  
  CLERK;
  if (call_other( clerk, "under_attack"))
    return;
  if (!objectp(this_player())) return; /* Brom 960113 */
  inv = deep_inventory( this_player() );
  for (c=0; c < sizeof( inv ); c++)
    if (inv[c]->id( BOOKLET_ID ))
    {
      if (tmp)
	tmp = "some booklets";
      else
	tmp = (string )inv[c]->short();
      destruct( inv[c] );
    }
  if (!tmp)
    return;
  SAY( "The clerk swiftly jumps over the counter.", ({ clerk }) );
  SAY( "The clerk takes " + tmp + " from " +
       this_player()->query_name() , ({ clerk, this_player() }) );
  TELL( this_player(), "The clerk takes " + tmp + " from you.", );
  SAY( "The clerk says: " + this_player()->query_name() + ", you ought to " +
       "be ashamed.\n" +
       "The clerk jumps back across the counter.\n" +
       "The clerk returns " + tmp +
       (tmp == "some booklets" ? " to their shelves." :
	" to it's shelf."), ({ clerk }) );
  if (WIZARDP(this_player()->query_real_name()))
    call_other( clerk, "careless_pp" );
}

exit()
{
  string err_msg;

  if (err_msg = catch( exit_dangerous() ) )
    log_file( "turist.bug", err_msg );
}

status move( string str )
{
  object booklet, *inv;
  int c;

  CLERK;
  if (call_other(clerk, "under_attack"))
    return ::move( str );
  if (!objectp(this_player())) return 0; /* Brom 960113 */
  inv = deep_inventory( this_player() );
  for (c=0; c < sizeof( inv ); c++)
    if (inv[c]->id( BOOKLET_ID ))
    {
      SAY( "The clerk says: " + this_player()->query_name() + ", you must " +
	   "return the booklet first.", ({ clerk }) );
      return 1;
    }
  return ::move( str );
}

int inorout=1;
