inherit "/obj/thing";
string my_cap_name;

reset (arg)
{  
  ::reset (arg);
  if (arg) return;
  my_cap_name="Noone";  
  set_short("A marble statue\n");
  set_long("The marble statue resembles "+my_cap_name+", who was devoted\n"+
	   "enough to donate money to NannyMud to ensure her future.\n");
  set_name("statue");
  set_alias("marble statue");
} /* reset */

get() {
  return 0;
}

set_the_name(name) {
  my_cap_name=capitalize(name);
  set_short("Statue of "+my_cap_name);
  set_alias("Statue of "+my_cap_name);
  set_long("The marble statue resembles "+my_cap_name+", who was devoted\n"+
	   "enough to donate money to NannyMud to ensure her future.\n");
  /* Added by Banshee 950519 */
} /* set_the_name */
