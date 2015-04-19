/* Handles if the other half of the door disappears - Padrone Aug 6 1992 */

/* My own version includes set_door_short() and set_no_short() /Traste 95 */

string direction, destination, back_direction;
object partner_door;
string key_code;
string key_type;
string door_long;
string door_short = "A door to the";
int can_lock, is_locked, is_closed;
int no_short = 0;

query_dir() { return direction;}
query_closed() { return is_closed; }
query_locked() { return is_locked; }
query_code() { return key_code; }

set_dir( str) { direction = str;}
set_code( str) { key_code = str;}
set_type( str) { key_type = str;}

set_door(obj) {
  partner_door = obj;
    destination = file_name(environment(partner_door));
    back_direction = obj->query_direction();
}

set_closed( val) 
{ 
  if ( is_closed != val ) {
    tell_room( environment( this_object()), "The " + direction + " door ");
    if ( val)
      tell_room( environment( this_object()),"closes.\n");
    else
      tell_room( environment( this_object()),"opens.\n");
  }
is_closed = val; 
}

set_locked( val) { 
  if ( is_locked != val )
    door_sound("Klick!");
  is_locked = val; 
}

set_can_lock( val) { can_lock = val; }

set_both_status()
{
  check_partner();
  call_other( partner_door, "set_closed", is_closed);
  call_other( partner_door, "set_locked", is_locked);
  call_other( partner_door, "set_can_lock", can_lock);
  call_other( partner_door, "set_type", key_type);
  call_other( partner_door, "set_code", key_code);
  call_other( partner_door, "set_door_long", door_long);
}

set_door_long( str)
{
  door_long = str;
}

set_door_short(str)
{
  door_short = str;
}

set_no_short()
{
  no_short = 1;
}

string door_room;

set_all( str) 
{
  if (!str)
    return 0;
  if ( sscanf( str, "%s^%s^%s^%s^%d^%d^%d", door_room, direction, key_type, 
	       key_code, is_closed, is_locked, can_lock) == 7 ||
      sscanf( str, "%s %s %s %s %d %d %d", door_room, direction, key_type, 
	       key_code, is_closed, is_locked, can_lock) == 7 ) {
    if( key_type == "0")
      key_type = 0;
    move_object(this_object(), door_room);
    return 1;
  }
 return 0;
}

query_room()
{
    return door_room;
}

player_enters( str)
{
  if (environment(this_object())) /*Check added by Qqqq*/
  tell_room( environment( this_object()), str + " enters through the " +
	    direction + " door.\n");
}

door_sound( str)
{
  tell_room( environment( this_object()), 
	    str + " is heard from the " + direction + " door.\n");
}

both_door_sound( str)
{
  door_sound( str);
  check_partner();
  call_other( partner_door, "door_sound", str);
}
     
short()
{
  if (!no_short)
    return door_short + " " + direction + (is_closed?" ( closed )":" ( open )");
  else
    return 0;
}

/* Profezzorn */
query_in_long_short()
{
  return short();
}

long(what_to_look_at)
{
  string str;
  if(what_to_look_at !="door" && what_to_look_at)
  {
    if(sscanf(what_to_look_at,"%slock",what_to_look_at))
    {
      if ( key_type)
	write( "On the door there is a " + key_type + " lock.\n");
      else
	write("The lock looks very ordinary.\n");
        return;   /* Bug here fixed By Celeborn */
    }
  }

  if(door_long)  
    {
      write(door_long);
    } else 
    {
      write("A normal wooden door\n");
    }
 
  if(key_type)
    write( "On the door there is a " + key_type + " lock.\n");
  
  write( "The door is " + (is_closed?"closed!\n":"open.\n"));
  
  if ( 7 == random(20) ) 
  {
    write("You notice a small sign stating:\n");
    write("A product from Hebol Inc.\n");
  }

  if ( 42 == random(64) )
  {
    write("You notice a small scrathing at the bottom of the door stating:\n"+
	  "'Celeborn was here'\n");
  }
  if ( 4711 == random(11)+4705 )
  {
    write("You notice a small scrathing at the bottom of the door stating:\n"+
	  "'Milamber was here'\n");
  }
}

id(strang)
{
  if ( ( strang == "door" ) ||
      ( strang == direction + " lock" ) ||
      ( strang == key_type + " lock" ) ||
      ( strang == "lock" ) ||
      ( strang == direction + " door" ) ||
      ( strang == "H_door" ) ) 
    return 1;
  return 0;
}

