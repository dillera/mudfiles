/* You know, a few comments on who wrote what is not wrong.
 *
 * Brom 960124
 */

inherit "std/room";

object ferry_obj;
object controler;
object whereob;
int at_sea;
string ferryman_name= "The ferryman";
string boat_type="ferry";
string facade ="/std/special/ferry/ferry_outer";

void arrive(string dest);
void set_ferryman_name(string str);
void set_boat_type(string str);
void set_facade(string str);

void reset(int arg) {
  if(arg) return;
  add_property("outdoors");
  add_property("no_fight");
  add_property("no_clean_up");
  set_light(1);
  set_short("A ferry");
  set_long("This is a ferry. There is a sign nailed to the rail.\n");
  add_item("view","@exa_view()");
  add_item("sign",
	   "The sign is nailed to the rail. There is some text "+
	   "written on it.");
  add_item_cmd("read","sign","@read_sign()");
  add_command("sail %s","@ask_sail()");
  add_command("arrival","@arrival()");
  set_facade("guilds/magicians/obj/ferry_outer.c");
} /* reset */

set_ferryman_name(str) {
  ferryman_name=capitalize(str);
} /* set_ferryman_name */

set_boat_type(str) {
  boat_type=str;
} /* set_boat_type */

query_boat_type(str) {
  return boat_type;
} /* query_boat_type */

set_facade(str) {
  facade=str;
} /* set_facade */

wherestr() {
  return controler->place_name(whereob);
} /* wherestr */

read_sign() {
  return controler->read_sign();
} /* read_sign */

string exa_view() {
  if (at_sea) {
    return "You see the wild sea.\n";
  }
  return "You can see "+wherestr()+" outside the "+boat_type+".\n";
} /* exa_view */

set_up(controlob,start) {
  controler=controlob;
  whereob=start;
  arrive(start);
} /* set_up */

int query_sailing() {
  return at_sea;
} /* query_sailing */

where_is() {
  return whereob;
} /* where_is */

info(str) {
  tell_room(this_object(),ferryman_name+" says: "+str);
}
int arrival() {
  int time;
  say(this_player()->query_name()+" asks "+ferryman_name+" how long the journey will last.\n");
  write("You ask "+ferryman_name+" how long the journey will last.\n");
  if (!at_sea) {
    info("We are not sailing.\n");
    return 1;
  }
  time=find_call_out("arrive");
  /* Now if something goes wrong, time will be -1.
   * This must be handled.
   */
  if (time < 0)
  {
    info("We are not sailing.\n");
    return 1;
  }

  info("We will arrive in "+time+" seconds.\n");
  return 1;
} /* arrival */

void leave(object dest) {
  tell_room(dest,"The "+boat_type+" leaves.\n");
  remove_exit("out");
  at_sea=1;
  info("We will sail now.\n");
  if (objectp(ferry_obj)) destruct(ferry_obj);
} /* leave */

void arrive(string dest) {
  catch(ferry_obj=load_object(facade));
  /* Now, here is the code to handle the case when the destination did not load.
   * Also added the 'catch' above.
   * Brom 960124.
   */
  if (!objectp(ferry_obj))
  {
    object item;

    info("The ship is attacked by a sea monster and swallowed! You fall overboard,\n"+
	 "and after floating around for what seems like forever, you arrive here.\n");
    while (item = first_inventory(this_object()))
      if (interactive(item))
	item -> move_player("floating on the waves", "room/sea");
      else
	destruct(item);
    destruct(this_object());
    return;
  }

  move_object(ferry_obj,dest);
  ferry_obj->set_inner(this_object());
  tell_room(dest,"A "+boat_type+" arrives.\n");
  add_exit("out",dest);
  info("We have arrived at "+wherestr()+".\n");
  at_sea=0;
} /* arrive */

ask_sail(arg,str) {
  object harbour;
  say(this_player()->query_name()+" asks "+ferryman_name+" to sail to "+str+".\n");
  write("You ask "+ferryman_name+" to sail to "+str+".\n");
  harbour=controler->is_harbour(str);
  if (harbour) {
    if (at_sea) {
      info("We are already sailing.\n");
      return 1;
    }
    if (harbour==whereob) {
      info("We are already here.\n");
      return 1;
    }
    return sail(harbour);
  }
  info("I dont know any harbour called that.\n");
  return 1;
}

int sail(object whereto) {
  int time;
  /* ... let us assume that the controler does not exist.
   * Brom 960124
   */
  if (!objectp(controler))
    return 1;
  
  time=controler->time_to_sail(whereob,whereto);
  leave(whereob);
  whereob=whereto;
  call_out("arrive",time,whereob);
  return 1;
} /* sail */

object ferryman() {
  object man;
  man=clone_object("/std/basic_monster");
  man->set_name("avanar");
  man->add_alias("ferryman");
  man->set_level(3);
  man->set_short("Avanar");
  man->set_long("Avanar is the ferryman. He has taken Fombrith "+
		     "across the sea more times than can be counted.");
  man->set_male();
  return man;
} /* ferryman */
