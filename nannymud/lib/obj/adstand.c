#define DIR "etc/News/"
#define WRITER "/secure/adwriter"

void long() {
  write("Here you can write ads, personals, announcements or even\n");
  write("comic pages in the NannyMUD Times.\n");
  write("Just type: write <ad/per/ann/com>\n");
  write("The cost is 10 coins per line, max 17 lines.\n");
} /* long */

string short() {
  return "A small stand saying: The NannyMUD Times";
} /* short */

void init() {
  add_action("write_something","write");
} /* init */

/* write_something - selfexplanatory
 * string str - what to write
 */
status write_something(string str) {
  string skrap;
  notify_fail("Write what??\n");
  if (!str)
    return 0;
  if(sscanf(str,"ad%s",skrap)) {
    clone_object(WRITER)->set_dir(DIR+"Ad");
    return 1;
  }
  if(sscanf(str,"per%s",skrap)) {
    clone_object(WRITER)->set_dir(DIR+"Per");
    return 1;
  }
  if(sscanf(str,"ann%s",skrap)) {
    clone_object(WRITER)->set_dir(DIR+"Ann");
    return 1;
  }
  if(sscanf(str,"com%s",skrap)) {
    clone_object(WRITER)->set_dir(DIR+"Com");
    return 1;
  }
  return 0;
} /* write_something */

status id(string str) {
  return str == "stand"; 
} /* id */


