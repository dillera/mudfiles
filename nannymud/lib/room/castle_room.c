query_dumb_castle_check() { return 1;}

long() { return write("A huge room, that contains all the new wizard's castles.\nIf you are a new wizard with a portable castle, drop it here.\n"+
"When you have built a full and ready area in your castle, then mail/talk to\nan arch before moving it out to the place where you want it\nin the player's world.\n");
       }

short() { return "The newbie wizard's castle room"; }

init() {
  add_action("north","north");
}

north() {
  this_player()->move_player("north#room/wiz_hall");
  return 1;
}

reset() {
set_light(1);
}
