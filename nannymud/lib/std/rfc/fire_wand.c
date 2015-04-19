/* 
 * This is an example wand, a wand of fire.
 * It inherits the general wand and use set_* for configurations.
 * Here is also an example of an effect.
 *
 */

inherit "std/rfc/gwand";

reset(arg) {
  if (arg) return;
  set_name("wand");
  add_alias("wand of fire");
  add_alias("fire wand");
  set_commands(({"zap", "use", "shake"}));
  set_short("A fire wand");
  set_long("The wand is made out of polished wood with brass rings\n"+
	   "as decoration. There is a faint scale-like pattern cut into\n"+
	   "the wood.\n");
  add_decor("ring", "It is a smooth-worn brass ring.\n");
  add_decor("rings", "It is a smooth-worn brass ring.\n");
  add_decor("pattern", "It looks like the scales of a Dragon.\n");
  set_fail_chance(50);   /* promille */
  set_chain_chance(100); /* promille */
  set_weight(1);
  set_value(1000);
  set_charges(100);
} /* reset */

wand_fail(att, vic) {
  write("The wand explodes!\n");
  say(att->query_name() +
      "'s wand explodes!\n");
  destruct(this_object());
} /* wand_fail */

wand_effect(att, vic, pre, nr) {
  
  if (!objectp(vic)) return;
  if (!nr) {
    write("You hit " +
	  vic -> query_name() +
	  " with the fire bolt from the wand!\n");
    tell_object(vic,
		att -> query_name() +
		" hits you with a bolt of fire from the wand!\n");
    tell_room(environment(att),
	      vic -> query_name() +
	      " is hit by a fire bolt from " +
	      att -> query_name() +
	      "'s wand!\n",
	      ({ att, vic }));
  } else {
    tell_object(vic,
		"The bolt bounces and hits you!\n");
    tell_room(environment(att),
	      "The bolt bounces and hits " +
	      vic -> query_name() +
	      "!\n",
	      ({ vic }));
  }
  
  vic -> attack_ob(att);
  vic -> hit_player(random(30) + 1);
} /* wand_effect */
