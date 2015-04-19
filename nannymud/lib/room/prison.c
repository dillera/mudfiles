/* Rewritten using add and set functions in room.c. Banshee 950216 */

inherit "/room/room";

reset(arg) {
  if (!arg) {
    set_light(1);
    set_no_castle_flag(1);
    set_short("The local prison");
    set_long("You are in the local prison.\n");
    set_realm("NT");
    set_indoors();
  }
}

init() {
  add_action("quit","quit");
  add_action("quit", "shout");
}

quit() { return 1; }

query_property(arg) { return arg=="no_magic";  }

query_prevent_shadow() { return "JAJAMENSAN!!!"; }
