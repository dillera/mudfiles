#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if (!arg)
  {
    set_light(1);
    add_property("outdoors");
    add_property("bridge");
    add_property("author", "brom");
    add_exit("west", "plane9");
    add_exit("east", "old_fence");
    set_short("On a bridge");
    set_long("You stand on Cauchy bridge. It is old and it creaks a lot.");
    add_item("bridge",
	     "Once upon the time, Cauchy the Wizard built this bridge. He planned to\n"+
	     "create a splendid area east of here, but judging from the poor shape\n"+
	     "bridge is in, you somehow doubt that he ever did.\n");
    if (load_object("/players/moonchild/river/river2"))
      add_exit("down", "/players/moonchild/river/river2");
  }
}
