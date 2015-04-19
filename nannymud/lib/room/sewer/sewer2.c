#include <std_mudlib.h>
inherit I_ROOM;

reset(arg)
{
  if (!arg)
  {
    add_property("indoors");
    add_property("author", "brom");
    add_exit("east", "sewer3");
    add_exit("west", "sewer1");
    set_short("In the sewers");
    set_long
      ("You are walking in thigh-deep putrescence; the smell is unbelievable.\n");
    add_item( ({ "water", "putrescence" }),
	     "This was water once; now it is a mixture of water, household wastes\n"+
	     "and worse things.\n");
    add_item( ({ "waste", "wastes", "household waste", "household wastes" }),
	     "Old rotten cabbage, maggot-ridden pieces of meet and such.\n");
    add_item( ({ "thing", "things" }),
	     "Faeces bloated with gases of the most unpleasant kind.\n");
    add_item( ({ "shit", "faeces" }),
	     "Well!\n");
  }
  if (!present("rat"))
  {
    object r;
    r = "/room/sewer/sewer1"->make_rat();
    move_object(r, this_object());
  }
} /* reset */
