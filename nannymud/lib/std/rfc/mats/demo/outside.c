inherit "/std/room";

reset(arg) {
  if (arg) return;

  set_light(1);
  set_short("Outside");
  set_long("This is the room outside the treasury.");
  add_object("bigdoor");
  add_object("@make_key()");
} 

make_key() {
  object k;
  k = clone_object("/std/basic_thing");
  k -> set_name("big key");
  k -> add_alias("key");
  k -> set_short("a big key");
  k -> add_property("code","foo");
  return k;
}