init()
{
  if ( direction ) 
  {
    add_action( "go_door", direction);
  }
  add_action( "go", "go");
  
  add_action( "close", "close");
  add_action( "open", "open");
  
  if ( can_lock ) 
  {
    add_action( "unlock", "unlock");
    add_action( "lock", "lock");
  }
}

go( str)
{
  int tmp;
  
  if (!str)
    return 0;
  
  tmp = this_door(str);
  if ( tmp == 2 ) 
  {
    go_door();
    return 1;
  } else if ( tmp == 1 )
    return 1;
  else
    return 0;
}

open( str)
{
  int tmp;
  
  if ( !str)
    return 0;
  
  tmp = this_door( str);
  if ( tmp == 2 ) {
    open_door();
    return 1;
  }
  else 
    if ( tmp == 1 )
      return 1;
    else
      return 0;
}

close( str)
{
  int tmp;
  
  if ( !str)
    return 0;
  
  tmp = this_door( str);
  if ( tmp == 2 ) {
    close_door();
    return 1;
  }
  else 
    if ( tmp == 1 )
      return 1;
    else
      return 0;
}

unlock( str)
{
  object ob;
  int tmp;
  string type, door;
  
  if ( !str)
    return 0;
  
  if ( str == "door" ) {
    write( "Unlock the door with what?\n");
    return 1;
  }
  type = 0;
  
  if (sscanf(str, "%s with %s", door, type) == 2) {
    tmp = this_door( door);
    if ( tmp != 2 ) 
      return tmp;
    
    tmp = this_key( type);
    if ( tmp != 2 )
      return 1;
    ob = get_key( type);
    
    unlock_door( ob);
    return 1;
  }
}    

lock( str)
{
  object ob;
  int tmp;
  string type, door;
  
  if ( !str)
    return 0;
  
  if ( str == "door" ) {
    write( "Lock the door with what?\n");
    return 1;
  }
  type = 0;
  
  if (sscanf(str, "%s with %s", door, type) == 2) {
    tmp = this_door( door);
    if ( tmp != 2 ) 
      return tmp;
    
    tmp = this_key( type);
    if ( tmp != 2 )
      return 1;
    ob = get_key( type);
    
    lock_door( ob);
    return 1;
  }
}    


/* this_door( str) tests if the argument arg refers to this door.
   Return values: 0 => str not refering to a door,
   1 => str refering to a door but there are several alternatives tells the
   player that this has occurred.
   2 => str refers to this door.
   */

this_door( str)
{
  string type;

  
  if ( !str)
    return 0;
  
  if ( (sscanf(str, "%s", type) == 1) && ( type == "door" ) ) {
    if ( number_of_doors() == 1)
      return 2;
    else
      which_door();
    
    return 1;
  }
  
  if ( (sscanf(str, "%s door", type) == 1) && ( type == direction ) ) {
    return 2;
  }
  
  return 0;
}

this_key( str)
{
  string type;
  
  if ( !str)
    return 0;
  
  if ( (sscanf(str, "%s", type) == 1) && ( type == "key" ) ) {
    if ( number_of_keys() == 1)
      return 2;
    else
      if ( number_of_keys() == 0) {
	write("You haven't got a key!\n");
	return 1;
      }
      else
	which_key();
    
    return 1;
  }
  
  if (sscanf(str, "%s key", type) == 1) {
    if ( present( type + " key", this_player()))
      return 2;
    write("You haven't got such a key!\n");
  }
  
  return 0;
}


open_door()
{
  string str;
  int tmp;
  
  if ( ! is_closed ) {
    write("But why? It's already open!\n");
    return;
  }
  
  if ( is_locked ) 
    write("You can't open the " + direction + " door, it's locked!\n");
  else  {
    write("You open the " + direction + " door.\n");
    set_closed( 0);
    check_partner();
    call_other( partner_door, "set_closed", is_closed);
  }
  
  return;
}

close_door()
{
  string str;
  int tmp;
  
  if ( is_closed ) {
    write("But why? It's already closed!\n");
    return;
  }
  
  if ( is_locked )
    write("You can't close the " + direction + " door, it's locked!\n");
  else {
    write("You close the " + direction + " door.\n");
    set_closed(1);
    check_partner();
    partner_door->set_closed(is_closed);
  }
  return;
}


