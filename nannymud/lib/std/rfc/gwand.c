/* wand.c 
 *
 * A generic wand/rod/staff/magic item ...
 *
 * Here is a list of functions.
 +--------------------------------
 * set_chain_chance	Set chance (in promille) that the effect will bounce further.
 * set_charges		Set number of uses.
 * set_commands		Set what command you can use it with.
 * set_effect_ob		Set path to file where the effect function is
 * set_fail_chance		Set risc in promille for failure. 
 * set_fail_ob		Set path to file with the failure function.
 * set_long		Set the long description.
 * set_name		Set the name.
 * set_short		Set the short description.
 * set_value		Set the value. 
 * set_weight		Set the weight.
 * 
 * add_alias		Add an alias to the item.
 * add_command		Add a command with wich you can use the item.
 * add_decor		Add some decor; you can look at it, but not use it.
 * 			To simplify removal, decor has no aliases.
 * long			Writes the long description.
 * query_long		Returns the long description.
 * query_magic		Returns 1. :)
 * query_name		Returns the name.
 * query_value		Returns the value.
 * rm_command		Remove one of the commands with which you can use the item.
 * 			You will then have to move the item from and to the user.
 * rm_decor		Remove a decor.
 * short	        Returns the short description.
 * wand_ok              This function is called when the item is used to see if it
 *                      is ok. Return 0 if not.
 * wand_effect          This function is called for the effect.
 * wand_fail            This function is called for the failure.
 *  
 */
/* +-==================================================================-+ */

static inherit "obj/property";

static mapping names = ([]); /* Names and aliases. Used for identification. */
static mapping decor = ([]); /* Things to examine on the item. */
static mapping data  = ([]); /* Stats and data. */
static mapping comm  = ([]); /* Commands for usage */

get() { return 1; }
drop() { return 0; }
query_magic() { return 1; }

set_name(str) { 
  if (str) {
    names["name"] = str; 
    names[str] = 1;
  }
} /* set_name */

query_name() { return names["name"]; }

/* Eat a single string as well as an array of strings. */
add_alias(name) {
  if (!name) return;
  if (pointerp(name))
    map_array(name, "add_alias", this_object());
  else
    names[name] = 1;
} /* add_alias */

id(str) { return !!(names[str] || decor[str]); /* convert to int ... */ }

/* Use decor for things that are on the wand but you do not want as part
 * of the identification when the wand in use. 
 * Example: The wand has a leaf-pattern. You want people to be able to examine
 *   that, with 'exa pattern', but you do not want people use the wand by
 *   'use pattern'.
 */

/* No aliases on the decor; it makes rm'ing it horrendous. */
add_decor(part, desc) {
  if (!stringp(desc) || !stringp(part)) return;
  decor[part] = desc;
} /* add_decor */

/*
rm_decor(part) {
  if (pointerp(part))
    map_array(part, "rm_decor", this_object());
  else
    decor = m_delete(decor, part);
} /* remove_decor */

/* Sanity checks on weight and value. */
set_weight(arg) { if (arg > 0) data["weight"] = arg; }
set_value(arg) { if (arg > 0) data["value"] = arg; }
query_value() { return data["value"]; }

set_short(str) { 
  data["short"] = str; 
  data["long"] = str + ".\n";
} /* set_short */
short() { return data["short"]; }

set_long(str) { data["long"] = str; }
query_long() { return data["long"]; }
  
long(str) {
  mixed desc;
  if (!(desc=names[str]) && !(desc=decor[str])) return;
  if (intp(desc)) desc = query_long();
  write(desc);
} /* long */

add_command(str) { if (str) comm[str] = 1; }

rm_command(str)  { if (str) comm = m_delete(comm, str); }

set_commands(arr) {
  if (!pointerp(arr)) return;
  comm = ([]);
  map_array(arr, "add_command", this_object());
} /* set_commands */

set_effect_ob(where) { 
  if (where)
    data["effect_ob"] = where;
  else
    data["effect_ob"] = this_object();
} /* set_effect_ob */

set_fail_ob(where) { 
  if (where)
    data["fail_ob"] = where;
  else
    data["fail_ob"] = this_object();
} /* set_effect_ob */

set_charges(arg) { if (arg > 0) data["charges"] = arg; }

/* Chanses in promille */
set_fail_chance(arg) {
  if (arg < 0) arg = 0;
  if (arg > 1000) arg = 1000;
  data["fail_chance"] = arg;
} /* set_fail_chance */

set_chain_chance(arg) {
  if (arg < 0) arg = 0;
  if (arg > 1000) arg = 1000;
  data["chain_chance"] = arg;
} /* set_chain_chance */

/* +-------------------------------------------------------------------+ */

reset(arg) {
  if (arg) return;
  set_name("wand");
  set_short("An unspecified wand");
  set_weight(1);
  set_commands(({"wave", "shake"}));
} /* reset */

