inherit "/std/room";

reset(arg) {
  if (arg) return;

  set_light(1);
  set_short("Treasury");
  set_long("This is the treasury.");
  add_object("bigdoor");
  add_object("a diamond");
} 
