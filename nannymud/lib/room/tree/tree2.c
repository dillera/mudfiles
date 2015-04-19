/* Earendil -- July 23, 95 */
inherit "/std/basic_room";
#define LB(X) sprintf("%-78=s\n", X)

void reset(int arg) {
  if( !arg ) {
    set_light(1);
    add_property("outdoors");
    add_exit("down", "/room/tree/tree1");
    set_short("Entrance to Elfin Abode");
    set_long(LB("Entrance to Elfin Abode\n"+
		"You are now high up in the tree, more than a hundred feet "+ 
		"off the ground, and yet nowhere near the top.  Here the stairway "+
		"comes to a brief landing, before leading up into the wooden structure "+
		"above you.  You are in awe at both the size of the tree and of the "+
		"fortress built within it, which sprawls out chaotically, its only "+
		"plan apparently being based on the the growth of branches around "+
		"it."));
    add_item("tree", 
	     LB("The tree stretches up for hundreds of more feet.  "+
		"This is obviously no ordinary tree; some magic must be involved.  "+
		"The wooden fortress is now just above you."));
    add_item(({"boughs", "branches", "branch" }), 
	     LB("This part of the tree is bare of branches, though just above you "+
		"are several enormous ones, making the main supports for the lower "+
		"floors of the Elfin fortress."));
    add_item("leaves", 
	     "The tree is thick and bushy, with many leaves around you.");
    add_item(({"stairs", "stair", "stairway", "landing"}),
	     LB("You are on a long but narrow landing.  At one end the stair starts "+
		"to wind away downwards, while at the other it goes another 20 feet "+
		"before entering the wooden structure."));
    add_item(({ "structure", "abode", "castle", "wooden structure", "building",
		  "fortress" }),
	     LB("The structure sprawls out in all directions, north, south, east, "
		+"west, and up;  it covers an immense amount of space, and is "+
		"built in almost a whimsical manner, with conflicting "+
		"styles of architecture: here a tall spire, there a gothic "+
		"vault, elsewhere a homey hall.  The entrance is directly above "+
		"you."));
  }
}