init() {
  string *cmds;
  int i;

  cmds = m_indices(comm);
  for (i=sizeof(cmds)-1; i>=0; i--)
    add_action("cmd_use", cmds[i]);
} /* init */

cmd_use(str) {
  string action;
  string item, target;
  object victim;
  object *chain;
  int i, s, nr_b, old_target, new_target;

  if (!str) {
    notify_fail(capitalize(action) + " what?\n");
    return 0;
  }
  action = query_verb();
  if (sscanf(str, "%s at %s", item, target) != 2) {
    notify_fail("Try to " + action + " at someone.\n");
    return 0;
  }
  if (!names[item]) {
    notify_fail(capitalize(action) + " what?\n");
    return 0;
  }
  victim = find_living(target);
  if (!victim || !present(victim, environment(this_player()))) {
    write("At whom, did you say?\n");
    return 1;
  }

  /* Check busy. */
  if (this_player() -> check_busy(2)) {
    write("Not yet.\n");
    return 1;
  }

  /* Check for charges left, and no_magic of environment. */
  if (!data["charges"] ||
      environment(this_player()) -> query_property("no_magic")) {
    write("Nothing happens.\n");
    say(this_player()->query_name() +
	" tries to use the " + item + ", but nothing happens.\n", 
	this_player());
    return 1;
  }

  /* Additional checks. */
  if (!wand_ok(this_player(), victim, environment(this_player()))) return 1;

  data["charges"]--; 

  if ((1+random(1000)) < data["fail_chance"]) {
    if (data["fail_ob"])
      data["fail_ob"] -> wand_effect(this_player(), victim);
    else
      wand_fail(this_player(), victim);
    return 1;
  }

  if (!(1+random(1000) < data["chain_chance"])) {
    if (data["effect_ob"])
      data["effect_ob"] -> wand_effect(this_player(), victim);
    else
      wand_effect(this_player(), victim);
    return 1;
  }
  
  /* Now, the chain code. */
  chain = map_array(all_inventory(environment(this_player())),
		    "is_living", this_object()) - ({ this_player() });
  chain = ({ this_player() }) + chain; /* The player is now on index 0 */
  while ((1+random(1000)) < data["chain_chance"] &&
	 nr_b++ < 10); /* Guaranteed to terminate, thank you. */
  if (!nr_b) {
    /* Just a normal one-time hit. */
    if (data["effect_ob"])
      data["effect_ob"] -> wand_effect(this_player(), victim);
    else
      wand_effect(this_player(), victim);
    return 1;
  }
  s = sizeof(chain);
  old_target = 0; /* First hit NOT on the original user. */
  for (i = 0; i<s; i++) {
    new_target = pick_one(s, old_target);
    /* BEWARE! Arguments might be zero in function call below! */
    if (data["effect_ob"])
      data["effect_ob"] -> wand_effect(this_player(), victim);
    else
      wand_effect(this_player(), chain[new_target], chain[old_target], i);
    old_target = new_target;
  }
  return 1;
} /* cmd_use */

is_living(o) { return living(o); }

pick_one(size, current) {
  int res;
  res = random(size);
  if (res == current) res = (res+1) % size;
  return res;
}

wand_ok(user, victim, env) { return 1; } /* Standard is 'ok' */

wand_fail(attacker, victim) { } 

wand_effect(attacker, victim, old, nr) {
  /* "attacker" is the user of the wand.
   * "victim" is the one being treated this time.
   * "old" is the one who was treated last time in the chain, if any.
   * "nr" is non-zero in a chain of hits.
   * BEWARE that those variables might be zero!
   *
   * Another caveat: YOU will have to check for PK if your item can be used
   * to hurt players. If the wand can be used on players, I recommend that
   * you don't let it bounce; shall the chain suddenly skip someone just
   * because he's not a PK? Naah.
   */
}
     
/* +-==========================================================================-+ */
/* This is a debug function and will be rm'ed in final version */

show_data() {
  int i;
  mixed *ind, *val;
  
  write("names\n-----\n");
  ind = m_indices(names);
  val = m_values(names);
  if (ind)
    for (i=sizeof(ind)-1; i>= 0; --i)
      write(sprintf("%-15s - ", ind[i]) + val[i] + "\n");

  write("data\n-----\n");
  ind = m_indices(data);
  val = m_values(data);
  if (ind)
    for (i=sizeof(ind)-1; i>= 0; --i)
      write(sprintf("%-15s - ", ind[i]) + val[i] + "\n");

  write("decor\n-----\n");
  ind = m_indices(decor);
  val = m_values(decor);
  if (ind)
    for (i=sizeof(ind)-1; i>= 0; --i)
      write(sprintf("%-15s - ", ind[i]) + val[i] + "\n");

  write("commands\n-----\n");
  ind = m_indices(comm);
  val = m_values(comm);
  if (ind)
    for (i=sizeof(ind)-1; i>= 0; --i)
      write(sprintf("%-15s - ", ind[i]) + val[i] + "\n");
  return 1;
}
/* EOF */
