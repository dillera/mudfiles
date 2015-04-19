#include <turistinfo.h>

#define SAVE_FILE "etc/turistinformator"

void my_say( mixed a, mixed b )
{ tell_room( environment(), LINE_WRAP(a), b ); }

#define BUSY (find_call_out( "find_wizard" ) != -1 \
	      || find_call_out( "list_wizards" ) != -1 \
	      || find_call_out( "continue_listing_wizards" ) != -1 \
	      || UNDER_ATTACK)
      

#define UNDER_ATTACK (find_call_out( "pulling_lever" ) != -1)

static string *visitors;
static mixed *borrowed;
mixed careless_wizards;

void create()
{
  visitors = ({ });
  borrowed = ({ });
  careless_wizards = ([ ]);
  enable_commands();
  set_living_name( "clerk" );
  restore_object( SAVE_FILE );
}

void reset( int flag )
{
  if (!flag)
    create();
}

void init()
{
  if (this_player()->query_npc())
    return;
  if (member_array( this_player()->query_name(), visitors ) != -1)
    call_out( "greet", 0 );
  else
  {
    call_out( "greet_long", 0 );
    visitors += ({ this_player()->query_name() });
  }
  add_action( "tell_kludge", "tell" );
}

void greet()
{
  SAY( "The clerk says: Welcome back " + this_player()->query_name() +
       ".", ({ this_object() }) );
}

void greet_long()
{
  SAY( "The clerk says: Welcome " + this_player()->query_name() + ". I'll " +
       "help you find out what interesting places you may visit in this " +
       "mud. I have information on what our hard-working wizards have " +
       "created so just name one and I'll tell you.\n"
       + "The clerk smiles happily.", ({ this_object() }) );
}

void be_quiet()
{
  SAY( "The clerk says: Be quiet, people are trying to read in here.",
      ({ this_object() }) );
}

void check_for_wizard( string str )
{
  string *subject;
  mixed tmp;

  if (!stringp( str ))
    return;
  if (BUSY)
  {
    call_out( "mumble", 0 );
    return;
  }
  if (str == "wing")
  {
    SAY( "The clerk sighs deeply.\n" +
	 "The clerk says: That guy never creates anything useful, I really " +
	 "don't understand why you would want to borrow a booklet about him.",
	 ({ this_object() }) );
    return;
  }

  if (!PLAYERP( str ))
  {
    be_quiet();
    return;
  }
  if ((tmp = member_array( str, borrowed )) != -1)
    if (!borrowed[ tmp + 1 ])
      borrowed = borrowed[ 0..tmp-1 ] + borrowed[ tmp + 2..MYCKE ];
    else if (!interactive( environment( borrowed[ tmp + 1 ] ) )
	     || environment( environment( borrowed[ tmp + 1 ] ) )
	     != environment())
    {
      borrowed[ tmp + 1 ]->seppuku();
      borrowed = borrowed[ 0..tmp-1 ] + borrowed[ tmp+2..MYCKE ];
    }
    else
    {
      SAY( "The clerk says: Well " +
	   environment( borrowed[ tmp + 1 ] )->query_name() + " has " +
	   "borrowed the booklet about " + capitalize( str ) + ".",
	   ({ this_object() }) );
      return;
    }
  if (present( BOOKLET_ID, this_player() ))
  {
    SAY( "The clerk says: No you have to return the booklet you're reading " +
	 "first.", ({ this_object() }) );
    return;
  }
  if (careless_wizards[ str ])
    SAY( "The clerk sighs deeply.\n" +
	 "The clerk says: That careless wizard.", ({ this_object() }) );
  SAY( "The clerk starts searching the shelves for information about the " +
       "wizard " + capitalize( str ) + ".", ({ this_object() }) );
  call_out( "find_wizard", random( 10 ), str );
}

void find_wizard( string str )
{
  object booklet;

  if (!stringp( str ))
    return;
  booklet = clone_object( BOOKLET );
  if (!booklet->set_wizard( str ))
  {
    destruct( booklet );
    if (careless_wizards[ str ])
    {
      if (!random(5))
      {
	if (!(careless_wizards[ str ][0]--))
	  m_delete( careless_wizards, str );
	save_object( SAVE_FILE );
      }
      SAY( "The clerk sighs deeply.\n" +
	   "The clerk says: As I suspected, " + careless_wizards[ str ][1] +
	   " hasn't even written a booklet.", ({ this_object() }) );
      return;
    }
    SAY( "The clerk sighs deeply.\n" +
	 "The clerk says: I don't have any booklet made by " +
	 capitalize( str ) + ". If you want me to list all wizards I have " +
	 "about, just tell me to.", ({ this_object() }) );
    return;
  }
  SAY( "The clerk smiles.\n" +
       "The clerk says: here we are.", ({ this_object() }) );
  if (!this_player())
  {
    SAY( "The clerk sighs.\n" +
	 "The clerk says: well I can always help someone else",
	 ({ this_object() }) );
    return;
  }
  if (!present( this_player(), environment() ))
  {
    SAY( "The clerk sighs deeply.\n" +
	 "The clerk says: where did " +
	 capitalize( (string )this_player()->query_name() ) + " go ?",
	 ({ this_object() }) );
    destruct( booklet );
    return;
  }
  SAY( "The clerk gives " + booklet->short() + " to " +
       this_player()->query_name() + ".",
       ({ this_object(), this_player() }) );
  TELL( this_player(), "The clerk gives you " + booklet->short() + "." );
  move_object( booklet, this_player() );
  borrowed += ({ str, booklet });
}

void mumble()
{
  SAY( "The clerk mumbles: Wait a moment, while searching.",
       ({ this_object() }) );
}

