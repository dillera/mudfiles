/* Earendil -- July 23, 95 */
inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("outside");
    
    set_short("In the Giant Ash");
    set_long(LB("In the Giant Ash\n"+
	"You are in the lower boughs of a gigantic ash tree.  "+
	"Around you branches spread out in all directions.  Above you "+
	"about fifty feet is the lower section of some sort of large "+
	"wooden castle, built around the trunk of the tree.  Just a "+
	"little ways above you a stair starts, "+
    	"winding around the trunk and leading up, making your climb "+
    	"much easier."));

    add_item("castle", LB("The castle, made entirely of wood apparently, "+
	"sprawls about the upper reaches of the ash tree.  Whoever it "+
	"is that lives within its reaches, you are positive they would "+
	"have little love of tree-killers."));
    add_item("stair", 
	"A stairway starts a short distance up the tree trunk.\n");

    add_exit("down", "/room/big_tree");
    add_exit("up", "/room/tree/tree2");
  }
}

mapping out_dirs = ([ "north" : 0, "south" : 0, "east" : 0,
  "west" : 0, "northeast" : 0, "southeast" : 0, "southwest" : 0,
  "northwest" : 0,
]);

void init() {
  ::init();
  map_array(m_indices(out_dirs), "__add_action", this_object());
}

void __add_action(string dir) { add_action("branch", dir); }

mapping opposite = ([
	"north":"south", "east":"west", "west":"east","south":"north",
	"northeast":"southwest", "southwest":"northeast",
	"southeast":"northwest", "northwest":"southeast", 
]);

int branch() {
  object b;

  b = out_dirs[query_verb()];
  if( !b ) {
    b = clone_object("/room/tree/branch");
    b->add_exit(opposite[query_verb()], "/room/tree/tree1");
    out_dirs[query_verb()] = b;
  }
  this_player()->move_player(query_verb(), b);
  return 1;
}
