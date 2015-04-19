inherit "room/room";

reset(arg) {
    if (arg) return;

    set_light(5);
    short_desc = "Forest";
    no_castle_flag = 0;
    long_desc = 
        "The trees are STILL high. It is still cold. There used to be a glass\n"
        + "wall here before. It has moved, so the forest must have grown.\n"
        + "To the west there is a high white wall. Thumping sounds are coming\n"
        + "from the north.\n";
    dest_dir = 
        ({
        "players/lazlar/forest_9", "south",
        "players/lazlar/entrance", "north",
        });
}

/*
    remove the comments around the "room is modified()" code
    below to prevent changes you have done to this room to
    to be lost by using the room maker
*/
/*
room_is_modified() {
    return 1;
}
*/

/*
 make your additions below this comment, do NOT remove this comment
--END-ROOM-MAKER-CODE--
*/