void list_wizards()
{
  string a, b;
  string *subjects;
  int c;
  
  if (BUSY)
  {
    call_out( "mumble", 0 );
    return;
  }
  subjects = get_dir( DATA_PATH );
  for (c=0; c < sizeof( subjects ); c++)
    subjects[c] = capitalize( subjects[c] );
  SAY( "The clerk says: I know that these excellent wizards have built " +
	"things:", ({ this_object() }) );
  call_out( "continue_listing_wizards", 2, 
	   explode( LINE_WRAP( implode( subjects[ 0 .. sizeof( subjects ) - 2 ]
				       , ", " ) + " and " +
				subjects[ sizeof( subjects ) - 1 ] + "." ),
		   "\n" ) );
}

void continue_listing_wizards( string *data )
{
  SAY( data[0], ({ this_object() }) );
  if (sizeof( data ) > 1)
    call_out( "continue_listing_wizards", 2, data[ 1..MYCKE ] );
}

void catch_tell( string str )
{
  string *broken_str;
  string a,b;
  
  if (!stringp( str ))
    return;
  str = lower_case( str );
  while (sscanf( str, "%s:%s", a, b ) == 2)
    str = a + b;
  while (sscanf( str, "%s.%s", a, b ) == 2)
    str = a + b;
  while (sscanf( str, "%s\"%s", a, b ) == 2)
    str = a + b;
  while (sscanf( str, "%s\n%s", a, b ) == 2)
    str = a + b;
  broken_str = explode( str, " " );
  if (sizeof( broken_str ) < 3)
    return;
  if (!present( broken_str[0], environment() ))
    return;
  if (broken_str[1] == "says")
    if (broken_str[2] == "list")
      call_out( "list_wizards", 0 );
    else if (sizeof( broken_str ) == 3)
      call_out( "check_for_wizard", 0, broken_str[2] );
    else if (!BUSY)
      call_out( "be_quiet", 0 );
    else;
  else if (broken_str[1] == "tells" && broken_str[2] == "you")
    if (broken_str[3] == "list")
      call_out( "list_wizards", 0 );
    else if (sizeof( broken_str ) == 4)
      call_out( "check_for_wizard", 0, broken_str[3] );
}

status tell_kludge( string str )
{
  if (stringp(str) && str[0..4]=="clerk" &&
      find_living( "clerk" ) != this_object())
  {
    catch_tell( this_player()->query_name() + " tells you " + str );
    return 1;
  }
  return 0;
}

/* systemjox */

string long()
{
  return LINE_WRAP( "This is a rather elderly clerk. " +
		    (BUSY ? "He's searching the shelves for a booklet " +
		     "about some wizards wonderful creations right now."
		     : "He's eagerly waiting to assist you in finding out " +
		     "about the many wonderful places you can visit in " +
		     "this mud.") );
}

string short()
{
  return "a clerk";
}

status id( string str )
{
  return stringp( str ) && lower_case( str ) == "clerk" ||
    lower_case( str ) == "turistinformator";
}

status query_gender()
{
  return 1;
}

string query_name()
{
  return "Clerk";
}

string query_real_name()
{
  return "clerk";
}

status hit_player()
{
  if (UNDER_ATTACK)
    return 0;
  SAY( "As " + this_player()->query_name() + " tries to attack the clerk, " +
       "the clerk ducks behind the counter with surprising swiftness.",
       ({ this_object(), this_player() }) );
  TELL( this_player(), "As you try to attack the clerk he ducks behind the " +
	"counter." );
  call_out( "pulling_lever", 4 );
  return 0;
}

void pulling_lever()
{
  int c;
  object *inv;
  
  SAY( "Suddenly the floor drops from beneth your feet.",
       ({ this_object() }) );
  inv = all_inventory( environment() );
  for( c=0; c < sizeof( inv ); c++)
    if (inv[c] != this_object() )
    {
      inv[c]->move_player( "down#" + MAZE_ROOM );
      move_object( inv[c], MAZE_ROOM );
    }
}
  
status add_weight()
{
  call_out( "check_inv", 1 );
  return 1;
}

status can_put_and_get()
{
  call_out( "check_inv", 1 );
  return 1;
}

void check_inv()
{
  object *inv, booklet;
  int c;
  string tmp;

  if (booklet = present( BOOKLET_ID, this_object() ))
  {
    tmp = (string )booklet->short();
    destruct( booklet );
    if (booklet = present( BOOKLET_ID, this_object() ))
    {
      destruct( booklet );
      while (booklet = present( BOOKLET_ID, this_object() ))
	destruct( booklet );
      SAY( "The clerks moves some booklets to their proper place in the " +
	   "shelves.", ({ this_object() }) );
    }
    else
      SAY( "The clerk says: thank you.\n" +
	   "The clerk puts back " + tmp + ".",
	   ({ this_object() }) );
    call_out( "check_inv", 0 );
  }
  else
  {
    inv = all_inventory( this_object() );
    if (!sizeof( inv ))
      return;
    SAY( "The clerk says: I don't need this.", ({ this_object() }) );
    for (c=0; c < sizeof( inv ); c++)
    {
      if (inv[c]->short())
	SAY( "The clerk drops " + inv[c]->short() + ".", ({ this_object() }) );
      move_object( inv[c], environment() );
    }
  }
}   
      
status query_npc()
{ return 1; }

void careless_pp()
{
  if (careless_wizards[ this_player()->query_real_name() ])
    careless_wizards[ this_player()->query_real_name() ][0]++;
  else
    careless_wizards[ this_player()->query_real_name() ] =
      ({ 1, this_player()->query_pronoun() });
  save_object( SAVE_FILE );
}

status under_attack()
{ return UNDER_ATTACK; }
      
