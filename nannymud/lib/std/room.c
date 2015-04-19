/* Written by Profezzorn */

#pragma strict_types
#pragma save_types

inherit "/std/basic_room";

/*** variables ***/

private mixed *local_objects = ({});

/*** useful defines ***/

#define ERROR(X) tell_object(find_player(creator(this_object())||"")||this_player()||this_object(),"ERROR "+(X))

/*** Setup functions ***/

/* This function adds an object to this room
 * if you give a filename, this file will be cloned every
 * reset in this room, unless it is already in the room.
 * If the file corresponds to a monster or unique item, it will
 * only be recloned if the previous one was destructed.
 * The filename will be sent to eval() before cloning.
 * (eval is also permitted to return an object)
 * Also, giving "a <something>" as argument will result in
 * reset calling /std/lib to make <something>.
 * Last but not least, You can add an object directly to the
 * room, this will not make it reclone itself next reset,
 * merly inform the room that this object was cloned here and
 * it may be destructed by clean_up.
 */
void add_object(mixed ob)
{
  mixed o;
  if(objectp(ob))
  {
    o=ob;
  }else{
    if(sscanf(ob,"a %s",ob) || sscanf(ob,"an %s",ob))
    {
      ob="@/std/lib->make("+ob+")";
    }else{
      ob=expand_file_name(ob);
    }
    o=eval(ob);
    if(stringp(o)) o=clone_object(o);
  }
  move_object(o,this_object());
  local_objects+=({ob,o});
}

/* This function removes something previously added with
 * add_object, it does not destruct any cloned objects however, 
 * merely makes sure new ones aren't cloned
 */
void remove_object(mixed ob)
{
  int i;
  if(stringp(ob))
  {
    if(sscanf(ob,"a %s",ob) || sscanf(ob,"an %s",ob))
    {
      ob="@/std/lib->make("+ob+")";
    }else{
      ob=expand_file_name(ob);
    }
  }

  i=member_array(ob,local_objects);
  if(i==-1) return;
  i&=~1; /* Make i even */
  local_objects=local_objects[0..i-1]+local_objects[i+2..0x7ffffff];
}


/*** functions called by mudlib / driver ***/
mixed *query_local_objects() { return local_objects; }

/* All commands go through here */
int do_cmd(string what)
{
  if(::do_cmd(what)) return 1;

  switch(query_verb())
  {
  case "north":
  case "south":
  case "east":
  case "west":
  case "northeast":
  case "northwest":
  case "southeast":
  case "southwest":
    if(query_inorout()==1)
    {
      notify_fail("You walk into a wall. *BONK*\n");
      break;
    }
  case "up":
  case "down":
    notify_fail("You cannot go that way.\n");
  }
  return 0;
}

/* Extra items,
 * Redefine and call query_mentioned_in_long(what) when making
 * 'hard-coded' items in standard rooms
 * This function was called 'xitems' but was renamed to
 * query_mentioned in long for compatibility reasons.
 */
string query_mentioned_in_long(string what)
{
  string s;

  if(what)
  {
    switch(query_inorout())
    {
    case 1: /* Indoors */
      switch(what)
      {
      case "floor":
      case "wall":
      case "walls":
      case "ceiling":
	return "You see nothing special.\n";
      }
      break;
    case 2: /* Outdoors */
      switch(what)
      {
      case "sky":
      case "ground":
	return "You see nothing special.\n";
      }
      break;
    }

    s=replace(lower_case(::query_long()),"\n"," ");
    if(sscanf(s,"%*s"+what+"%*s"))
    {
      return "You see nothing special.\n";
    }
  }
}

/* Don't forget to call ::reset(arg), or add_object
 * will not work properly
 */
void reset(int arg)
{
  int e;
  mixed o;
  ::reset(arg);
  for(e=0;e<sizeof(local_objects);e+=2)
  {
    if(local_objects[e+1])
    {
      if(environment(local_objects[e+1])==this_object())
	continue;

      if(local_objects[e+1]->query_living()) /* Walking npc */
	continue;

      /* Unique object */
      if(local_objects[e+1]->query_property("unique_item"))
	continue;
    }else{
      if(!local_objects[e])
      {
	/* cut away some dead meat */
	local_objects=local_objects[0..e-1]+
	  local_objects[e+2..sizeof(local_objects)];
	e-=2;
	continue;
      }
    }

    o=eval(local_objects[e]);
    if(stringp(o)) o=clone_object(o);
    local_objects[e+1]=o;
    move_object(local_objects[e+1],this_object());
  }
}

/* Clean up takes into account which objects where cloned
 * in this room and which were not.
 */
int clean_up(int refs)
{
  int e;

  if(_isclone(this_object())) return 0;
  if(refs > 1 || query_property("no_clean_up")) return 1;

  /* Are there objects in this room not created in this room ? */
  if(sizeof(all_inventory() - local_objects)) return 1;

  /* Are there objects elsewhere which we need to keep track of ? */
  for(e=1;e<sizeof(local_objects);e+=2)
  {
    if(local_objects[e] &&
       environment(local_objects[e]) != this_object() &&
       (local_objects[e]->query_living() ||
	local_objects[e]->query_property("unique_item")))
      return 1;
  }
  destruct(this_object());
}
