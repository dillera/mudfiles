/* Rewritten using set functions in room.c. Banshee 950216 */

#pragma strict_types
#pragma save_type

inherit "/room/room";

void reset(int arg)
{
  if(arg) return;
  set_light(1);
  set_short("The inner office");
  set_long("You're in the inner bailiff's office, there's nothing of interest here.\n");
  set_realm("NT");
}
