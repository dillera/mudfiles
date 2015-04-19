/* stone.c
 *
 * The __extra_look property in this object makes it necessary
 * to have it as a separate object.
 *
 */

inherit "std/basic_thing";

reset(arg)
{
  if (!arg)
  {
    set_name("stone");
    set_value(8);
    set_weight(10);
    set_short("A stone");
    set_long("This is a plain stone. Not very valuable, and quite heavy.\n");

    /* If we want the player carring the stone to look a little extra
       it is easily added with a property. */
    add_property("__extra_look", "@look_func()");
  }
} /* reset */


look_func()
{
  if (!objectp(environment(this_object()))) return 0;
  return (environment(this_object()) -> query_pronoun() +
	  " is carring a big stone. What a fool"); 
} /* look_func */