lock_door( key)
{
  string str;
  int tmp;

  if ( is_locked ) {
    write("But why? It's already locked!\n");
    return;
  }
  if ( key)
    str = call_other( key, "query_code", 0);
  
  if ( ( str == key_code ) || ( str == "zap" ) ) {
    write("\nYou lock the " + direction + " door.\n");
    set_locked( 1);
    check_partner();
    partner_door->set_locked(is_locked);
  }
  else
    write("The key doesn't fit!\n");
  
  return;
}


unlock_door( key)
{
  string str;
  
  if ( ! is_locked ){
    write("But why? It's already unlocked!\n");
    return;
  }

  if ( key)
    str = call_other( key, "query_code", 0);
  
  if ( ( str == key_code ) || ( str == "zap" ) ) {
    write("You unlock the " + direction + " door.\n");
    set_locked( 0);
    check_partner();
    partner_door->set_locked(is_locked);
  }
  else
    write("The key doesn't fit!\n");
  
  return;
}

go_door()
{
  string str;
  
  if ( is_closed ) {
    write("You can't do that, the door is closed.\n");
    return 1;
  }
  
  check_partner();
  if(environment()->check_for_door_passage(query_verb())) return 1;

  str = call_other( this_player(), "query_name", 0);
  call_other( partner_door, "player_enters", str);
  write( "You go through the " + direction + " door.\n");
  call_other(this_player(), "move_player", 
	     direction + "#" + call_other(partner_door, "query_room"));

  return 1;
	       
}

number_of_doors()
{
  object ob;
  int num_door;
  
  num_door = 0;
  
  ob = first_inventory(environment(this_object()));
  while(ob) {
    if (call_other(ob, "id", "H_door")) 
      num_door += 1;
    
    ob = next_inventory(ob);
  }
  return num_door;
}

which_door()
{
  object ob;
  int num_door;
  int tmp_num;
  string str;
  
  tmp_num = 0;
  num_door = number_of_doors();
  
  write("Which door do you mean");
  
  ob = first_inventory(environment(this_object()));
  while(ob) {
    if (call_other(ob, "id", "H_door")) {
      tmp_num += 1;
      str = call_other(ob, "query_dir", 0);
      
      if ( tmp_num == num_door )
	write(" or the " + str + " door?\n");
      else 
	write( ", the " + str + " door");
    }
    if ( tmp_num == num_door ) return;
    
    ob = next_inventory(ob);
  }
}

number_of_keys()
{
  object ob;
  int num_key;
  
  num_key = 0;
  
  ob = first_inventory(this_player());
  while(ob) {
    if (call_other(ob, "id", "H_key")) 
      num_key += 1;
    
    ob = next_inventory(ob);
  }
  return num_key;
}

which_key()
{
  object ob;
  int num_key;
  int tmp_num;
  string str;

  tmp_num = 0;
  num_key = number_of_keys();
  
  write("Which key do you mean");
  
  ob = first_inventory(this_player());
  while(ob) {
    if (call_other(ob, "id", "H_key")) {
      tmp_num += 1;
      str = call_other(ob, "query_type", 0);
      
      if ( tmp_num == num_key )
	write(" or the " + str + " key?\n");
      else 
	write( ", the " + str + " key");
    }
    if ( tmp_num == num_key ) return;
    
    ob = next_inventory(ob);
  }
}


get_key(type)
{
  object ob;
  int num_key;
  int tmp_num;
  string str;
  string k_type;
  
  if ( ! (sscanf( type, "%s key", k_type) == 1) )
    k_type = 0;

  tmp_num = 0;
  num_key = number_of_keys();
  
  ob = first_inventory(this_player());
  while(ob) {
    if (call_other(ob, "id", "key")) {
      tmp_num += 1;
      str = call_other(ob, "query_type", 0);
      
      if  ( ( str == k_type ) || ( ! k_type ) )
	return ob;
    }
    if ( tmp_num == num_key ) return 0;
    
    ob = next_inventory(ob);
  }
}

void check_partner() {
  if (!partner_door) {
    /* The other half of this door has been destructed! */
    partner_door = clone_object("obj/door");
    move_object(partner_door, destination);
    partner_door->set_all(destination + " " + back_direction + " " +
			  key_type + " " + key_code + " " + is_closed +
			      " " + is_locked + " " + can_lock);
    partner_door->set_door_long("A (new) door leading " + back_direction + ".\n");
    partner_door->set_door(this_object());
  }
} /* check_partner */

string query_direction() { return direction; }

string query_destination() { return destination; }

reset(arg) {
  if (arg)
    check_partner();
}

