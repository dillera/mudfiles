short() {
  return "a small note marked 'About NannyMUD'";
}

query_name() {
  return "note";
}

long() {
  write("The note says:\n");
  cat("/etc/NANNYMUD");
}

    
init() {
  add_action("read"); add_verb("read");
}
 
id(str) {
  return str == "note" || str == "small note" || str == "paper" || str == "lyspaper";
}
 
read(str) {
  if (!id(str)) {
    notify_fail("Read what?\n"); /* Pell */
    return 0;
  } 
  say(call_other(this_player(), "query_name") + " looks at the small note.\n");
  long();
  return 1;
}
 
query_weight() {
  return 1;
}
 
get() {
  return 1;
}
 
query_value() {
  return 4;
}
