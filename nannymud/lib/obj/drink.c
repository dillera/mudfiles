/* /obj/drink.c - Kept for compability, 
   use /obj/soft_drink.c or /obj/alco_drink.c instead.
   */

inherit "/obj/alco_drink";

string message;

set_message(str) 
{ 
  drinker_mess = str +"\n";
}

set_value(arg)
{
  if (intp(arg))
    value = arg;
  else
    if (stringp(arg))
      if (sscanf(arg, "%s#%s#%s#%d#%d#%d", name, short_desc, drinker_mess,
		 heal, value, strength) != 6)
	return 0;
      else
      {
	alias = "the "+ name;
	alt_name = "a"+ (vocp(name) ? "n " : " ")+ name;
	drinker_mess += "\n";
	set_short("",1); /* fix longdesc */
	make_id(); 
      }
  return 1;
}

drink(arg)
{
  message && !drinker_mess && (drinker_mess = message);
  return ::drink(arg);
}
