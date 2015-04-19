#define DATE "Mar 14, 1992"

short() {
  return "the NannyMUD Times (" + DATE + ")";
}

query_name()
{
  return "newspaper";
}

long() {
  if (this_player())
    this_player()->more("/etc/NEWSPAPER");	/* Padrone */
}

init() {
  add_action("read", "read");
}

id(str) {
  return str == "newspaper" || str == "paper" || str == "times" ||
    str == "the times";
}

read(str) {
    if (!id(str)) {
      notify_fail("Read what?\n"); /* Pell */
      return 0;
    }
    say(call_other(this_player(), "query_name") +
	" reads the NannyMUD Times.\n");
    long();
    return 1;
}

query_weight() { return 1; }

get() { return 1; }

query_value() { return 0; }


