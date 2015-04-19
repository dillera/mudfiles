inherit "/std/rfc/door";

reset(arg) {
  ::reset(arg);
  if (arg) return;

  set_name("big door");
  set_first("west","outside");
  set_second("east","treasury");
  set_lock("foo");
}
